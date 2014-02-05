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

#include <QObject>
#include <QMap>

class QSocketNotifier;

class quail_event_loop : public QObject
{
    Q_OBJECT
    struct QQuailTimer
    {
        QQuailTimer(guint sourceId = 0, GSourceFunc func = 0, gpointer data = 0)
                : sourceId(sourceId)
                , func(func)
                , data(data)
            {; }
        guint sourceId;
        GSourceFunc func;
        gpointer data;
    };

    struct QQuailInputNotifier
    {
        QQuailInputNotifier(int fd
                            , PurpleInputCondition cond
                            , PurpleInputFunction func
                            , gpointer userData
                            , QSocketNotifier* notifier
                            , guint sourceId
                            )
            : fd(fd)
            , cond(cond)
            , func(func)
            , userData(userData)
            , notifier(notifier)
            , sourceId(sourceId) {}
        int fd;
        PurpleInputCondition cond;
        PurpleInputFunction func;
        gpointer userData;
        QSocketNotifier *notifier;
        guint sourceId;
    };

public:
    explicit quail_event_loop(QObject* parent = 0);

    Q_INVOKABLE void startTimer(int interval, int *id);
    guint       quail_timeout_add(guint interval, GSourceFunc func, gpointer data);
    gboolean    quail_timeout_remove(guint handle);
    guint       quail_input_add(int fd,
                        PurpleInputCondition cond,
                        PurpleInputFunction func,
                        gpointer userData);
    gboolean    quail_source_remove(guint handle);
    int         quail_input_get_error(int /*fd*/, int */*error*/);
    guint       quail_timeout_add_seconds(guint interval,
                        GSourceFunc function,
                        gpointer data);
protected:
    virtual void timerEvent(QTimerEvent *event);

private slots:
    void ioInvoke(int fd);

private:
    QMap<int, QQuailTimer*> m_timers;
    QMap<guint, QQuailInputNotifier*> m_io;
    guint nextSourceId;

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

#endif /* _QUAIL_EVENT_LOOP_H_ */
