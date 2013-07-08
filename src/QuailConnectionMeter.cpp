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

#include <QPushButton>
#include <QLabel>

/**************************************************************************
 * QQuailConnectionProgressBar
 **************************************************************************/
QQuailConnectionProgressBar::QQuailConnectionProgressBar(QWidget *parent,
													   const char *name,
                                                       Qt::WindowFlags fl)
    : QProgressBar(parent)
{
}

/**************************************************************************
 * QQuailConnectionMeter
 **************************************************************************/
QQuailConnectionMeter::QQuailConnectionMeter(PurpleConnection *gc,
										   QWidget *parent,
                                           const char *name, Qt::WindowFlags fl)
    : QWidget(parent), gc(gc)
{
    hbox = new QHBoxLayout(this);

	QLabel *label, *icon;
	PurpleAccount *account;

	account = purple_connection_get_account(gc);

    icon = new QLabel();
	icon->setPixmap(QQuailProtocolUtils::getProtocolIcon(account));
    hbox->addWidget(icon);

    label = new QLabel(purple_account_get_username(account));
    hbox->addWidget(label);

    progressBar = new QQuailConnectionProgressBar();
    hbox->addWidget(progressBar);
}

QQuailConnectionMeter::~QQuailConnectionMeter()
{
}

void
QQuailConnectionMeter::update(QString, int progress, int totalSteps)
{
    progressBar->setValue(progress);

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
                                             const char *name, Qt::WindowFlags fl)
    : QWidget(parent, fl)
{
    vbox = new QVBoxLayout(this);
}

QQuailConnectionMeters::~QQuailConnectionMeters()
{
}

void
QQuailConnectionMeters::addMeter(QQuailConnectionMeter *meter)
{
	meters.append(meter);

    show();
}

void
QQuailConnectionMeters::removeMeter(QQuailConnectionMeter *meter)
{
    meters.removeAt(meters.indexOf(meter));
}

QQuailConnectionMeter *
QQuailConnectionMeters::addConnection(PurpleConnection *gc)
{
    QQuailConnectionMeter *meter = new QQuailConnectionMeter(gc, this);

	addMeter(meter);

	return meter;
}

QQuailConnectionMeter *
QQuailConnectionMeters::findMeter(PurpleConnection *gc)
{
    for (int i = 0; i < meters.size(); ++i)
	{
        if (meters.at(i)->getConnection() == gc)
            return meters.at(i);
	}

    return 0;
}
