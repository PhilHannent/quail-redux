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

class QQuailTimer : public QTimer
{
    Q_OBJECT

    public:
        QQuailTimer(guint sourceId, GSourceFunc func, gpointer data);

    public slots:
        void update();

    private:
        guint sourceId;
        GSourceFunc func;
        gpointer userData;
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

#endif /* _QUAIL_EVENT_LOOP_H_ */
