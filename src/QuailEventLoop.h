/**
 * @file QQuailMenuLoop.h Gaim Qt Event Loop implementation
 *
 * @Copyright (C) 2004 Christian Hammond.
 * @Copyright (C) 2013 Phil Hannent.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA  02111-1307  USA
 */
#ifndef _QUAIL_EVENT_LOOP_H_
#define _QUAIL_EVENT_LOOP_H_

#include <libpurple/eventloop.h>

#include <QTimer>
#include <QSocketNotifier>
/* http://harmattan-dev.nokia.com/docs/library/html/qt4/qabstracteventdispatcher.html */
//#include <QAbstractEventDispatcher>
#include "QuailMainWindow.h"

static QQuailMainWindow *mainWin = 0;

class QQuailTimer : public QObject
{
    Q_OBJECT

    public:
        QQuailTimer(guint sourceId, GSourceFunc func, gpointer data);

        void start(int interval);
    private slots:
        void update();

    private:
        guint sourceId;
        GSourceFunc func;
        gpointer userData;
        QTimer *t;
};

class QQuailInputNotifier : public QObject
{
    Q_OBJECT

    public:
        QQuailInputNotifier(int fd, PurpleInputCondition cond,
                           PurpleInputFunction func, gpointer userData);
        ~QQuailInputNotifier();

    private slots:
        void ioInvoke(int fd);

    private:
        PurpleInputCondition cond;
        PurpleInputFunction func;
        gpointer userData;
        QSocketNotifier *readNotifier, *writeNotifier;
};



/**
 * Returns the Qt event loop UI operations structure.
 *
 * @return The Qt event loop UI operations structure.
 */
PurpleEventLoopUiOps *qQuailGetEventLoopUiOps(void);

/**
 * Initializes the Qt event loop subsystem.
 */
void qQuailEventLoopInit(void);

/**
 * Uninitializes the Qt evnet loop subsystem.
 */
void qQuailEventLoopUninit(void);

//class QuailEventDispatcherMarkTwo : public QAbstractEventDispatcher
//{
//    Q_OBJECT

//public:
//    QuailEventDispatcherMarkTwo(QObject *parent = 0);

//    virtual bool processEvents(QEventLoop::ProcessEventsFlags flags)
//    {; }
//    virtual bool hasPendingEvents() = 0;

//    virtual void registerSocketNotifier(QSocketNotifier *notifier) = 0;
//    virtual void unregisterSocketNotifier(QSocketNotifier *notifier) = 0;

//    virtual void registerTimer(int timerId, int interval, QObject *object) = 0;
//    virtual bool unregisterTimer(int timerId) = 0;
//    virtual bool unregisterTimers(QObject *object) = 0;
//    virtual QList<TimerInfo> registeredTimers(QObject *object) const  = 0;

//    virtual void wakeUp() = 0;
//    virtual void interrupt() = 0;
//    virtual void flush() = 0;

//};

#endif /* _QUAIL_EVENT_LOOP_H_ */
