﻿/**
 * @file QQuailMainWindow.h Main gaim window
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
#ifndef _QUAIL_MAIN_WINDOW_H_
#define _QUAIL_MAIN_WINDOW_H_

#include <QMainWindow>
#include <QTranslator>
#include <QTimer>

#include <libpurple/conversation.h>

class quail_accounts_window;
class QQuailBListWindow;
class QQuailConnectionMeters;
class QQuailConvWindow;
class QQuailConversation;
class QQuailPrefsDialog;

class QMenu;
class QStackedWidget;
class QSystemTrayIcon;

class quail_main_window : public QMainWindow
{
	Q_OBJECT

	public:
        static QString appName() { return QString::fromLatin1(APP_NAME); }

	public:
        quail_main_window(QWidget *parent = 0);
        ~quail_main_window();

        void addConversationWindow(PurpleConversation *win);
        void removeConversationWindow(QQuailConversation *win);

		QQuailBListWindow *getBlistWindow() const;
		quail_accounts_window *getAccountsWindow() const;

        void setLastActiveConvWindow(QQuailConversation *win);
        QQuailConversation *getLastActiveConvWindow() const;

        QStackedWidget *getWidgetStack() const;
		QQuailConnectionMeters *getMeters() const;

        QTimer *getNewTimer()
        {   return new QTimer(this); }

	public slots:
		void showBlistWindow();
		void showAccountsWindow();
        void showConvWindow();
        void showPrefWindow();
        void retranslateUi(QWidget * /*currentForm*/);

	protected:
		void closeEvent(QCloseEvent *e);
        void changeEvent(QEvent *event);

    signals:
        void signalLanguageCode(QString lang);

    private slots:
        void slotStateChanges(QAction*);
        void slotSaveSettings();
        void slotReadSettings();
        void slot_quit();

	private:
        void buildInterface();
        void initCore();
        void createTrayIcon();
        void createActions();
        QMenu* createStatusMenu();

        void switchLanguage();

        QStackedWidget *widgetStack;
		quail_accounts_window *accountsWin;
		QQuailBListWindow *blistWin;
		QQuailConnectionMeters *meters;
        QQuailConvWindow *convWin;
        QQuailConversation *lastConvWin;
        QQuailPrefsDialog *prefWin;
        QAction *actShowBuddyList;
        QAction *actShowAccounts;
        QAction *actMinimize;
        QAction *actQuit;
        QMenu *trayIconMenu;
        QSystemTrayIcon *trayIcon;
        int nextConvWinId;
        QString m_language;
        QTranslator 	appTranslator;
        QTranslator 	qtTranslator;
        QMenu* m_statusMenu;

};

quail_main_window *qQuailGetMainWindow();

#endif /* _QUAIL_MAIN_WINDOW_H_ */
