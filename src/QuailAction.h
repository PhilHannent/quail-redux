/**
 * @file QGaimAction.h QAction subclass
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
#ifndef _QGAIM_ACTION_H_
#define _QGAIM_ACTION_H_

#include <qaction.h>

class QGaimAction : public QAction
{
	Q_OBJECT

	public:
		QGaimAction(QObject *parent = NULL, const char *name = NULL,
					bool toggle = false, void *data = NULL)
			: QAction(parent, name, toggle), userData(data)
		{ setupSignals(); }

		QGaimAction(const QString &text, const QIconSet &icon,
					const QString &menuText, int accel, QObject *parent,
					const char *name = NULL, bool toggle = false,
					void *data = NULL)
			: QAction(text, icon, menuText, accel, parent, name, toggle),
			  userData(data)
		{ setupSignals(); }

		QGaimAction(const QString &text, const QString &menuText, int accel,
					QObject *parent, const char *name = NULL,
					bool toggle = false, void *data = NULL)
			: QAction(text, menuText, accel, parent, name, toggle),
			  userData(data)
		{ setupSignals(); }

		void setUserData(void *data)  { userData  = data; }
		void setUserData2(void *data) { userData2 = data; }
		void *getUserData() const     { return userData;  }
		void *getUserData2() const    { return userData2; }

	signals:
		void activated(void *data);
		void activated(void *data1, void *data2);

	private slots:
		void activatedSlot() { emit activated(getUserData()); }
		void activated2Slot() { emit activated(getUserData(), getUserData2()); }

	private:
		void setupSignals()
		{
			connect(this, SIGNAL(activated()), this, SLOT(activatedSlot()));
			connect(this, SIGNAL(activated()), this, SLOT(activated2Slot()));
		}

	private:
		void *userData, *userData2;
};

#endif /* _QGAIM_ACTION_H_ */