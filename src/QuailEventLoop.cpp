/**
 * @file QQuailMenuLoop.cpp Gaim Qt Event Loop implementation
 *
 * @Copyright (C) 2004 Christian Hammond.
 * @Copyright (C) 2013      Phil Hannent.
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
#include "QuailEventLoop.h"
#include <QDebug>

static gboolean qQuailSourceRemove(guint handle);
static gboolean qQuailTimeoutRemove(guint handle);

static QuailEventLoop *eventLoop = NULL;

QQuailTimer::QQuailTimer(GSourceFunc func, gpointer data)
    : QTimer(eventLoop), func(func), userData(data)
{
	connect(this, SIGNAL(timeout()),
			this, SLOT(update()));
}

void
QQuailTimer::update()
{
	if (!func(userData))
        qQuailTimeoutRemove(sourceId);
}

void
QQuailTimer::setHandle(guint newSourceId)
{
    sourceId(newSourceId);
}

QQuailInputNotifier::QQuailInputNotifier(int fd,
                                         PurpleInputCondition cond,
                                         PurpleInputFunction func,
                                         gpointer userData)
    : QObject(eventLoop), func(func), userData(userData), readNotifier(NULL),
	  writeNotifier(NULL)
{
    //qDebug() << "QQuailInputNotifier::QQuailInputNotifier";
    if (cond & PURPLE_INPUT_READ)
	{
        //qDebug() << "QQuailInputNotifier::QQuailInputNotifier::READ";
		readNotifier = new QSocketNotifier(fd, QSocketNotifier::Read);

		connect(readNotifier, SIGNAL(activated(int)),
				this, SLOT(ioInvoke(int)));
	}

    if (cond & PURPLE_INPUT_WRITE)
	{
        //qDebug() << "QQuailInputNotifier::QQuailInputNotifier::WRITE";
		writeNotifier = new QSocketNotifier(fd, QSocketNotifier::Write);

		connect(writeNotifier, SIGNAL(activated(int)),
				this, SLOT(ioInvoke(int)));
	}
}

QQuailInputNotifier::~QQuailInputNotifier()
{
	if (readNotifier != NULL)
		delete readNotifier;

	if (writeNotifier != NULL)
		delete writeNotifier;
}

void
QQuailInputNotifier::ioInvoke(int fd)
{
    //qDebug() << "QQuailInputNotifier::ioInvoke";
	int cond = 0;

	if (readNotifier != NULL)
        cond |= PURPLE_INPUT_READ;

	if (writeNotifier != NULL)
        cond |= PURPLE_INPUT_WRITE;

	func(userData, fd, (PurpleInputCondition)cond);
}

QuailEventLoop::QuailEventLoop(QObject *parent) :
    QObject(parent)
{
    eventLoop = this;
}

bool QuailEventLoop::processEvents(QEventLoop::ProcessEventsFlags flags)
{
    //TODO: Fill this in
}

bool QuailEventLoop::hasPendingEvents()
{
    return (m_timers.count() + m_sources.count()) > 0;
}

void QuailEventLoop::registerSocketNotifier(QSocketNotifier *notifier)
{
    //qQuailInputAdd
}

void QuailEventLoop::unregisterSocketNotifier(QSocketNotifier *notifier)
{
    //qQuailSourceRemove
}

void QuailEventLoop::registerTimer(int timerId, int interval, QObject *object)
{
    //qQuailTimeoutAdd
}

bool QuailEventLoop::unregisterTimer(int timerId)
{
    qQuailTimeoutRemove(timerId);
}

bool QuailEventLoop::unregisterTimers(QObject *object)
{
    qQuailTimeoutRemove(m_timers.indexOf(object));
}

QList<QAbstractEventDispatcher::TimerInfo>
QuailEventLoop::registeredTimers(QObject *object) const
{
    Q_UNUSED(object);
    /* Not implemented, we won't use this. It's only used for
       transfering QObject from one thread to another. */
    return QList<QAbstractEventDispatcher::TimerInfo>();
}

static guint
qQuailTimeoutAdd(guint interval, GSourceFunc func, gpointer data)
{
    //qDebug() << "QQuailInputNotifier::qQuailTimeoutAdd";
    QQuailTimer *timer = new QQuailTimer(func, data);
    eventLoop->m_timers.append(timer);
    guint handle = eventLoop->m_timers.lastIndexOf(timer);
    timer->setHandle(handle);
    timer->start(interval);

    return handle;
}

static gboolean
qQuailTimeoutRemove(guint handle)
{
    //qDebug() << "QQuailInputNotifier::qQuailTimeoutRemove";
    QQuailTimer *timer = eventLoop->m_timers.takeAt(handle);

    if (timer == NULL)
        return false;

    delete timer;
    return true;
}

static guint
qQuailInputAdd(int fd,
               PurpleInputCondition cond,
               PurpleInputFunction func,
               gpointer userData)
{
    //qDebug() << "QQuailInputNotifier::qQuailInputAdd";
    QQuailInputNotifier *notifier = new QQuailInputNotifier(fd, cond, func, userData);
    eventLoop->m_sources.append(notifier);
    return eventLoop->m_sources.lastIndexOf(notifier);
}

static gboolean
qQuailSourceRemove(guint handle)
{
    //qDebug() << "QQuailInputNotifier::qQuailSourceRemove";
    QQuailInputNotifier *notifier = eventLoop->m_sources.takeAt(handle);

    if (notifier == NULL)
        return false;

    delete notifier;
    return true;
}

static int
qQuailInputGetError(int /*fd*/, int */*error*/)
{
    //qDebug() << "QQuailInputNotifier::qQuailInputGetError";
    return 0;
}

static guint
qQuailTimeoutAddSeconds(guint interval,
                        GSourceFunc function,
                        gpointer data)
{
        return qQuailTimeoutAdd(interval * 1000,function, data);
}


static PurpleEventLoopUiOps eventloop_ops =
{
    qQuailTimeoutAdd, /*timeout_add */
    qQuailTimeoutRemove,/*timeout_remove */
    qQuailInputAdd, /*input_add */
    qQuailSourceRemove, /*input_remove */
    qQuailInputGetError, /* input_get_error */
    qQuailTimeoutAddSeconds, /* timeout_add_seconds */
    NULL,
    NULL,
    NULL
};

PurpleEventLoopUiOps *
qQuailGetEventLoopUiOps(void)
{
	return &eventloop_ops;
}
