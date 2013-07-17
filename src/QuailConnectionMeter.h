/**
 * @file QQuailConnectionMeter.h Connection meters
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
#ifndef _QUAIL_CONNECTION_METER_H_
#define _QUAIL_CONNECTION_METER_H_

#include <QHBoxLayout>
#include <QList>
#include <QProgressBar>
#include <QWidget>
#include <QVariant>
#include <QVBoxLayout>

#include <libpurple/connection.h>

class QQuailConnectionProgressBar : public QProgressBar
{
	Q_OBJECT

	public:
        QQuailConnectionProgressBar(QWidget *parent = NULL);

		bool setIndicator(QString &str, int progress, int totalSteps);
};

class QQuailConnectionMeter : public QWidget
{
	Q_OBJECT

	public:
        QQuailConnectionMeter(PurpleConnection *gc, QWidget *parent = NULL);
		virtual ~QQuailConnectionMeter();

		void update(QString str, int totalSteps, int progress);

		PurpleConnection *getConnection() const;

	private:
		QQuailConnectionProgressBar *progressBar;
		PurpleConnection *gc;
        QHBoxLayout *hbox;
};

class QQuailConnectionMeters : public QWidget
{
	Q_OBJECT

	public:
        QQuailConnectionMeters(QWidget *parent = NULL);
		virtual ~QQuailConnectionMeters();

		void addMeter(QQuailConnectionMeter *meter);
		void removeMeter(QQuailConnectionMeter *meter);

		QQuailConnectionMeter *addConnection(PurpleConnection *gc);

		QQuailConnectionMeter *findMeter(PurpleConnection *gc);

	public:
        QList<QQuailConnectionMeter*> meters;
        QVBoxLayout *vbox;
};

#endif /* _QUAIL_CONNECTION_METER_H_ */
