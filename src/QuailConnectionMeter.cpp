/**
 * @file QQuailConnectionMeter.cpp Connection meters
 *
 * @Copyright (C) 2003-2004 Christian Hammond.
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
#include "QuailConnectionMeter.h"
#include "QuailProtocolUtils.h"

#include <libpurple/debug.h>

#include <qpushbutton.h>
#include <qlabel.h>

/**************************************************************************
 * QQuailConnectionProgressBar
 **************************************************************************/
QQuailConnectionProgressBar::QQuailConnectionProgressBar(QWidget *parent,
													   const char *name,
													   WFlags fl)
	: QProgressBar(parent, name, fl)
{
	setCenterIndicator(true);
}

bool
QQuailConnectionProgressBar::setIndicator(QString &str, int progress,
										 int totalSteps)
{
	return QProgressBar::setIndicator(str, progress, totalSteps);
}


/**************************************************************************
 * QQuailConnectionMeter
 **************************************************************************/
QQuailConnectionMeter::QQuailConnectionMeter(PurpleConnection *gc,
										   QWidget *parent,
										   const char *name, WFlags fl)
	: QHBox(parent, name, fl), gc(gc)
{
	QLabel *label, *icon;
	PurpleAccount *account;

	setSpacing(2);
	setMargin(2);

	account = gaim_connection_get_account(gc);

	icon = new QLabel(this);
	icon->setPixmap(QQuailProtocolUtils::getProtocolIcon(account));

	label = new QLabel(gaim_account_get_username(account), this);

	progressBar = new QQuailConnectionProgressBar(this);
}

QQuailConnectionMeter::~QQuailConnectionMeter()
{
}

void
QQuailConnectionMeter::update(QString, int progress, int totalSteps)
{
	if (progress == 0)
		progressBar->setTotalSteps(totalSteps);

	progressBar->setProgress(progress);

//	progressBar->setIndicator(str, progress, totalSteps);
}

PurpleConnection *
QQuailConnectionMeter::getConnection() const
{
	return gc;
}


/**************************************************************************
 * QQuailConnectionMeters
 **************************************************************************/
QQuailConnectionMeters::QQuailConnectionMeters(QWidget *parent,
											 const char *name, WFlags fl)
	: QVBox(parent, name, fl)
{
	meters.setAutoDelete(true);
}

QQuailConnectionMeters::~QQuailConnectionMeters()
{
}

void
QQuailConnectionMeters::addMeter(QQuailConnectionMeter *meter)
{
	meters.append(meter);

	meter->show();
}

void
QQuailConnectionMeters::removeMeter(QQuailConnectionMeter *meter)
{
	meters.remove(meter);
}

QQuailConnectionMeter *
QQuailConnectionMeters::addConnection(PurpleConnection *gc)
{
	QQuailConnectionMeter *meter;

	meter = new QQuailConnectionMeter(gc, this);

	addMeter(meter);

	return meter;
}

QQuailConnectionMeter *
QQuailConnectionMeters::findMeter(PurpleConnection *gc)
{
	QQuailConnectionMeter *meter;

	for (meter = meters.first(); meter != NULL; meter = meters.next())
	{
		if (meter->getConnection() == gc)
			return meter;
	}

	return NULL;
}
