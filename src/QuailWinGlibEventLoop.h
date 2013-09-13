#ifndef QUAILWINGLIBEVENTLOOP_H
#define QUAILWINGLIBEVENTLOOP_H

#include "QtCore/qt_windows.h"
#include <QAbstractEventDispatcher>
#include <QHash>

typedef struct _GMainContext GMainContext;

class QWinEventNotifier;
class QuailEventDispatcherWinGlibPrivate;

// forward declaration
LRESULT QT_WIN_CALLBACK qt_internal_proc(HWND hwnd, UINT message, WPARAM wp, LPARAM lp);
int qt_msectime();


class QuailEventDispatcherWinGlib : public QAbstractEventDispatcher
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QuailEventDispatcherWinGlib)
    void createInternalHwnd();

public:
    explicit QuailEventDispatcherWinGlib(QObject parent = 0);
    explicit QuailEventDispatcherWinGlib(GMainContext *context, QObject *parent = 0);
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

protected:
    QEventDispatcherWinGlib(QuailEventDispatcherWinGlibPrivate &dd, QObject *parent = 0);
    virtual void sendPostedEvents();

private:
    friend LRESULT QT_WIN_CALLBACK qt_internal_proc(HWND hwnd, UINT message, WPARAM wp, LPARAM lp);
    friend LRESULT QT_WIN_CALLBACK qt_GetMessageHook(int, WPARAM, LPARAM);
};

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

class Q_CORE_EXPORT QuailEventDispatcherWinGlibPrivate : public QAbstractEventDispatcherPrivate
{
    Q_DECLARE_PUBLIC(QEventDispatcherWinGlib)
public:
    QEventDispatcherWinGlibPrivate();
    QEventDispatcherWinGlibPrivate(GMainContext *context = 0);
    ~QEventDispatcherWinGlibPrivate();

    GMainContext *mainContext;
    GPostEventSource *postEventSource;
    GSocketNotifierSource *socketNotifierSource;
    GTimerSource *timerSource;
    GIdleTimerSource *idleTimerSource;

    void runTimersOnceWithNormalPriority();

    DWORD threadId;

    bool interrupt;

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
};

#endif // QUAILWINGLIBEVENTLOOP_H
