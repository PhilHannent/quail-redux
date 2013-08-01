/**
 * @file QQuailPrefsWindow.h Preferences window
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
#ifndef _QUAIL_PREFS_DIALOG_H_
#define _QUAIL_PREFS_DIALOG_H_

#include <QDialog>

class QCheckBox;
class QMainWindow;
class QTabWidget;

class QQuailPrefPage : public QWidget
{
	Q_OBJECT

	public:
        QQuailPrefPage(QMainWindow *parent = 0)
            : QWidget(), dirty(false) { }

		void setDirty(bool dirty) { this->dirty = dirty; }
		bool isDirty() const { return dirty; }

        void accept()
            { ; }

	private:
		bool dirty;
};

class QQuailBlistPrefPage : public QQuailPrefPage
{
	Q_OBJECT

	public:
        QQuailBlistPrefPage(QMainWindow *parent = 0);

		void accept();

	protected:
		void buildInterface();

	private:
		QCheckBox *idleTimes;
		QCheckBox *showWarnings;
		QCheckBox *groupCount;
		QCheckBox *dimIdle;
		QCheckBox *largeIcons;
};

class QQuailConvPrefPage : public QQuailPrefPage
{
	Q_OBJECT

	public:
        QQuailConvPrefPage(QMainWindow *parent = 0);

		void accept();

	protected:
		void buildInterface();
};

class QQuailNotifyPrefPage : public QQuailPrefPage
{
	Q_OBJECT

	public:
        QQuailNotifyPrefPage(QMainWindow *parent = 0);

		void accept();

	protected:
		void buildInterface();

	private:
		QCheckBox *incomingIm;
		QCheckBox *incomingChat;
		QCheckBox *useBuzzer;
		QCheckBox *useLed;
};

class QQuailAwayPrefPage : public QQuailPrefPage
{
	Q_OBJECT

	public:
        QQuailAwayPrefPage(QMainWindow *parent = 0);

		void accept();

	protected:
		void buildInterface();
};

class QQuailProxyPrefPage : public QQuailPrefPage
{
	Q_OBJECT

	public:
        QQuailProxyPrefPage(QMainWindow *parent = 0);

		void accept();

	protected:
		void buildInterface();
};

class QQuailPluginPrefPage : public QQuailPrefPage
{
	Q_OBJECT

	public:
        QQuailPluginPrefPage(QMainWindow *parent = 0);

		void accept();

	protected:
		void buildInterface();
};

class QQuailPrefsDialog : public QWidget
{
	Q_OBJECT

	public:
        QQuailPrefsDialog(QMainWindow *parent = NULL);
		~QQuailPrefsDialog();

        //virtual void accept() = 0;
        //virtual void done(int r);

    protected slots:
        void slotAccept();
        void slotRejected();

    signals:
        void signalBListWindow();

	protected:
		void buildInterface();

	private:
        QMainWindow *parentMainWindow;
        QTabWidget *tabs;

		QQuailPrefPage *blistPage;
		QQuailPrefPage *convPage;
		QQuailPrefPage *notifyPage;
		QQuailPrefPage *awayIdlePage;
		QQuailPrefPage *proxyPage;
		QQuailPrefPage *pluginPage;
};

#endif /* _QUAIL_PREFS_DIALOG_H_ */
