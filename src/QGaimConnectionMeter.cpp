/**
 * @file QGaimConnectionMeter.cpp Connection meters
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
#include "QGaimConnectionMeter.h"
#include "QGaimProtocolUtils.h"

#include <libgaim/debug.h>

#include <qpushbutton.h>
#include <qlabel.h>

/**************************************************************************
 * QGaimConnectionProgressBar
 **************************************************************************/
QGaimConnectionProgressBar::QGaimConnectionProgressBar(QWidget *parent,
													   const char *name,
													   WFlags fl)
	: QProgressBar(parent, name, fl)
{
	setCenterIndicator(true);
}

bool
QGaimConnectionProgressBar::setIndicator(QString &str, int progress,
										 int totalSteps)
{
	return QProgressBar::setIndicator(str, progress, totalSteps);
}


/**************************************************************************
 * QGaimConnectionMeter
 **************************************************************************/
QGaimConnectionMeter::QGaimConnectionMeter(GaimConnection *gc,
										   QWidget *parent,
										   const char *name, WFlags fl)
	: QHBox(parent, name, fl), gc(gc)
{
	QLabel *label, *icon;
	GaimAccount *account;

	setSpacing(2);
	setMargin(2);

	account = gaim_connection_get_account(gc);

	icon = new QLabel(this);
	icon->setPixmap(QGaimProtocolUtils::getProtocolIcon(account));

	label = new QLabel(gaim_account_get_username(account), this);

	progressBar = new QGaimConnectionProgressBar(this);
}

QGaimConnectionMeter::~QGaimConnectionMeter()
{
}

void
QGaimConnectionMeter::update(QString, int progress, int totalSteps)
{
	if (progress == 0)
		progressBar->setTotalSteps(totalSteps);

	progressBar->setProgress(progress);

//	progressBar->setIndicator(str, progress, totalSteps);
}

GaimConnection *
QGaimConnectionMeter::getConnection() const
{
	return gc;
}


/**************************************************************************
 * QGaimConnectionMeters
 **************************************************************************/
QGaimConnectionMeters::QGaimConnectionMeters(QWidget *parent,
											 const char *name, WFlags fl)
	: QVBox(parent, name, fl)
{
	meters.setAutoDelete(true);
}

QGaimConnectionMeters::~QGaimConnectionMeters()
{
}

void
QGaimConnectionMeters::addMeter(QGaimConnectionMeter *meter)
{
	meters.append(meter);

	meter->show();
}

void
QGaimConnectionMeters::removeMeter(QGaimConnectionMeter *meter)
{
	meters.remove(meter);
}

QGaimConnectionMeter *
QGaimConnectionMeters::addConnection(GaimConnection *gc)
{
	QGaimConnectionMeter *meter;

	meter = new QGaimConnectionMeter(gc, this);

	addMeter(meter);

	return meter;
}

QGaimConnectionMeter *
QGaimConnectionMeters::findMeter(GaimConnection *gc)
{
	QGaimConnectionMeter *meter;

	for (meter = meters.first(); meter != NULL; meter = meters.next())
	{
		if (meter->getConnection() == gc)
			return meter;
	}

	return NULL;
}
