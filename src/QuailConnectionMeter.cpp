/**
 * @file QQuailConnectionMeter.cpp Connection meters
 *
 * @Copyright (C) 2003-2004 Christian Hammond.
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
#include "QuailConnectionMeter.h"
#include "QuailProtocolUtils.h"

#include <libpurple/debug.h>

#include <QDebug>
#include <QPushButton>
#include <QLabel>

/**************************************************************************
 * QQuailConnectionProgressBar
 **************************************************************************/
QQuailConnectionProgressBar::QQuailConnectionProgressBar(QWidget *parent)
    : QProgressBar(parent)
{
}

/**************************************************************************
 * QQuailConnectionMeter
 **************************************************************************/
QQuailConnectionMeter::QQuailConnectionMeter(PurpleConnection *gc,
                                           QWidget *parent)
    : QWidget(parent), gc(gc)
{
    qDebug() << "QQuailConnectionMeter::QQuailConnectionMeter";
    hbox = new QHBoxLayout(this);

	QLabel *label, *icon;
	PurpleAccount *account;

	account = purple_connection_get_account(gc);

    icon = new QLabel();
    icon->setPixmap(quail_protocol_utils::getProtocolIcon(account));
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
QQuailConnectionMeter::update(QString, int progress, int /*totalSteps*/)
{
    qDebug() << "QQuailConnectionMeter::update";
    progressBar->setValue(progress);

}

PurpleConnection *
QQuailConnectionMeter::getConnection() const
{
    qDebug() << "QQuailConnectionMeter::getConnection";
	return gc;
}


/**************************************************************************
 * QQuailConnectionMeters
 **************************************************************************/
QQuailConnectionMeters::QQuailConnectionMeters(QWidget *parent)
    : QWidget(parent)
{
    qDebug() << "QQuailConnectionMeters::QQuailConnectionMeters";
    vbox = new QVBoxLayout(this);
}

QQuailConnectionMeters::~QQuailConnectionMeters()
{
}

void
QQuailConnectionMeters::addMeter(QQuailConnectionMeter *meter)
{
    qDebug() << "QQuailConnectionMeters::addMeter";
	meters.append(meter);

    show();
}

void
QQuailConnectionMeters::removeMeter(QQuailConnectionMeter *meter)
{
    qDebug() << "QQuailConnectionMeters::removeMeter";
    meters.removeAt(meters.indexOf(meter));
}

QQuailConnectionMeter *
QQuailConnectionMeters::addConnection(PurpleConnection *gc)
{
    qDebug() << "QQuailConnectionMeters::addConnection";
    QQuailConnectionMeter *meter = new QQuailConnectionMeter(gc, this);

	addMeter(meter);

	return meter;
}

QQuailConnectionMeter *
QQuailConnectionMeters::findMeter(PurpleConnection *gc)
{
    qDebug() << "QQuailConnectionMeters::findMeter";
    foreach (QQuailConnectionMeter* meter, meters)
	{
        if (meter->getConnection() == gc)
            return meter;
	}

    return 0;
}
