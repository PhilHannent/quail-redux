#ifndef QUAILTIMERINFOLIST_H
#define QUAILTIMERINFOLIST_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

// #define QTIMERINFO_DEBUG

#include "qabstracteventdispatcher.h"

#include <sys/time.h> // struct timeval

// Internal operator functions for timespecs
inline timespec &normalizedTimespec(timespec &t)
{
    while (t.tv_nsec >= 1000000000) {
        ++t.tv_sec;
        t.tv_nsec -= 1000000000;
    }
    while (t.tv_nsec < 0) {
        --t.tv_sec;
        t.tv_nsec += 1000000000;
    }
    return t;
}
inline bool operator<(const timespec &t1, const timespec &t2)
{ return t1.tv_sec < t2.tv_sec || (t1.tv_sec == t2.tv_sec && t1.tv_nsec < t2.tv_nsec); }
inline bool operator==(const timespec &t1, const timespec &t2)
{ return t1.tv_sec == t2.tv_sec && t1.tv_nsec == t2.tv_nsec; }
inline timespec &operator+=(timespec &t1, const timespec &t2)
{
    t1.tv_sec += t2.tv_sec;
    t1.tv_nsec += t2.tv_nsec;
    return normalizedTimespec(t1);
}
inline timespec operator+(const timespec &t1, const timespec &t2)
{
    timespec tmp;
    tmp.tv_sec = t1.tv_sec + t2.tv_sec;
    tmp.tv_nsec = t1.tv_nsec + t2.tv_nsec;
    return normalizedTimespec(tmp);
}
inline timespec operator-(const timespec &t1, const timespec &t2)
{
    timespec tmp;
    tmp.tv_sec = t1.tv_sec - (t2.tv_sec - 1);
    tmp.tv_nsec = t1.tv_nsec - (t2.tv_nsec + 1000000000);
    return normalizedTimespec(tmp);
}
inline timespec operator*(const timespec &t1, int mul)
{
    timespec tmp;
    tmp.tv_sec = t1.tv_sec * mul;
    tmp.tv_nsec = t1.tv_nsec * mul;
    return normalizedTimespec(tmp);
}


// internal timer info
struct QTimerInfo {
    int id;           // - timer identifier
    int interval;     // - timer interval in milliseconds
    Qt::TimerType timerType; // - timer type
    timespec timeout;  // - when to actually fire
    QObject *obj;     // - object to receive event
    QTimerInfo **activateRef; // - ref from activateTimers

#ifdef QTIMERINFO_DEBUG
    timeval expected; // when timer is expected to fire
    float cumulativeError;
    uint count;
#endif
};

class QTimerInfoList : public QList<QTimerInfo*>
{
#if ((_POSIX_MONOTONIC_CLOCK-0 <= 0) && !defined(Q_OS_MAC)) || defined(QT_BOOTSTRAPPED)
    timespec previousTime;
    clock_t previousTicks;
    int ticksPerSecond;
    int msPerTick;

    bool timeChanged(timespec *delta);
    void timerRepair(const timespec &);
#endif

    // state variables used by activateTimers()
    QTimerInfo *firstTimerInfo;

public:
    QTimerInfoList();

    timespec currentTime;
    timespec updateCurrentTime();

    // must call updateCurrentTime() first!
    void repairTimersIfNeeded();

    bool timerWait(timespec &);
    void timerInsert(QTimerInfo *);

    int timerRemainingTime(int timerId);

    void registerTimer(int timerId, int interval, Qt::TimerType timerType, QObject *object);
    bool unregisterTimer(int timerId);
    bool unregisterTimers(QObject *object);
    QList<QAbstractEventDispatcher::TimerInfo> registeredTimers(QObject *object) const;

    int activateTimers();
};

#endif // QUAILTIMERINFOLIST_H
