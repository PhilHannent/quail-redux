/**
 * @file QGaim.h Main Gaim class
 *
 * @Copyright (C) 2003 Christian Hammond.
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
#ifndef _QGAIM_H_
#define _QGAIM_H_

#include <libgaim/conversation.h>

#include <qpe/qpeapplication.h>
#include <qvariant.h>

#include "QGaimBListWindow.h"

class QGaim : QObject
{
	Q_OBJECT
	
	public:
		QGaim(int argc, char **argv);
		~QGaim();

		void init();
		int exec();

		QGaimBListWindow *getBlistWindow() const;
		QGaimAccountsWindow *getAccountsWindow() const;

		void setLastActiveConvWindow(GaimWindow *win);
		GaimWindow *getLastActiveConvWindow() const;

	public slots:
		void showBlistWindow();
		void showAccountsWindow();

	private slots:
		void doMainLoop();

	private:
		QPEApplication *app;
		QGaimBListWindow *blistWin;
		QGaimAccountsWindow *accountsWin;
		GaimWindow *lastConvWin;
};

QGaim *qGaimGetHandle();

#endif /* _QGAIM_H_ */
