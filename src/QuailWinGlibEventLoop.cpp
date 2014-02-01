#include "QuailWinGlibEventLoop.h"

#include <QCoreApplication>
#include <glib.h>
#include <QDateTime>
#include <QDebug>
#include <QThread>
#include <QMutex>
#include <QMutexLocker>
#include "qsystemlibrary.h"
#include <QWinEventNotifier>
#include <QElapsedTimer>
#include "quailtimerinfolist.h"

#include <sys/time.h>
//#include <windows.h>
#define WM_SOCKET (WM_USER + 1)

struct GPollFDWithQSocketNotifier
{
    GPollFD pollfd;
    QSocketNotifier *socketNotifier;
};

struct GSocketNotifierSource
{
    GSource source;
    QList<GPollFDWithQSocketNotifier *> pollfds;
};

static gboolean socketNotifierSourcePrepare(GSource *, gint *timeout)
{
    qDebug() << "socketNotifierSourcePrepare";
    if (timeout)
        *timeout = -1;
    return false;
}

static gboolean socketNotifierSourceCheck(GSource *source)
{
    qDebug() << "socketNotifierSourceCheck";
    GSocketNotifierSource *src = reinterpret_cast<GSocketNotifierSource *>(source);

    bool pending = false;
    for (int i = 0; !pending && i < src->pollfds.count(); ++i) {
        GPollFDWithQSocketNotifier *p = src->pollfds.at(i);

        if (p->pollfd.revents & G_IO_NVAL) {
            // disable the invalid socket notifier
            static const char *t[] = { "Read", "Write", "Exception" };
            qWarning("QSocketNotifier: Invalid socket %d and type '%s', disabling...",
                     p->pollfd.fd, t[int(p->socketNotifier->type())]);
            // ### note, modifies src->pollfds!
            p->socketNotifier->setEnabled(false);
        }

        pending = ((p->pollfd.revents & p->pollfd.events) != 0);
    }

    return pending;
}

static gboolean socketNotifierSourceDispatch(GSource *source, GSourceFunc, gpointer)
{
    qDebug() << "socketNotifierSourceDispatch";
    QEvent event(QEvent::SockAct);

    GSocketNotifierSource *src = reinterpret_cast<GSocketNotifierSource *>(source);
    for (int i = 0; i < src->pollfds.count(); ++i) {
        GPollFDWithQSocketNotifier *p = src->pollfds.at(i);

        if ((p->pollfd.revents & p->pollfd.events) != 0)
            QCoreApplication::sendEvent(p->socketNotifier, &event);
    }

    return true; // ??? don't remove, right?
}

static GSourceFuncs socketNotifierSourceFuncs = {
    socketNotifierSourcePrepare,
    socketNotifierSourceCheck,
    socketNotifierSourceDispatch,
    NULL,
    NULL,
    NULL
};

struct GTimerSource
{
    GSource source;
    QTimerInfoList timerList;
    QEventLoop::ProcessEventsFlags processEventsFlags;
    bool runWithIdlePriority;
};

static gboolean timerSourcePrepareHelper(GTimerSource *src, gint *timeout)
{
    timespec tv = { 0l, 0l };
    if (!(src->processEventsFlags & QEventLoop::X11ExcludeTimers) && src->timerList.timerWait(tv))
        *timeout = (tv.tv_sec * 1000) + ((tv.tv_nsec + 999999) / 1000 / 1000);
    else
        *timeout = -1;

    return (*timeout == 0);
}

static gboolean timerSourceCheckHelper(GTimerSource *src)
{
    if (src->timerList.isEmpty()
        || (src->processEventsFlags & QEventLoop::X11ExcludeTimers))
        return false;

    if (src->timerList.updateCurrentTime() < src->timerList.first()->timeout)
        return false;

    return true;
}

static gboolean timerSourcePrepare(GSource *source, gint *timeout)
{
    gint dummy;
    if (!timeout)
        timeout = &dummy;

    GTimerSource *src = reinterpret_cast<GTimerSource *>(source);
    if (src->runWithIdlePriority) {
        if (timeout)
            *timeout = -1;
        return false;
    }

    return timerSourcePrepareHelper(src, timeout);
}

static gboolean timerSourceCheck(GSource *source)
{
    GTimerSource *src = reinterpret_cast<GTimerSource *>(source);
    if (src->runWithIdlePriority)
        return false;
    return timerSourceCheckHelper(src);
}

static gboolean timerSourceDispatch(GSource *source, GSourceFunc, gpointer)
{
    GTimerSource *timerSource = reinterpret_cast<GTimerSource *>(source);
    if (timerSource->processEventsFlags & QEventLoop::X11ExcludeTimers)
        return true;
    timerSource->runWithIdlePriority = true;
    (void) timerSource->timerList.activateTimers();
    return true; // ??? don't remove, right again?
}

static GSourceFuncs timerSourceFuncs = {
    timerSourcePrepare,
    timerSourceCheck,
    timerSourceDispatch,
    NULL,
    NULL,
    NULL
};

struct GIdleTimerSource
{
    GSource source;
    GTimerSource *timerSource;
};

static gboolean idleTimerSourcePrepare(GSource *source, gint *timeout)
{
    qDebug() << "QuailEventDispatcherWinGlib::idleTimerSourcePrepare";
    GIdleTimerSource *idleTimerSource = reinterpret_cast<GIdleTimerSource *>(source);
    GTimerSource *timerSource = idleTimerSource->timerSource;
    if (!timerSource->runWithIdlePriority) {
        // Yield to the normal priority timer source
        if (timeout)
            *timeout = -1;
        return false;
    }

    return timerSourcePrepareHelper(timerSource, timeout);
}

static gboolean idleTimerSourceCheck(GSource *source)
{
    qDebug() << "QuailEventDispatcherWinGlib::idleTimerSourceCheck";
    GIdleTimerSource *idleTimerSource = reinterpret_cast<GIdleTimerSource *>(source);
    GTimerSource *timerSource = idleTimerSource->timerSource;
    if (!timerSource->runWithIdlePriority) {
        // Yield to the normal priority timer source
        return false;
    }
    return timerSourceCheckHelper(timerSource);
}

static gboolean idleTimerSourceDispatch(GSource *source, GSourceFunc, gpointer)
{

    GTimerSource *timerSource = reinterpret_cast<GIdleTimerSource *>(source)->timerSource;
    (void) timerSourceDispatch(&timerSource->source, 0, 0);
    return true;
}

static GSourceFuncs idleTimerSourceFuncs = {
    idleTimerSourcePrepare,
    idleTimerSourceCheck,
    idleTimerSourceDispatch,
    NULL,
    NULL,
    NULL
};

struct GPostEventSource
{
    GSource source;
    QAtomicInt serialNumber;
    int lastSerialNumber;
    QuailEventDispatcherWinGlib *d;
};

static gboolean postEventSourcePrepare(GSource *s, gint */*timeout*/)
{
    qDebug() << "QuailEventDispatcherWinGlib::postEventSourcePrepare";
//    QThreadData *data = QThreadData::current();
//    if (!data)
//        return false;

//    gint dummy;
//    if (!timeout)
//        timeout = &dummy;
//    const bool canWait = data->canWaitLocked();
//    *timeout = canWait ? -1 : 0;

    GPostEventSource *source = reinterpret_cast<GPostEventSource *>(s);
//    return (!canWait
//            || (source->serialNumber.load() != source->lastSerialNumber));
    return source->serialNumber.load() != source->lastSerialNumber;

}

static gboolean postEventSourceCheck(GSource *source)
{
    return postEventSourcePrepare(source, 0);
}

static gboolean postEventSourceDispatch(GSource *s, GSourceFunc, gpointer)
{
    qDebug() << "QuailEventDispatcherWinGlib::postEventSourceDispatch";
    GPostEventSource *source = reinterpret_cast<GPostEventSource *>(s);
    source->lastSerialNumber = source->serialNumber.load();
    QCoreApplication::sendPostedEvents();
    source->d->runTimersOnceWithNormalPriority();
    return true; // i dunno, george...
}

static GSourceFuncs postEventSourceFuncs = {
    postEventSourcePrepare,
    postEventSourceCheck,
    postEventSourceDispatch,
    NULL,
    NULL,
    NULL
};


HINSTANCE qWinAppInst();
extern uint qGlobalPostedEventsCount();

#ifndef TIME_KILL_SYNCHRONOUS
#  define TIME_KILL_SYNCHRONOUS 0x0100
#endif

#ifndef QS_RAWINPUT
#  define QS_RAWINPUT 0x0400
#endif

#ifndef WM_TOUCH
#  define WM_TOUCH 0x0240
#endif
#ifndef QT_NO_GESTURES
#ifndef WM_GESTURE
#  define WM_GESTURE 0x0119
#endif
#ifndef WM_GESTURENOTIFY
#  define WM_GESTURENOTIFY 0x011A
#endif
#endif // QT_NO_GESTURES

enum {
    WM_QT_SOCKETNOTIFIER = WM_USER,
    WM_QT_SENDPOSTEDEVENTS = WM_USER + 1,
    SendPostedEventsWindowsTimerId = ~1u
};

#if !defined(DWORD_PTR) && !defined(Q_OS_WIN64)
#define DWORD_PTR DWORD
#endif

typedef MMRESULT(WINAPI *ptimeSetEvent)(UINT, UINT, LPTIMECALLBACK, DWORD_PTR, UINT);
typedef MMRESULT(WINAPI *ptimeKillEvent)(UINT);

static ptimeSetEvent qtimeSetEvent = 0;
static ptimeKillEvent qtimeKillEvent = 0;

LRESULT QT_WIN_CALLBACK qt_internal_proc(HWND hwnd, UINT message, WPARAM wp, LPARAM lp);

static void resolveTimerAPI()
{
    qDebug() << "QuailEventDispatcherWinGlib::resolveTimerAPI";
    static bool triedResolve = false;
    if (!triedResolve) {
#ifndef QT_NO_THREAD
        //QMutexLocker locker(QMutexPool::globalInstanceGet(&triedResolve));
        QMutex locker;
        triedResolve = locker.tryLock();
        if (!triedResolve)
            return;
#endif
        triedResolve = true;
#  if defined(_MSC_VER) && _MSC_VER >= 1700
        if (QSysInfo::WindowsVersion >= QSysInfo::WV_WINDOWS8) {
#  else
        {
#  endif
        qtimeSetEvent = (ptimeSetEvent)QSystemLibrary::resolve(QLatin1String("winmm"), "timeSetEvent");
        qtimeKillEvent = (ptimeKillEvent)QSystemLibrary::resolve(QLatin1String("winmm"), "timeKillEvent");
        }
    }
    qDebug() << "QuailEventDispatcherWinGlib::resolveTimerAPI.end";
}

QuailEventDispatcherWinGlib::QuailEventDispatcherWinGlib(QObject *parent)
    : QAbstractEventDispatcher(parent), mainContext(0),
      threadId(GetCurrentThreadId()), interruptFlag(false),
      internalHwnd(0), getMessageHook(0),
      serialNumber(0), lastSerialNumber(0),
      sendPostedEventsWindowsTimerId(0), wakeUps(0)
{
    qDebug() << "QuailEventDispatcherWinGlib::QuailEventDispatcherWinGlib";
    resolveTimerAPI();
    qDebug() << "QuailEventDispatcherWinGlib::QuailEventDispatcherWinGlib.1";
#if GLIB_MAJOR_VERSION == 2 && GLIB_MINOR_VERSION < 32
    if (qEnvironmentVariableIsEmpty("QT_NO_THREADED_GLIB")) {
        static QBasicMutex mutex;
        QMutexLocker locker(&mutex);
//        if (!g_thread_supported())
//            g_thread_init(NULL);
    }
#endif

    if (mainContext) {
        g_main_context_ref(mainContext);
    } else {
        QCoreApplication *app = QCoreApplication::instance();
        if (app && QThread::currentThread() == app->thread()) {
            mainContext = g_main_context_default();
            g_main_context_ref(mainContext);
        } else {
            mainContext = g_main_context_new();
        }
    }
    qDebug() << "QuailEventDispatcherWinGlib::QuailEventDispatcherWinGlib.2";
#if GLIB_CHECK_VERSION (2, 22, 0)
    g_main_context_push_thread_default (mainContext);
#endif

    // setup post event source
    postEventSource = reinterpret_cast<GPostEventSource *>(g_source_new(&postEventSourceFuncs,
                                                                        sizeof(GPostEventSource)));

    qDebug() << "QuailEventDispatcherWinGlib::QuailEventDispatcherWinGlib.3";
    postEventSource->serialNumber.store(1);
    postEventSource->d = this;
    g_source_set_can_recurse(&postEventSource->source, true);
    g_source_attach(&postEventSource->source, mainContext);

    // setup socketNotifierSource
    socketNotifierSource =
        reinterpret_cast<GSocketNotifierSource *>(g_source_new(&socketNotifierSourceFuncs,
                                                               sizeof(GSocketNotifierSource)));
    (void) new (&socketNotifierSource->pollfds) QList<GPollFDWithQSocketNotifier *>();
    g_source_set_can_recurse(&socketNotifierSource->source, true);
    g_source_attach(&socketNotifierSource->source, mainContext);

    // setup normal and idle timer sources
    timerSource = reinterpret_cast<GTimerSource *>(g_source_new(&timerSourceFuncs,
                                                                sizeof(GTimerSource)));
    (void) new (&timerSource->timerList) QTimerInfoList();
    timerSource->processEventsFlags = QEventLoop::AllEvents;
    timerSource->runWithIdlePriority = false;
    g_source_set_can_recurse(&timerSource->source, true);
    g_source_attach(&timerSource->source, mainContext);

    idleTimerSource = reinterpret_cast<GIdleTimerSource *>(g_source_new(&idleTimerSourceFuncs,
                                                                        sizeof(GIdleTimerSource)));
    idleTimerSource->timerSource = timerSource;
    g_source_set_can_recurse(&idleTimerSource->source, true);
    g_source_set_priority(&idleTimerSource->source, G_PRIORITY_DEFAULT_IDLE);
    g_source_attach(&idleTimerSource->source, mainContext);
    qDebug() << "QuailEventDispatcherWinGlib.end";
}

void QuailEventDispatcherWinGlib::runTimersOnceWithNormalPriority()
{
    qDebug() << "QuailEventDispatcherWinGlib::runTimersOnceWithNormalPriority";
    timerSource->runWithIdlePriority = false;
}

QuailEventDispatcherWinGlib::~QuailEventDispatcherWinGlib()
{
    qDebug() << "QuailEventDispatcherWinGlib::~QuailEventDispatcherWinGlib";
    if (internalHwnd)
        DestroyWindow(internalHwnd);
    QString className = QLatin1String("QuailEventDispatcherWinGlib_Internal_Widget") + QString::number(quintptr(qt_internal_proc));
    UnregisterClass((wchar_t*)className.utf16(), qWinAppInst());
}

void QuailEventDispatcherWinGlib::activateEventNotifier(QWinEventNotifier * wen)
{
    qDebug() << "QuailEventDispatcherWinGlib::activateEventNotifier";
    QEvent event(QEvent::WinEventAct);
    QCoreApplication::sendEvent((QObject*)wen, &event);
}

// This function is called by a workerthread
void WINAPI QT_WIN_CALLBACK qt_fast_timer_proc(uint timerId, uint /*reserved*/, DWORD_PTR user, DWORD_PTR /*reserved*/, DWORD_PTR /*reserved*/)
{
    qDebug() << "QuailEventDispatcherWinGlib::qt_fast_timer_proc";
    if (!timerId) // sanity check
        return;
    WinTimerInfo *t = (WinTimerInfo*)user;
    Q_ASSERT(t);
    QCoreApplication::postEvent(t->dispatcher, new QTimerEvent(t->timerId));
}

LRESULT QT_WIN_CALLBACK qt_internal_proc(HWND hwnd, UINT message, WPARAM wp, LPARAM lp)
{
    qDebug() << "QuailEventDispatcherWinGlib::qt_internal_proc";
    if (message == WM_NCCREATE)
        return true;
    qDebug() << "QuailEventDispatcherWinGlib::qt_internal_proc.1";
    MSG msg;
    msg.hwnd = hwnd;
    msg.message = message;
    msg.wParam = wp;
    msg.lParam = lp;
    QAbstractEventDispatcher* dispatcher = QAbstractEventDispatcher::instance();
    long result;
    if (!dispatcher) {
        if (message == WM_TIMER)
            KillTimer(hwnd, wp);
        return 0;
    } else if (dispatcher->filterNativeEvent(QByteArrayLiteral("windows_dispatcher_MSG"), &msg, &result)) {
        return result;
    }

#ifdef GWLP_USERDATA
    QuailEventDispatcherWinGlib *q = (QuailEventDispatcherWinGlib *) GetWindowLongPtr(hwnd, GWLP_USERDATA);
#else
    QuailEventDispatcherWinGlib *q = (QuailEventDispatcherWinGlib *) GetWindowLong(hwnd, GWL_USERDATA);
#endif
    QuailEventDispatcherWinGlib *d = 0;
    if (q != 0)
        d = q;
        //d = q->d_func();

    if (message == WM_QT_SOCKETNOTIFIER) {
        // socket notifier message
        int type = -1;
        switch (WSAGETSELECTEVENT(lp)) {
        case FD_READ:
        case FD_ACCEPT:
            type = 0;
            break;
        case FD_WRITE:
        case FD_CONNECT:
            type = 1;
            break;
        case FD_OOB:
            type = 2;
            break;
        case FD_CLOSE:
            type = 3;
            break;
        }
        if (type >= 0) {
            Q_ASSERT(d != 0);
            QSNDict *sn_vec[4] = { &d->sn_read, &d->sn_write, &d->sn_except, &d->sn_read };
            QSNDict *dict = sn_vec[type];

            QSockNot *sn = dict ? dict->value(wp) : 0;
            if (sn) {
                if (type < 3) {
                    QEvent event(QEvent::SockAct);
                    QCoreApplication::sendEvent(sn->obj, &event);
                } else {
                    QEvent event(QEvent::SockClose);
                    QCoreApplication::sendEvent(sn->obj, &event);
                }
            }
        }
        return 0;
    } else if (message == WM_QT_SENDPOSTEDEVENTS
               // we also use a Windows timer to send posted events when the message queue is full
               || (message == WM_TIMER
                   && d->sendPostedEventsWindowsTimerId != 0
                   && wp == (uint)d->sendPostedEventsWindowsTimerId)) {
        const int localSerialNumber = d->serialNumber.load();
        if (localSerialNumber != d->lastSerialNumber) {
            d->lastSerialNumber = localSerialNumber;
            q->sendPostedEvents();
        }
        return 0;
    } else if (message == WM_TIMER) {
        Q_ASSERT(d != 0);
        d->sendTimerEvent(wp);
        return 0;
    }

    return DefWindowProc(hwnd, message, wp, lp);
}

LRESULT QT_WIN_CALLBACK qt_GetMessageHook(int code, WPARAM wp, LPARAM lp)
{
    qDebug() << "QuailEventDispatcherWinGlib::qt_GetMessageHook";
    if (wp == PM_REMOVE) {
        QuailEventDispatcherWinGlib *q = qobject_cast<QuailEventDispatcherWinGlib *>(QAbstractEventDispatcher::instance());
        Q_ASSERT(q != 0);
        if (q) {
            MSG *msg = (MSG *) lp;
            //QuailEventDispatcherWinGlib *d = q->d_func();
            QuailEventDispatcherWinGlib *d = q;
            const int localSerialNumber = d->serialNumber.load();
            if (HIWORD(GetQueueStatus(QS_TIMER | QS_INPUT | QS_RAWINPUT)) == 0) {
                // no more input or timer events in the message queue, we can allow posted events to be sent normally now
                if (d->sendPostedEventsWindowsTimerId != 0) {
                    // stop the timer to send posted events, since we now allow the WM_QT_SENDPOSTEDEVENTS message
                    KillTimer(d->internalHwnd, d->sendPostedEventsWindowsTimerId);
                    d->sendPostedEventsWindowsTimerId = 0;
                }
                (void) d->wakeUps.fetchAndStoreRelease(0);
                if (localSerialNumber != d->lastSerialNumber
                    // if this message IS the one that triggers sendPostedEvents(), no need to post it again
                    && (msg->hwnd != d->internalHwnd
                        || msg->message != WM_QT_SENDPOSTEDEVENTS)) {
                    PostMessage(d->internalHwnd, WM_QT_SENDPOSTEDEVENTS, 0, 0);
                }
            } else if (d->sendPostedEventsWindowsTimerId == 0
                       && localSerialNumber != d->lastSerialNumber) {
                // start a special timer to continue delivering posted events while
                // there are still input and timer messages in the message queue
                d->sendPostedEventsWindowsTimerId = SetTimer(d->internalHwnd,
                                                             SendPostedEventsWindowsTimerId,
                                                             0, // we specify zero, but Windows uses USER_TIMER_MINIMUM
                                                             NULL);
                // we don't check the return value of SetTimer()... if creating the timer failed, there's little
                // we can do. we just have to accept that posted events will be starved
            }
        }
    }
    return CallNextHookEx(0, code, wp, lp);
}

static HWND qt_create_internal_window(const QuailEventDispatcherWinGlib *eventDispatcher)
{
    qDebug() << "qt_create_internal_window";
    // make sure that multiple Qt's can coexist in the same process
    QString className = QLatin1String("QuailEventDispatcherWinGlib_Internal_Widget") + QString::number(quintptr(qt_internal_proc));

    WNDCLASS wc;
    wc.style = 0;
    wc.lpfnWndProc = qt_internal_proc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = qWinAppInst();
    wc.hIcon = 0;
    wc.hCursor = 0;
    wc.hbrBackground = 0;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = reinterpret_cast<const wchar_t *> (className.utf16());

    RegisterClass(&wc);
    HWND parent = HWND_MESSAGE;
    HWND wnd = CreateWindow(wc.lpszClassName,  // classname
                            wc.lpszClassName,  // window name
                            0,                 // style
                            0, 0, 0, 0,        // geometry
                            parent,            // parent
                            0,                 // menu handle
                            qWinAppInst(),     // application
                            0);                // windows creation data.

    if (!wnd) {
        qWarning("QEventDispatcher: Failed to create QuailEventDispatcherWinGlib internal window: %d\n", (int)GetLastError());
    }

#ifdef GWLP_USERDATA
    SetWindowLongPtr(wnd, GWLP_USERDATA, (LONG_PTR)eventDispatcher);
#else
    SetWindowLong(wnd, GWL_USERDATA, (LONG)eventDispatcher);
#endif
    qDebug() << "qt_create_internal_window.end";
    return wnd;
}

void QuailEventDispatcherWinGlib::registerTimer(WinTimerInfo *t)
{
    qDebug() << "QuailEventDispatcherWinGlib::registerTimer";
    Q_ASSERT(internalHwnd);

    //Q_Q(QuailEventDispatcherWinGlib);
    QuailEventDispatcherWinGlib *q = this;

    int ok = 0;
    uint interval = t->interval;
    if (interval == 0u) {
        // optimization for single-shot-zero-timer
        QCoreApplication::postEvent(q, new QZeroTimerEvent(t->timerId));
        ok = 1;
    } else if ((interval < 20u || t->timerType == Qt::PreciseTimer) && qtimeSetEvent) {
        ok = t->fastTimerId = qtimeSetEvent(interval, 1, qt_fast_timer_proc, (DWORD_PTR)t,
                                            TIME_CALLBACK_FUNCTION | TIME_PERIODIC | TIME_KILL_SYNCHRONOUS);
    } else if (interval >= 20000u || t->timerType == Qt::VeryCoarseTimer) {
        // round the interval, VeryCoarseTimers only have full second accuracy
        interval = ((interval + 500)) / 1000 * 1000;
    }
    if (ok == 0) {
        // user normal timers for (Very)CoarseTimers, or if no more multimedia timers available
        ok = SetTimer(internalHwnd, t->timerId, interval, 0);
    }

    t->timeout = QDateTime::currentMSecsSinceEpoch() + interval;

    if (ok == 0)
        qErrnoWarning("QuailEventDispatcherWinGlib::registerTimer: Failed to create a timer");
}

void QuailEventDispatcherWinGlib::unregisterTimer(WinTimerInfo *t)
{
    qDebug() << "QuailEventDispatcherWinGlib::unregisterTimer";
    if (t->interval == 0) {
        //QCoreApplicationPrivate::removePostedTimerEvent(t->dispatcher, t->timerId);
        QCoreApplication::removePostedEvents(t->dispatcher, t->timerId);
    } else if (t->fastTimerId != 0) {
        qtimeKillEvent(t->fastTimerId);
        //QCoreApplicationPrivate::removePostedTimerEvent(t->dispatcher, t->timerId);
        QCoreApplication::removePostedEvents(t->dispatcher, t->timerId);
    } else if (internalHwnd) {
        KillTimer(internalHwnd, t->timerId);
    }
    delete t;
}

void QuailEventDispatcherWinGlib::sendTimerEvent(int timerId)
{
    qDebug() << "QuailEventDispatcherWinGlib::sendTimerEvent";
    WinTimerInfo *t = timerDict.value(timerId);
    if (t && !t->inTimerEvent) {
        // send event, but don't allow it to recurse
        t->inTimerEvent = true;

        QTimerEvent e(t->timerId);
        QCoreApplication::sendEvent(t->obj, &e);

        // timer could have been removed
        t = timerDict.value(timerId);
        if (t) {
            t->inTimerEvent = false;
        }
    }
}

void QuailEventDispatcherWinGlib::doWsaAsyncSelect(int socket)
{
    qDebug() << "QuailEventDispatcherWinGlib::doWsaAsyncSelect";
    Q_ASSERT(internalHwnd);
    int sn_event = 0;
    if (sn_read.contains(socket))
        sn_event |= FD_READ | FD_CLOSE | FD_ACCEPT;
    if (sn_write.contains(socket))
        sn_event |= FD_WRITE | FD_CONNECT;
    if (sn_except.contains(socket))
        sn_event |= FD_OOB;
    // BoundsChecker may emit a warning for WSAAsyncSelect when sn_event == 0
    // This is a BoundsChecker bug and not a Qt bug
    WSAAsyncSelect(socket, internalHwnd, sn_event ? int(WM_QT_SOCKETNOTIFIER) : 0, sn_event);
}

void QuailEventDispatcherWinGlib::createInternalHwnd()
{
    qDebug() << "QuailEventDispatcherWinGlib::createInternalHwnd";
    //QuailEventDispatcherWinGlib *d = this;
    QuailEventDispatcherWinGlib *d = this;

    Q_ASSERT(!d->internalHwnd);
    if (d->internalHwnd)
        return;
    d->internalHwnd = qt_create_internal_window(this);

    // setup GetMessage hook needed to drive our posted events
    d->getMessageHook = SetWindowsHookEx(WH_GETMESSAGE, (HOOKPROC) qt_GetMessageHook, NULL, GetCurrentThreadId());
    if (!d->getMessageHook) {
        qFatal("Qt: INTERNALL ERROR: failed to install GetMessage hook");
    }

    // register all socket notifiers
    QList<int> sockets = (d->sn_read.keys().toSet()
                          + d->sn_write.keys().toSet()
                          + d->sn_except.keys().toSet()).toList();
    for (int i = 0; i < sockets.count(); ++i)
        d->doWsaAsyncSelect(sockets.at(i));

    // start all normal timers
    for (int i = 0; i < d->timerVec.count(); ++i)
        d->registerTimer(d->timerVec.at(i));

    // trigger a call to sendPostedEvents()
    wakeUp();
}

bool QuailEventDispatcherWinGlib::processEvents(QEventLoop::ProcessEventsFlags flags)
{
    qDebug() << "QuailEventDispatcherWinGlib::processEvents";
    //QuailEventDispatcherWinGlib *d = this;
    QuailEventDispatcherWinGlib *d = this;

    if (!d->internalHwnd)
        createInternalHwnd();

    d->interruptFlag = false;
    emit awake();

    bool canWait;
    bool retVal = false;
    bool seenWM_QT_SENDPOSTEDEVENTS = false;
    bool needWM_QT_SENDPOSTEDEVENTS = false;
    do {
        DWORD waitRet = 0;
        HANDLE pHandles[MAXIMUM_WAIT_OBJECTS - 1];
        QVarLengthArray<MSG> processedTimers;
        while (!d->interruptFlag) {
            DWORD nCount = d->winEventNotifierList.count();
            Q_ASSERT(nCount < MAXIMUM_WAIT_OBJECTS - 1);

            MSG msg;
            bool haveMessage;

            if (!(flags & QEventLoop::ExcludeUserInputEvents) && !d->queuedUserInputEvents.isEmpty()) {
                // process queued user input events
                haveMessage = true;
                msg = d->queuedUserInputEvents.takeFirst();
            } else if(!(flags & QEventLoop::ExcludeSocketNotifiers) && !d->queuedSocketEvents.isEmpty()) {
                // process queued socket events
                haveMessage = true;
                msg = d->queuedSocketEvents.takeFirst();
            } else {
                haveMessage = PeekMessage(&msg, 0, 0, 0, PM_REMOVE);
                if (haveMessage && (flags & QEventLoop::ExcludeUserInputEvents)
                    && ((msg.message >= WM_KEYFIRST
                         && msg.message <= WM_KEYLAST)
                        || (msg.message >= WM_MOUSEFIRST
                            && msg.message <= WM_MOUSELAST)
                        || msg.message == WM_MOUSEWHEEL
                        || msg.message == WM_MOUSEHWHEEL
                        || msg.message == WM_TOUCH
#ifndef QT_NO_GESTURES
                        || msg.message == WM_GESTURE
                        || msg.message == WM_GESTURENOTIFY
#endif
                        || msg.message == WM_CLOSE)) {
                    // queue user input events for later processing
                    haveMessage = false;
                    d->queuedUserInputEvents.append(msg);
                }
                if (haveMessage && (flags & QEventLoop::ExcludeSocketNotifiers)
                    && (msg.message == WM_QT_SOCKETNOTIFIER && msg.hwnd == d->internalHwnd)) {
                    // queue socket events for later processing
                    haveMessage = false;
                    d->queuedSocketEvents.append(msg);
                }
            }
            if (!haveMessage) {
                // no message - check for signalled objects
                for (int i=0; i<(int)nCount; i++)
                    pHandles[i] = d->winEventNotifierList.at(i)->handle();
                waitRet = MsgWaitForMultipleObjectsEx(nCount, pHandles, 0, QS_ALLINPUT, MWMO_ALERTABLE);
                if ((haveMessage = (waitRet == WAIT_OBJECT_0 + nCount))) {
                    // a new message has arrived, process it
                    continue;
                }
            }
            if (haveMessage) {
                if (d->internalHwnd == msg.hwnd && msg.message == WM_QT_SENDPOSTEDEVENTS) {
                    if (seenWM_QT_SENDPOSTEDEVENTS) {
                        // when calling processEvents() "manually", we only want to send posted
                        // events once
                        needWM_QT_SENDPOSTEDEVENTS = true;
                        continue;
                    }
                    seenWM_QT_SENDPOSTEDEVENTS = true;
                } else if (msg.message == WM_TIMER) {
                    // avoid live-lock by keeping track of the timers we've already sent
                    bool found = false;
                    for (int i = 0; !found && i < processedTimers.count(); ++i) {
                        const MSG processed = processedTimers.constData()[i];
                        found = (processed.wParam == msg.wParam && processed.hwnd == msg.hwnd && processed.lParam == msg.lParam);
                    }
                    if (found)
                        continue;
                    processedTimers.append(msg);
                } else if (msg.message == WM_QUIT) {
                    if (QCoreApplication::instance())
                        QCoreApplication::instance()->quit();
                    return false;
                }

                if (!filterNativeEvent(QByteArrayLiteral("windows_generic_MSG"), &msg, 0)) {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            } else if (waitRet - WAIT_OBJECT_0 < nCount) {
                d->activateEventNotifier(d->winEventNotifierList.at(waitRet - WAIT_OBJECT_0));
            } else {
                // nothing todo so break
                break;
            }
            retVal = true;
        }

        // still nothing - wait for message or signalled objects
        canWait = (!retVal
                   && !d->interruptFlag
                   && (flags & QEventLoop::WaitForMoreEvents));
        if (canWait) {
            DWORD nCount = d->winEventNotifierList.count();
            Q_ASSERT(nCount < MAXIMUM_WAIT_OBJECTS - 1);
            for (int i=0; i<(int)nCount; i++)
                pHandles[i] = d->winEventNotifierList.at(i)->handle();

            emit aboutToBlock();
            waitRet = MsgWaitForMultipleObjectsEx(nCount, pHandles, INFINITE, QS_ALLINPUT, MWMO_ALERTABLE | MWMO_INPUTAVAILABLE);
            emit awake();
            if (waitRet - WAIT_OBJECT_0 < nCount) {
                d->activateEventNotifier(d->winEventNotifierList.at(waitRet - WAIT_OBJECT_0));
                retVal = true;
            }
        }
    } while (canWait);

    if (!seenWM_QT_SENDPOSTEDEVENTS && (flags & QEventLoop::EventLoopExec) == 0) {
        // when called "manually", always send posted events
        sendPostedEvents();
    }

    if (needWM_QT_SENDPOSTEDEVENTS)
        PostMessage(d->internalHwnd, WM_QT_SENDPOSTEDEVENTS, 0, 0);

    return retVal;
}

bool QuailEventDispatcherWinGlib::hasPendingEvents()
{
    qDebug() << "QuailEventDispatcherWinGlib::hasPendingEvents";
    MSG msg;
    return qGlobalPostedEventsCount() || PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
}

void QuailEventDispatcherWinGlib::registerSocketNotifier(QSocketNotifier *notifier)
{
    qDebug() << "QuailEventDispatcherWinGlib::registerSocketNotifier";
    Q_ASSERT(notifier);
    int sockfd = notifier->socket();
    int type = notifier->type();
#ifndef QT_NO_DEBUG
    if (sockfd < 0) {
        qWarning("QSocketNotifier: Internal error");
        return;
    } else if (notifier->thread() != thread() || thread() != QThread::currentThread()) {
        qWarning("QSocketNotifier: socket notifiers cannot be enabled from another thread");
        return;
    }
#endif

    //QuailEventDispatcherWinGlib *d = this;
    QuailEventDispatcherWinGlib *d = this;
    QSNDict *sn_vec[3] = { &d->sn_read, &d->sn_write, &d->sn_except };
    QSNDict *dict = sn_vec[type];

    if (QCoreApplication::closingDown()) // ### d->exitloop?
        return; // after sn_cleanup, don't reinitialize.

    if (dict->contains(sockfd)) {
        const char *t[] = { "Read", "Write", "Exception" };
    /* Variable "socket" below is a function pointer. */
        qWarning("QSocketNotifier: Multiple socket notifiers for "
                 "same socket %d and type %s", sockfd, t[type]);
    }

    QSockNot *sn = new QSockNot;
    sn->obj = notifier;
    sn->fd  = sockfd;
    dict->insert(sn->fd, sn);

    if (d->internalHwnd)
        d->doWsaAsyncSelect(sockfd);
}

void QuailEventDispatcherWinGlib::unregisterSocketNotifier(QSocketNotifier *notifier)
{
    qDebug() << "QuailEventDispatcherWinGlib::unregisterSocketNotifier";
    Q_ASSERT(notifier);
    int sockfd = notifier->socket();
    int type = notifier->type();
#ifndef QT_NO_DEBUG
    if (sockfd < 0) {
        qWarning("QSocketNotifier: Internal error");
        return;
    } else if (notifier->thread() != thread() || thread() != QThread::currentThread()) {
        qWarning("QSocketNotifier: socket notifiers cannot be disabled from another thread");
        return;
    }
#endif

    QuailEventDispatcherWinGlib *d = this;
    QSNDict *sn_vec[3] = { &d->sn_read, &d->sn_write, &d->sn_except };
    QSNDict *dict = sn_vec[type];
    QSockNot *sn = dict->value(sockfd);
    if (!sn)
        return;

    dict->remove(sockfd);
    delete sn;

    if (d->internalHwnd)
        d->doWsaAsyncSelect(sockfd);
}

void QuailEventDispatcherWinGlib::registerTimer(int timerId, int interval, Qt::TimerType timerType, QObject *object)
{
    qDebug() << "QuailEventDispatcherWinGlib::registerTimer.GLIB'ed";
    if (timerId < 1 || interval < 0 || !object) {
        qWarning("QuailEventDispatcherWinGlib::registerTimer: invalid arguments");
        return;
    } else if (object->thread() != thread() || thread() != QThread::currentThread()) {
        qWarning("QObject::startTimer: timers cannot be started from another thread");
        return;
    }

    QuailEventDispatcherWinGlib *d = this;

    WinTimerInfo *t = new WinTimerInfo;
    t->dispatcher = this;
    t->timerId  = timerId;
    t->interval = interval;
    t->timerType = timerType;
    t->obj  = object;
    t->inTimerEvent = false;
    t->fastTimerId = 0;

    if (d->internalHwnd)
        d->registerTimer(t);

    d->timerVec.append(t);                      // store in timer vector
    d->timerDict.insert(t->timerId, t);          // store timers in dict
}

bool QuailEventDispatcherWinGlib::unregisterTimer(int timerId)
{
    qDebug() << "QuailEventDispatcherWinGlib::unregisterTimer";
    if (timerId < 1) {
        qWarning("QuailEventDispatcherWinGlib::unregisterTimer: invalid argument");
        return false;
    }
    QThread *currentThread = QThread::currentThread();
    if (thread() != currentThread) {
        qWarning("QObject::killTimer: timers cannot be stopped from another thread");
        return false;
    }

    QuailEventDispatcherWinGlib *d = this;
    if (d->timerVec.isEmpty() || timerId <= 0)
        return false;

    WinTimerInfo *t = d->timerDict.value(timerId);
    if (!t)
        return false;

    d->timerDict.remove(t->timerId);
    d->timerVec.removeAll(t);
    d->unregisterTimer(t);
    return true;
}

bool QuailEventDispatcherWinGlib::unregisterTimers(QObject *object)
{
    qDebug() << "QuailEventDispatcherWinGlib::unregisterTimer";
    if (!object) {
        qWarning("QuailEventDispatcherWinGlib::unregisterTimers: invalid argument");
        return false;
    }
    QThread *currentThread = QThread::currentThread();
    if (object->thread() != thread() || thread() != currentThread) {
        qWarning("QObject::killTimers: timers cannot be stopped from another thread");
        return false;
    }

    QuailEventDispatcherWinGlib *d = this;
    if (d->timerVec.isEmpty())
        return false;
    register WinTimerInfo *t;
    for (int i=0; i<d->timerVec.size(); i++) {
        t = d->timerVec.at(i);
        if (t && t->obj == object) {                // object found
            d->timerDict.remove(t->timerId);
            d->timerVec.removeAt(i);
            d->unregisterTimer(t);
            --i;
        }
    }
    return true;
}

QList<QuailEventDispatcherWinGlib::TimerInfo>
QuailEventDispatcherWinGlib::registeredTimers(QObject *object) const
{
    qDebug() << "QuailEventDispatcherWinGlib::registeredTimers";
    if (!object) {
        qWarning("QuailEventDispatcherWinGlib:registeredTimers: invalid argument");
        return QList<TimerInfo>();
    }

    const QuailEventDispatcherWinGlib *d = this;
    QList<TimerInfo> list;
    for (int i = 0; i < d->timerVec.size(); ++i) {
        const WinTimerInfo *t = d->timerVec.at(i);
        if (t && t->obj == object)
            list << TimerInfo(t->timerId, t->interval, t->timerType);
    }
    return list;
}

bool QuailEventDispatcherWinGlib::registerEventNotifier(QWinEventNotifier *notifier)
{
    qDebug() << "QuailEventDispatcherWinGlib::registerEventNotifier";
    if (!notifier) {
        qWarning("QWinEventNotifier: Internal error");
        return false;
    } else if (notifier->thread() != thread() || thread() != QThread::currentThread()) {
        qWarning("QWinEventNotifier: event notifiers cannot be enabled from another thread");
        return false;
    }

    QuailEventDispatcherWinGlib *d = this;

    if (d->winEventNotifierList.contains(notifier))
        return true;

    if (d->winEventNotifierList.count() >= MAXIMUM_WAIT_OBJECTS - 2) {
        qWarning("QWinEventNotifier: Cannot have more than %d enabled at one time", MAXIMUM_WAIT_OBJECTS - 2);
        return false;
    }
    d->winEventNotifierList.append(notifier);
    return true;
}

void QuailEventDispatcherWinGlib::unregisterEventNotifier(QWinEventNotifier *notifier)
{
    qDebug() << "QuailEventDispatcherWinGlib::unregisterEventNotifier";
    if (!notifier) {
        qWarning("QWinEventNotifier: Internal error");
        return;
    } else if (notifier->thread() != thread() || thread() != QThread::currentThread()) {
        qWarning("QWinEventNotifier: event notifiers cannot be disabled from another thread");
        return;
    }

    QuailEventDispatcherWinGlib *d = this;

    int i = d->winEventNotifierList.indexOf(notifier);
    if (i != -1)
        d->winEventNotifierList.takeAt(i);
}

void QuailEventDispatcherWinGlib::activateEventNotifiers()
{
    QuailEventDispatcherWinGlib *d = this;
    //### this could break if events are removed/added in the activation
    for (int i=0; i<d->winEventNotifierList.count(); i++) {
        if (WaitForSingleObjectEx(d->winEventNotifierList.at(i)->handle(), 0, TRUE) == WAIT_OBJECT_0)
            d->activateEventNotifier(d->winEventNotifierList.at(i));
    }
}

int QuailEventDispatcherWinGlib::remainingTime(int timerId)
{
#ifndef QT_NO_DEBUG
    if (timerId < 1) {
        qWarning("QuailEventDispatcherWinGlib::remainingTime: invalid argument");
        return -1;
    }
#endif

    QuailEventDispatcherWinGlib *d = this;

    if (d->timerVec.isEmpty())
        return -1;

    quint64 currentTime = QDateTime::currentMSecsSinceEpoch ();

    register WinTimerInfo *t;
    for (int i=0; i<d->timerVec.size(); i++) {
        t = d->timerVec.at(i);
        if (t && t->timerId == timerId) {                // timer found
            if (currentTime < t->timeout) {
                // time to wait
                return t->timeout - currentTime;
            } else {
                return 0;
            }
        }
    }

#ifndef QT_NO_DEBUG
    qWarning("QuailEventDispatcherWinGlib::remainingTime: timer id %d not found", timerId);
#endif

    return -1;
}

void QuailEventDispatcherWinGlib::wakeUp()
{
    QuailEventDispatcherWinGlib *d = this;
    d->serialNumber.ref();
    if (d->internalHwnd && d->wakeUps.testAndSetAcquire(0, 1)) {
        // post a WM_QT_SENDPOSTEDEVENTS to this thread if there isn't one already pending
        PostMessage(d->internalHwnd, WM_QT_SENDPOSTEDEVENTS, 0, 0);
    }
}

void QuailEventDispatcherWinGlib::interrupt()
{
    QuailEventDispatcherWinGlib *d = this;
    d->interruptFlag = true;
    wakeUp();
}

void QuailEventDispatcherWinGlib::flush()
{ }

void QuailEventDispatcherWinGlib::startingUp()
{ }

void QuailEventDispatcherWinGlib::closingDown()
{
    QuailEventDispatcherWinGlib *d = this;

    // clean up any socketnotifiers
    while (!d->sn_read.isEmpty())
        unregisterSocketNotifier((*(d->sn_read.begin()))->obj);
    while (!d->sn_write.isEmpty())
        unregisterSocketNotifier((*(d->sn_write.begin()))->obj);
    while (!d->sn_except.isEmpty())
        unregisterSocketNotifier((*(d->sn_except.begin()))->obj);

    // clean up any timers
    for (int i = 0; i < d->timerVec.count(); ++i)
        d->unregisterTimer(d->timerVec.at(i));
    d->timerVec.clear();
    d->timerDict.clear();

    if (d->getMessageHook)
        UnhookWindowsHookEx(d->getMessageHook);
    d->getMessageHook = 0;
}

bool QuailEventDispatcherWinGlib::event(QEvent *e)
{
    qDebug() << "QuailEventDispatcherWinGlib::event";
    QuailEventDispatcherWinGlib *d = this;
    if (e->type() == QEvent::ZeroTimerEvent) {
        QZeroTimerEvent *zte = static_cast<QZeroTimerEvent*>(e);
        WinTimerInfo *t = d->timerDict.value(zte->timerId());
        if (t) {
            t->inTimerEvent = true;

            QTimerEvent te(zte->timerId());
            QCoreApplication::sendEvent(t->obj, &te);

            t = d->timerDict.value(zte->timerId());
            if (t) {
                if (t->interval == 0 && t->inTimerEvent) {
                    // post the next zero timer event as long as the timer was not restarted
                    QCoreApplication::postEvent(this, new QZeroTimerEvent(zte->timerId()));
                }

                t->inTimerEvent = false;
            }
        }
        return true;
    } else if (e->type() == QEvent::Timer) {
        QTimerEvent *te = static_cast<QTimerEvent*>(e);
        d->sendTimerEvent(te->timerId());
    }
    return QAbstractEventDispatcher::event(e);
}

void QuailEventDispatcherWinGlib::sendPostedEvents()
{
    //QuailEventDispatcherWinGlib *d = this;
    //QCoreApplicationPrivate::sendPostedEvents(0, 0, d->threadData);
    QCoreApplication::sendPostedEvents();
}

QThreadData::QThreadData(int initialRefCount)
    : _ref(initialRefCount), thread(0), threadId(0),
      quitNow(false), loopLevel(0), eventDispatcher(0), canWait(true), isAdopted(false)
{
    // fprintf(stderr, "QThreadData %p created\n", this);
}

QThreadData::~QThreadData()
{
    Q_ASSERT(_ref.load() == 0);

    // In the odd case that Qt is running on a secondary thread, the main
    // thread instance will have been dereffed asunder because of the deref in
    // QThreadData::current() and the deref in the pthread_destroy. To avoid
    // crashing during QCoreApplicationData's global static cleanup we need to
    // safeguard the main thread here.. This fix is a bit crude, but it solves
    // the problem...
//    if (this->thread == QCoreApplication::thread()) {
//       //QCoreApplication::thread() = 0;
//       QThreadData::clearCurrentThreadData();
//    }

    QThread *t = thread;
    thread = 0;
    delete t;

    for (int i = 0; i < postEventList.size(); ++i) {
        const QPostEvent &pe = postEventList.at(i);
        if (pe.event) {
            //--pe.receiver->d_func()->postedEvents;
            pe.event->posted = false;
            delete pe.event;
        }
    }

    // fprintf(stderr, "QThreadData %p destroyed\n", this);
}

void QThreadData::ref()
{
#ifndef QT_NO_THREAD
    (void) _ref.ref();
    Q_ASSERT(_ref.load() != 0);
#endif
}

void QThreadData::deref()
{
#ifndef QT_NO_THREAD
    if (!_ref.deref())
        delete this;
#endif
}

