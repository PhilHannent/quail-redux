﻿/**
 * @file QQuailMenuLoop.cpp Gaim Qt Event Loop implementation
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
#include "QuailEventLoop.h"
#include <QMap>

typedef struct
{
	guint handle;

	union
	{
		QQuailTimer *timer;
		QQuailInputNotifier *notifier;
	};

} QQuailSourceInfo;

static void qQuailSourceRemove(guint handle);

static guint nextSourceId = 0;
static QMap<QQuailSourceInfo> sources;

QQuailTimer::QQuailTimer(guint sourceId, GSourceFunc func, gpointer data)
	: QTimer(), sourceId(sourceId), func(func), userData(data)
{
	connect(this, SIGNAL(timeout()),
			this, SLOT(update()));
}

void
QQuailTimer::update()
{
	if (!func(userData))
		qQuailSourceRemove(sourceId);
}

QQuailInputNotifier::QQuailInputNotifier(int fd, PurpleInputCondition cond,
									   PurpleInputFunction func,
									   gpointer userData)
	: QObject(), func(func), userData(userData), readNotifier(NULL),
	  writeNotifier(NULL)
{
	if (cond & GAIM_INPUT_READ)
	{
		readNotifier = new QSocketNotifier(fd, QSocketNotifier::Read);

		connect(readNotifier, SIGNAL(activated(int)),
				this, SLOT(ioInvoke(int)));
	}

	if (cond & GAIM_INPUT_WRITE)
	{
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
	int cond = 0;

	if (readNotifier != NULL)
		cond |= GAIM_INPUT_READ;

	if (writeNotifier != NULL)
		cond |= GAIM_INPUT_WRITE;

	func(userData, fd, (PurpleInputCondition)cond);
}


static guint
qQuailTimeoutAdd(guint interval, GSourceFunc func, gpointer data)
{
	QQuailSourceInfo *info = new QQuailSourceInfo;

	info->handle = nextSourceId++;

	info->timer = new QQuailTimer(info->handle, func, data);
	info->timer->start(interval);

	sources.insert(info->handle, info);

	return info->handle;
}

static guint
qQuailTimeoutRemove(guint handle)
{
	qQuailSourceRemove(handle);

	return 0;
}

static guint
qQuailInputAdd(int fd, PurpleInputCondition cond, PurpleInputFunction func,
			  gpointer userData)
{
	QQuailSourceInfo *info = new QQuailSourceInfo;

	info->handle = nextSourceId++;

	info->notifier = new QQuailInputNotifier(fd, cond, func, userData);

	sources.insert(info->handle, info);

	return info->handle;
}

static void
qQuailSourceRemove(guint handle)
{
	QQuailSourceInfo *info;

	info = sources.find(handle);

	if (info == NULL)
		return;

	sources.remove(handle);

	if (info->timer != NULL)
		delete info->timer;
	else if (info->notifier != NULL)
		delete info->notifier;

	delete info;
}

static GaimEventLoopUiOps eventloop_ops =
{
	qQuailTimeoutAdd,
	qQuailTimeoutRemove,
	qQuailInputAdd,
	qQuailSourceRemove
};

GaimEventLoopUiOps *
qQuailGetEventLoopUiOps(void)
{
	return &eventloop_ops;
}
