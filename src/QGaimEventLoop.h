/**
 * @file QGaimMenuLoop.h Gaim Qt Event Loop implementation
 *
 * @Copyright (C) 2004 Christian Hammond.
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
#ifndef _QGAIM_EVENT_LOOP_H_
#define _QGAIM_EVENT_LOOP_H_

#include <libgaim/eventloop.h>

#include <qtimer.h>
#include <qsocketnotifier.h>

class QGaimTimer : public QTimer
{
	public:
		QGaimTimer(guint sourceId, GSourceFunc func, gpointer data);

	private:
		void update();

		guint sourceId;
		GSourceFunc func;
		gpointer userData;
};

class QGaimInputNotifier : public QObject
{
	Q_OBJECT

	public:
		QGaimInputNotifier(int fd, GaimInputCondition cond,
						   GaimInputFunction func, gpointer userData);
		~QGaimInputNotifier();

	private:
		void ioInvoke(int fd);

		GaimInputCondition cond;
		GaimInputFunction func;
		gpointer userData;
		QSocketNotifier *readNotifier, *writeNotifier;
};

/**
 * Returns the Qt event loop UI operations structure.
 *
 * @return The Qt event loop UI operations structure.
 */
GaimEventLoopUiOps *qGaimGetEventLoopUiOps(void);

/**
 * Initializes the Qt event loop subsystem.
 */
void qGaimEventLoopInit(void);

/**
 * Uninitializes the Qt evnet loop subsystem.
 */
void qGaimEventLoopUninit(void);

#endif /* _QGAIM_EVENT_LOOP_H_ */
