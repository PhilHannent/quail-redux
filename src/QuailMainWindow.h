/**
 * @file QGaimMainWindow.h Main gaim window
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
#ifndef _QGAIM_MAIN_WINDOW_H_
#define _QGAIM_MAIN_WINDOW_H_

#include <qmainwindow.h>
#include <qwidgetstack.h>

#include <libgaim/conversation.h>

#include "QuailBListWindow.h"

class QGaimAccountsWindow;
class QGaimConnectionMeters;
class QGaimConvWindow;

class QGaimMainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		static QString appName() { return QString::fromLatin1("gaim"); }

	public:
		QGaimMainWindow(QWidget *parent = 0, const char *name = 0,
						WFlags fl = 0);
		~QGaimMainWindow();

		void addConversationWindow(QGaimConvWindow *win);
		void removeConversationWindow(QGaimConvWindow *win);

		QGaimBListWindow *getBlistWindow() const;
		QGaimAccountsWindow *getAccountsWindow() const;

		void setLastActiveConvWindow(GaimConvWindow *win);
		GaimConvWindow *getLastActiveConvWindow() const;

		QWidgetStack *getWidgetStack() const;
		QGaimConnectionMeters *getMeters() const;

	public slots:
		void showBlistWindow();
		void showAccountsWindow();

	protected:
		void buildInterface();
		void initCore();

		void closeEvent(QCloseEvent *e);

	private:
		QWidgetStack *widgetStack;
		QGaimAccountsWindow *accountsWin;
		QGaimBListWindow *blistWin;
		QGaimConnectionMeters *meters;

		GaimConvWindow *lastConvWin;

		int nextConvWinId;
};

QGaimMainWindow *qGaimGetMainWindow();

#endif /* _QGAIM_MAIN_WINDOW_H_ */
