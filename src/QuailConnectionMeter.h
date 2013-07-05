/**
 * @file QGaimConnectionMeter.h Connection meters
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
#ifndef _QGAIM_CONNECTION_METER_H_
#define _QGAIM_CONNECTION_METER_H_

#include <QHBoxLayout>
#include <qlist.h>
#include <qprogressbar.h>
#include <qwidget.h>
#include <qvariant.h>
#include <QVBoxLayout>

#include <libpurple/connection.h>

class QProgressBar;
class QVBox;

class QGaimConnectionProgressBar : public QProgressBar
{
	Q_OBJECT

	public:
		QGaimConnectionProgressBar(QWidget *parent = NULL,
								   const char *name = NULL, WFlags fl = 0);

		bool setIndicator(QString &str, int progress, int totalSteps);
};

class QGaimConnectionMeter : public QHBox
{
	Q_OBJECT

	public:
		QGaimConnectionMeter(GaimConnection *gc, QWidget *parent = NULL,
							 const char *name = NULL, WFlags fl = 0);
		virtual ~QGaimConnectionMeter();

		void update(QString str, int totalSteps, int progress);

		GaimConnection *getConnection() const;

	private:
		QGaimConnectionProgressBar *progressBar;
		GaimConnection *gc;
};

class QGaimConnectionMeters : public QVBox
{
	Q_OBJECT

	public:
		QGaimConnectionMeters(QWidget *parent = NULL,
							  const char *name = NULL, WFlags fl = 0);
		virtual ~QGaimConnectionMeters();

		void addMeter(QGaimConnectionMeter *meter);
		void removeMeter(QGaimConnectionMeter *meter);

		QGaimConnectionMeter *addConnection(GaimConnection *gc);

		QGaimConnectionMeter *findMeter(GaimConnection *gc);

	public:
		QList<QGaimConnectionMeter> meters;
};

#endif /* _QGAIM_CONNECTION_METER_H_ */
