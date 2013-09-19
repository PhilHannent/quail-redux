#ifndef QUAILWINGLIBEVENTLOOP_H
#define QUAILWINGLIBEVENTLOOP_H

#include "QtCore/qt_windows.h"
//#include "QtCore/qcoreevent.h"
#include <QAbstractEventDispatcher>
#include <QCoreApplication>
#include <QHash>
#include <QMutex>
#include <QThread>
#include <QSocketNotifier>
#include <QStack>
#include <QVector>

typedef struct _GMainContext GMainContext;

// forward declaration
LRESULT QT_WIN_CALLBACK qt_internal_proc(HWND hwnd, UINT message, WPARAM wp, LPARAM lp);
int qt_msectime();

struct GPostEventSource;
struct GSocketNotifierSource;
struct GTimerSource;
struct GIdleTimerSource;

struct QSockNot {
    QSocketNotifier *obj;
    int fd;
};

typedef QHash<int, QSockNot *> QSNDict;

struct WinTimerInfo {                           // internal timer info
    QObject *dispatcher;
    int timerId;
    int interval;
    Qt::TimerType timerType;
    quint64 timeout;                            // - when to actually fire
    QObject *obj;                               // - object to receive events
    bool inTimerEvent;
    int fastTimerId;
};

class QZeroTimerEvent : public QTimerEvent
{
public:
    explicit inline QZeroTimerEvent(int timerId)
        : QTimerEvent(timerId)
    { t = QEvent::ZeroTimerEvent; }
};

typedef QList<WinTimerInfo*>  WinTimerVec;      // vector of TimerInfo structs
typedef QHash<int, WinTimerInfo*> WinTimerDict; // fast dict of timers

class QPostEvent
{
public:
    QObject *receiver;
    QEvent *event;
    int priority;
    inline QPostEvent()
        : receiver(0), event(0), priority(0)
    { }
    inline QPostEvent(QObject *r, QEvent *e, int p)
        : receiver(r), event(e), priority(p)
    { }
};

inline bool operator<(const QPostEvent &first, const QPostEvent &second)
{
    return first.priority > second.priority;
}

// This class holds the list of posted events.
//  The list has to be kept sorted by priority
class QPostEventList : public QVector<QPostEvent>
{
public:
    // recursion == recursion count for sendPostedEvents()
    int recursion;

    // sendOffset == the current event to start sending
    int startOffset;
    // insertionOffset == set by sendPostedEvents to tell postEvent() where to start insertions
    int insertionOffset;

    QMutex mutex;

    inline QPostEventList()
        : QVector<QPostEvent>(), recursion(0), startOffset(0), insertionOffset(0)
    { }

    void addEvent(const QPostEvent &ev) {
        int priority = ev.priority;
        if (isEmpty() ||
            last().priority >= priority ||
            insertionOffset >= size()) {
            // optimization: we can simply append if the last event in
            // the queue has higher or equal priority
            append(ev);
        } else {
            // insert event in descending priority order, using upper
            // bound for a given priority (to ensure proper ordering
            // of events with the same priority)
            QPostEventList::iterator at = std::upper_bound(begin() + insertionOffset, end(), ev);
            insert(at, ev);
        }
    }
private:
    //hides because they do not keep that list sorted. addEvent must be used
    using QVector<QPostEvent>::append;
    using QVector<QPostEvent>::insert;
};

class QThreadPrivate : public QObject
{
    Q_OBJECT

public:
    QThreadPrivate(QThreadData *d = 0);
    //~QThreadPrivate();

    void setPriority(QThread::Priority prio);

    mutable QMutex mutex;
    QAtomicInt quitLockRef;

    bool running;
    bool finished;
    bool isInFinish; //when in QThreadPrivate::finish

    bool exited;
    int returnCode;

    uint stackSize;
    QThread::Priority priority;

    static QThread *threadForId(int id);

#ifdef Q_OS_UNIX
    pthread_t thread_id;
    QWaitCondition thread_done;

    static void *start(void *arg);
    static void finish(void *);

#endif // Q_OS_UNIX

#ifdef Q_OS_WIN
    static unsigned int __stdcall start(void *);
    static void finish(void *, bool lockAnyway=true);

    Qt::HANDLE handle;
    unsigned int id;
    int waiters;
    bool terminationEnabled, terminatePending;
# endif
    QThreadData *data;

    static void createEventDispatcher(QThreadData *data);

    void ref()
    {
        quitLockRef.ref();
    }

    void deref()
    {
        if (!quitLockRef.deref() && running) {
            QCoreApplication::instance()->postEvent(this, new QEvent(QEvent::Quit));
        }
    }
};

class QThreadData
{
    QAtomicInt _ref;

public:
    QThreadData(int initialRefCount = 1);
    ~QThreadData();

    static QThreadData *current();
    static void clearCurrentThreadData();
//    static QThreadData *get2(QThread *thread)
//    { Q_ASSERT_X(thread != 0, "QThread", "internal error"); return thread->d_func()->data; }

    void ref();
    void deref();
    inline bool hasEventDispatcher() const
    { return eventDispatcher.load() != 0; }

    bool canWaitLocked()
    {
        QMutexLocker locker(&postEventList.mutex);
        return canWait;
    }

    QThread *thread;
    Qt::HANDLE threadId;
    bool quitNow;
    int loopLevel;
    QAtomicPointer<QAbstractEventDispatcher> eventDispatcher;
    QStack<QEventLoop *> eventLoops;
    QPostEventList postEventList;
    bool canWait;
    QVector<void *> tls;
    bool isAdopted;
};

class QuailEventDispatcherWinGlib : public QAbstractEventDispatcher
{
    Q_OBJECT
    void createInternalHwnd();

public:
    QuailEventDispatcherWinGlib(QObject *parent = 0);
    ~QuailEventDispatcherWinGlib();

    bool QT_ENSURE_STACK_ALIGNED_FOR_SSE processEvents(QEventLoop::ProcessEventsFlags flags);
    bool hasPendingEvents();

    void registerSocketNotifier(QSocketNotifier *notifier);
    void unregisterSocketNotifier(QSocketNotifier *notifier);

    void registerTimer(int timerId, int interval, Qt::TimerType timerType, QObject *object);
    bool unregisterTimer(int timerId);
    bool unregisterTimers(QObject *object);
    QList<TimerInfo> registeredTimers(QObject *object) const;

    bool registerEventNotifier(QWinEventNotifier *notifier);
    void unregisterEventNotifier(QWinEventNotifier *notifier);
    void activateEventNotifiers();

    int remainingTime(int timerId);

    void wakeUp();
    void interrupt();
    void flush();

    void startingUp();
    void closingDown();

    bool event(QEvent *e);

    /* Glib */
    static bool versionSupported();

    GMainContext *mainContext;
    GPostEventSource *postEventSource;
    GSocketNotifierSource *socketNotifierSource;
    GTimerSource *timerSource;
    GIdleTimerSource *idleTimerSource;

    void runTimersOnceWithNormalPriority();

    DWORD threadId;

    bool interruptFlag;

    // internal window handle used for socketnotifiers/timers/etc
    HWND internalHwnd;
    HHOOK getMessageHook;

    // for controlling when to send posted events
    QAtomicInt serialNumber;
    int lastSerialNumber, sendPostedEventsWindowsTimerId;
    QAtomicInt wakeUps;

    // timers
    WinTimerVec timerVec;
    WinTimerDict timerDict;
    void registerTimer(WinTimerInfo *t);
    void unregisterTimer(WinTimerInfo *t);
    void sendTimerEvent(int timerId);

    // socket notifiers
    QSNDict sn_read;
    QSNDict sn_write;
    QSNDict sn_except;
    void doWsaAsyncSelect(int socket);

    QList<QWinEventNotifier *> winEventNotifierList;
    void activateEventNotifier(QWinEventNotifier * wen);

    QList<MSG> queuedUserInputEvents;
    QList<MSG> queuedSocketEvents;

protected:
    virtual void sendPostedEvents();

private:
    friend LRESULT QT_WIN_CALLBACK qt_internal_proc(HWND hwnd, UINT message, WPARAM wp, LPARAM lp);
    friend LRESULT QT_WIN_CALLBACK qt_GetMessageHook(int, WPARAM, LPARAM);
};


#endif // QUAILWINGLIBEVENTLOOP_H
