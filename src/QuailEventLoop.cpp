﻿/**
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
#include <QApplication>
#include <QDebug>
#include <QMap>
#include <QThread>

static gboolean qQuailSourceRemove(guint handle);

//static QMap<guint, quail_source_info*> m_sources;
//static QuailEventDispatcherMarkTwo *mainEvent = 0;
//QThread *quail_event_thread = 0;
static quail_event_loop* quail_app = 0;

quail_event_loop::quail_event_loop(QObject * parent)
    : QObject(parent), nextSourceId(0)
{
    quail_app = this;
}

guint
quail_event_loop::quail_timeout_add(guint interval, GSourceFunc func, gpointer data)
{
    qDebug() << "quail_application::quail_timeout_add" << interval;
    int id = -1;
    if (QThread::currentThread() == qApp->thread())
    {
        qDebug() << "quail_application::quail_timeout_add.1a";
        id = QObject::startTimer(interval);
    } else {
        qDebug() << "quail_application::quail_timeout_add.1b";
        QMetaObject::invokeMethod(this, "startTimer", Qt::BlockingQueuedConnection,
                                  Q_ARG(int, interval), Q_ARG(int*, &id));
    }

    qDebug() << "quail_application::quail_timeout_add.2";
    m_timers.insert(id,new QQuailTimer(id, func, data));
    qDebug() << "quail_application::quail_timeout_add.end";
    return static_cast<guint>(id);
}

void
quail_event_loop::startTimer(int interval, int *id)
{
    *id = QObject::startTimer(interval);
}

void
quail_event_loop::timerEvent(QTimerEvent* e)
{
    //qDebug() << "quail_event_loop::timerEvent()";
    QQuailTimer *t = m_timers.take(e->timerId());
    if (t == 0)
        return;

    if (!t->func(t->data))
    {
        quail_timeout_remove(e->timerId());
    }
}


gboolean
quail_event_loop::quail_timeout_remove(guint handle)
{
    qDebug() << "quail_application::quail_timeout_remove";
    QQuailTimer *t = m_timers.take(handle);
    if (t == 0)
        return FALSE;

    killTimer(static_cast<int>(handle));
    delete t;

    return TRUE;
}

guint
quail_event_loop::quail_input_add(int fd,
               PurpleInputCondition cond,
               PurpleInputFunction func,
               gpointer userData)
{
    qDebug() << "quail_application::quail_input_add";
    bool bRead = false;
    bool bWrite = false;
    QSocketNotifier* notifier;

    if (cond & PURPLE_INPUT_READ)
    {
        qDebug() << "QQuailInputNotifier::QQuailInputNotifier::READ";
        notifier = new QSocketNotifier(fd, QSocketNotifier::Read);

        bRead = true;
    }
    else if (cond & PURPLE_INPUT_WRITE)
    {
        qDebug() << "QQuailInputNotifier::QQuailInputNotifier::WRITE";
        notifier = new QSocketNotifier(fd, QSocketNotifier::Write);

        bWrite = true;
    }
    connect(notifier, SIGNAL(activated(int)),
            this, SLOT(ioInvoke(int)));

    if (!bWrite && !bRead)
        qWarning() << "QQuailInputNotifier::QQuailInputNotifier:Unknown QSocketNotifier type";

    m_io.insert(nextSourceId, new QQuailInputNotifier(fd
                                                      , cond
                                                      , func
                                                      , userData
                                                      , notifier
                                                      , nextSourceId
                                                      ));
    qDebug() << "quail_application::quail_input_add.end::" << m_io.size();
    return nextSourceId++;
}

gboolean
quail_event_loop::quail_source_remove(guint handle)
{
    qDebug() << "quail_event_loop::quail_source_remove";
    QQuailInputNotifier* notifier = m_io.take(handle);
    if (notifier == NULL)
        return FALSE;

    delete notifier;
    return TRUE;
}

int
quail_event_loop::quail_input_get_error(int /*fd*/, int */*error*/)
{
    qDebug() << "quail_application::quail_input_get_error";
    return 0;
}

guint
quail_event_loop::quail_timeout_add_seconds(guint interval,
                        GSourceFunc function,
                        gpointer data)
{
    return this->quail_timeout_add(interval * 1000,function, data);
}

void
quail_event_loop::ioInvoke(int fd)
{
    qDebug() << "QQuailInputNotifier::ioInvoke" << fd;
    QQuailInputNotifier *s = m_io.take(fd);

    if (s) {
        (*s->func)(s->userData, s->fd, (PurpleInputCondition)s->cond);

        delete s;
    }
}


static guint
qQuailTimeoutAdd(guint interval, GSourceFunc func, gpointer data)
{
    qDebug() << "qQuailTimeoutAdd" << interval;
    return quail_app->quail_timeout_add(interval, func, data);
}

static gboolean
qQuailTimeoutRemove(guint handle)
{
    qDebug() << "QQuailInputNotifier::qQuailTimeoutRemove";
    return quail_app->quail_timeout_remove(handle);
}

static guint
qQuailInputAdd(int fd,
               PurpleInputCondition cond,
               PurpleInputFunction func,
               gpointer userData)
{
    qDebug() << "QQuailInputNotifier::qQuailInputAdd";
    return quail_app->quail_input_add(fd, cond, func, userData);
}

static gboolean
qQuailSourceRemove(guint handle)
{
    return quail_app->quail_source_remove(handle);
}

static int
qQuailInputGetError(int /*fd*/, int */*error*/)
{
    qDebug() << "qQuailInputGetError";
    return 0;
}

static guint
qQuailTimeoutAddSeconds(guint interval,
                        GSourceFunc function,
                        gpointer data)
{
    return quail_app->quail_timeout_add(interval * 1000,function, data);
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
