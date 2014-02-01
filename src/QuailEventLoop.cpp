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

} quail_source_info;

static gboolean qQuailSourceRemove(guint handle);

static guint nextSourceId = 0;
static QMap<guint, quail_source_info*> m_sources;
//static QuailEventDispatcherMarkTwo *mainEvent = 0;
//QThread *quail_event_thread = 0;
static quail_application* quail_app = 0;

quail_application::quail_application(int &argc, char **argv)
    : QApplication(argc, argv)
{
    quail_app = this;
//    quail_event_thread = new QThread(this);
//    quail_event_thread->start();
}

guint
quail_application::quail_timeout_add(guint interval, GSourceFunc func, gpointer data)
{
    qDebug() << "quail_application::quail_timeout_add" << interval;
    quail_source_info *info = new quail_source_info;
    info->handle = nextSourceId++;
    info->timer = new QQuailTimer(info->handle, func, data);
    info->timer->setInterval(interval);
    //QThread* test_thread = new QThread;
    //info->timer->moveToThread(test_thread);
    //connect(test_thread, SIGNAL(started()), info->timer, SLOT(startTimer()));
    qDebug() << "quail_application::quail_timeout_add.1";
    //m_sources.insert(info->handle, info);
    //info->timer->startTimer();
    info->timer->startTimer();
    //test_thread->start();
    //info->timer->moveToThread(quail_event_thread);
    qDebug() << "quail_application::quail_timeout_add.2";
    qDebug() << "quail_application::quail_timeout_add.end";
    return info->handle;
}

gboolean
quail_application::quail_timeout_remove(guint handle)
{
    qDebug() << "quail_application::quail_timeout_remove";
    qQuailSourceRemove(handle);

    return 0;
}

guint
quail_application::quail_input_add(int fd,
               PurpleInputCondition cond,
               PurpleInputFunction func,
               gpointer userData)
{
    qDebug() << "quail_application::quail_input_add";
    quail_source_info *info = new quail_source_info;

    info->handle = nextSourceId++;

    info->notifier = new QQuailInputNotifier(fd, cond, func, userData);
    m_sources.insert(info->handle, info);
    qDebug() << "quail_application::quail_input_add.end::" << info->handle;
    qDebug() << "quail_application::quail_input_add.end::" << m_sources.size();
    return info->handle;
}

gboolean
quail_application::quail_source_remove(guint handle)
{
    quail_source_info *info;

    info = m_sources.take(handle);
    if (info == NULL)
        return false;

    if (info->timer != NULL) {
        delete info->timer;
        info->timer = NULL;
    } else if (info->notifier != NULL) {
        delete info->notifier;
        info->notifier = NULL;
    }
    delete info;
    return true;
}

int
quail_application::quail_input_get_error(int /*fd*/, int */*error*/)
{
    qDebug() << "quail_application::quail_input_get_error";
    return 0;
}

guint
quail_application::quail_timeout_add_seconds(guint interval,
                        GSourceFunc function,
                        gpointer data)
{
    return this->quail_timeout_add(interval * 1000,function, data);
}


QQuailTimer::QQuailTimer(guint sourceId, GSourceFunc func, gpointer data)
    : QObject(0)
    , m_timer(new QTimer(this))
    , sourceId(sourceId)
    , func(func)
    , userData(data)
{
    //qDebug() << "QQuailTimer::QQuailTimer.1";
    connect(m_timer, SIGNAL(timeout()), this, SLOT(update()));
}

void
QQuailTimer::update()
{
    //qDebug() << "QQuailTimer::update()";
    if (!func(userData))
        qQuailSourceRemove(sourceId);
}

//void
//QQuailTimer::start(int msec)
//{
//    //qDebug() << "QQuailTimer::start.1";
//    if (msec == 0)
//        update();
//    else
//        this->start(msec);
//}

QQuailInputNotifier::QQuailInputNotifier(int fd,
                                         PurpleInputCondition cond,
                                         PurpleInputFunction func,
                                         gpointer userData)
    : QObject(), func(func), userData(userData), readNotifier(NULL),
      writeNotifier(NULL)
{
    qDebug() << "QQuailInputNotifier::QQuailInputNotifier" << cond;
    qDebug() << "QQuailInputNotifier::QQuailInputNotifier" << fd;
    bool bRead = false;
    bool bWrite = false;

    if (cond & PURPLE_INPUT_READ)
    {
        qDebug() << "QQuailInputNotifier::QQuailInputNotifier::READ";
        readNotifier = new QSocketNotifier(fd, QSocketNotifier::Read);

        connect(readNotifier, SIGNAL(activated(int)),
                this, SLOT(ioInvoke(int)), Qt::DirectConnection);
        bRead = true;
    }

    if (cond & PURPLE_INPUT_WRITE)
    {
        qDebug() << "QQuailInputNotifier::QQuailInputNotifier::WRITE";
        writeNotifier = new QSocketNotifier(fd, QSocketNotifier::Write);

        connect(writeNotifier, SIGNAL(activated(int)),
                this, SLOT(ioInvoke(int)), Qt::DirectConnection);
        bWrite = true;
    }

    if (!bWrite && !bRead)
        qWarning() << "QQuailInputNotifier::QQuailInputNotifier:Unknown QSocketNotifier type";
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
    qDebug() << "QQuailInputNotifier::ioInvoke";
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
    qDebug() << "qQuailTimeoutAdd" << interval;
    return quail_app->quail_timeout_add(interval, func, data);
}

static gboolean
qQuailTimeoutRemove(guint handle)
{
    qDebug() << "QQuailInputNotifier::qQuailTimeoutRemove";
    qQuailSourceRemove(handle);
    return 0;
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
