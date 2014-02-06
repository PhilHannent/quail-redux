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
#include <QApplication>
#include <QDebug>
#include <QMap>
#include <QMutexLocker>
#include <QSocketNotifier>
#include <QThread>
#include <QTimerEvent>

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
    int id;
    if (QThread::currentThread() == qApp->thread())
    {
        qDebug() << "quail_application::quail_timeout_add.1a";
        id = QObject::startTimer(interval);
    } else {
        qDebug() << "quail_application::quail_timeout_add.1b";
        QMetaObject::invokeMethod(this, "startTimer", Qt::BlockingQueuedConnection,
                                  Q_ARG(int, interval), Q_ARG(int*, &id));
    }
    QMutexLocker locker(&m_timer_mutex);
    qDebug() << "quail_application::quail_timeout_add.2";
    m_timers.insert(id,new QQuailTimer(id, func, data));
    qDebug() << "quail_application::quail_timeout_add.end";
    return static_cast<guint>(id);
}

void
quail_event_loop::startTimer(int interval, int *id)
{
    QMutexLocker locker(&m_timer_mutex);
    *id = QObject::startTimer(interval);
}

void
quail_event_loop::timerEvent(QTimerEvent* e)
{
    m_timer_mutex.lock();
    qDebug() << "quail_event_loop::timerEvent()" << e->timerId();
    QQuailTimer *t = m_timers.take(e->timerId());
    m_timer_mutex.unlock();
    if (t == 0)
        return;
    qDebug() << "quail_event_loop::timerEvent().1";

    if (!(*t->func)(t->data))
    {
        QMutexLocker locker(&m_timer_mutex);
        qDebug() << "quail_event_loop::timerEvent().2";
        killTimer(e->timerId());
    }
    qDebug() << "quail_event_loop::timerEvent().end";

}


gboolean
quail_event_loop::quail_timeout_remove(guint handle)
{
    QMutexLocker locker(&m_timer_mutex);
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
        qDebug() << "quail_event_loop::quail_input_add::READ";
        notifier = new QSocketNotifier(fd, QSocketNotifier::Read);

        bRead = true;
    }
    else
    {
        qDebug() << "quail_event_loop::quail_input_add::WRITE";
        notifier = new QSocketNotifier(fd, QSocketNotifier::Write);

        bWrite = true;
    }
    connect(notifier, SIGNAL(activated(int)),
            this, SLOT(ioInvoke(int)));

    if (!bWrite && !bRead)
        qWarning() << "quail_event_loop::quail_input_add:Unknown QSocketNotifier type";
    notifier->setProperty("sourceId", nextSourceId);
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
    QMap<guint, QQuailInputNotifier *>::iterator i = m_io.find(handle);
    if (i == m_io.end())
        return FALSE;

    QQuailInputNotifier* notifier = i.value();
    if (notifier == NULL)
        return FALSE;

    notifier->notifier->deleteLater();
    m_io.erase(i);
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
    qDebug() << "QQuailInputNotifier::ioInvoke::fd" << fd;
    QSocketNotifier *socket = qobject_cast<QSocketNotifier *>(sender());
    guint id = socket->property("sourceId").toUInt();

    QMap<guint, QQuailInputNotifier *>::iterator i = m_io.find(id);
    if (i == m_io.end())
        return;

    QQuailInputNotifier* s = i.value();
    if (s == NULL)
        return;

    (*s->func)(s->userData, s->fd, s->cond);

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
