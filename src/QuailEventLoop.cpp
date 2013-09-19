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
#include <QThread>

typedef struct
{
    guint handle;

    union
    {
        QQuailTimer *timer;
        QQuailInputNotifier *notifier;
    };

} QQuailSourceInfo;

static gboolean qQuailSourceRemove(guint handle);

static guint nextSourceId = 0;
static QMap<guint, QQuailSourceInfo*> m_sources;
//static QThread *quailThread = new QThread();

QQuailTimer::QQuailTimer(guint sourceId, GSourceFunc func, gpointer data)
    : sourceId(sourceId), func(func), userData(data), t(0)
{
    qDebug() << "QQuailTimer::QQuailTimer.1";
}

void
QQuailTimer::update()
{
    qDebug() << "QQuailTimer::update()";
    if (!func(userData))
        qQuailSourceRemove(sourceId);
}

void
QQuailTimer::start(int msec)
{
    qDebug() << "QQuailTimer::start.1";
//    if (mainWin == 0)
//        return;

    if (t == 0)
    {
        t = mainWin->getNewTimer();
        connect(t, SIGNAL(timeout()),
                this, SLOT(update()));
    }
    t->start(msec);
}

QQuailInputNotifier::QQuailInputNotifier(int fd,
                                         PurpleInputCondition cond,
                                         PurpleInputFunction func,
                                         gpointer userData)
    : QObject(), func(func), userData(userData), readNotifier(NULL),
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


static guint
qQuailTimeoutAdd(guint interval, GSourceFunc func, gpointer data)
{
    //qDebug() << "QQuailInputNotifier::qQuailTimeoutAdd";
    QQuailSourceInfo *info = new QQuailSourceInfo;

    info->handle = nextSourceId++;

    info->timer = new QQuailTimer(info->handle, func, data);
    //info->timer->moveToThread(quailThread);
    info->timer->start(interval);

    m_sources.insert(info->handle, info);

    return info->handle;
}

static gboolean
qQuailTimeoutRemove(guint handle)
{
    //qDebug() << "QQuailInputNotifier::qQuailTimeoutRemove";
    qQuailSourceRemove(handle);

    return 0;
}

static guint
qQuailInputAdd(int fd,
               PurpleInputCondition cond,
               PurpleInputFunction func,
               gpointer userData)
{
    //qDebug() << "QQuailInputNotifier::qQuailInputAdd";
    QQuailSourceInfo *info = new QQuailSourceInfo;

    info->handle = nextSourceId++;

    info->notifier = new QQuailInputNotifier(fd, cond, func, userData);

    m_sources.insert(info->handle, info);

    return info->handle;
}

static gboolean
qQuailSourceRemove(guint handle)
{
    //qDebug() << "QQuailInputNotifier::qQuailSourceRemove";
    QQuailSourceInfo *info;

    info = m_sources.value(handle);

    if (info == NULL)
        return false;

    m_sources.remove(handle);

    if (info->timer != NULL)
        delete info->timer;
    else if (info->notifier != NULL)
        delete info->notifier;

    delete info;
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

