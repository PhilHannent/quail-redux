/**
 * @file QQuailMainWindow.h Main gaim window
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
#ifndef _QUAIL_MAIN_WINDOW_H_
#define _QUAIL_MAIN_WINDOW_H_

#include <QMainWindow>
#include <QStackedWidget>

#include <libpurple/conversation.h>

#include "QuailBListWindow.h"

class QQuailAccountsWindow;
class QQuailConnectionMeters;
class QQuailConvWindow;
class QQuailConversation;

class QQuailMainWindow : public QMainWindow
{
	Q_OBJECT

	public:
        static QString appName() { return QString::fromLatin1(APP_NAME); }

	public:
		QQuailMainWindow(QWidget *parent = 0, const char *name = 0,
                        Qt::WindowFlags fl = 0);
		~QQuailMainWindow();

        void addConversationWindow(PurpleConversation *win);
        void removeConversationWindow(QQuailConversation *win);

		QQuailBListWindow *getBlistWindow() const;
		QQuailAccountsWindow *getAccountsWindow() const;

        void setLastActiveConvWindow(QQuailConversation *win);
        QQuailConversation *getLastActiveConvWindow() const;

        QStackedWidget *getWidgetStack() const;
		QQuailConnectionMeters *getMeters() const;

	public slots:
		void showBlistWindow();
		void showAccountsWindow();

	protected:
		void buildInterface();
		void initCore();

		void closeEvent(QCloseEvent *e);

	private:
        QStackedWidget *widgetStack;
		QQuailAccountsWindow *accountsWin;
		QQuailBListWindow *blistWin;
		QQuailConnectionMeters *meters;

        QQuailConversation *lastConvWin;

		int nextConvWinId;
};

QQuailMainWindow *qQuailGetMainWindow();

#endif /* _QUAIL_MAIN_WINDOW_H_ */
