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

class QTabWidget;
class QCheckBox;

class QQuailPrefPage : public QWidget
{
	Q_OBJECT

	public:
        QQuailPrefPage(QWidget *parent = 0)
            : QWidget(parent), dirty(false) { }

		void setDirty(bool dirty) { this->dirty = dirty; }
		bool isDirty() const { return dirty; }

		virtual void accept() = 0;

	private:
		bool dirty;
};

class QQuailBlistPrefPage : public QQuailPrefPage
{
	Q_OBJECT

	public:
        QQuailBlistPrefPage(QWidget *parent = 0);

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
        QQuailConvPrefPage(QWidget *parent = 0);

		void accept();

	protected:
		void buildInterface();
};

class QQuailNotifyPrefPage : public QQuailPrefPage
{
	Q_OBJECT

	public:
        QQuailNotifyPrefPage(QWidget *parent = 0);

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
        QQuailAwayPrefPage(QWidget *parent = 0);

		void accept();

	protected:
		void buildInterface();
};

class QQuailProxyPrefPage : public QQuailPrefPage
{
	Q_OBJECT

	public:
        QQuailProxyPrefPage(QWidget *parent = 0);

		void accept();

	protected:
		void buildInterface();
};

class QQuailPluginPrefPage : public QQuailPrefPage
{
	Q_OBJECT

	public:
        QQuailPluginPrefPage(QWidget *parent = 0);

		void accept();

	protected:
		void buildInterface();
};

class QQuailPrefsDialog : public QDialog
{
	Q_OBJECT

	public:
        QQuailPrefsDialog(QWidget *parent = NULL);
		~QQuailPrefsDialog();

		virtual void accept();
		virtual void done(int r);

	protected:
		void buildInterface();

	private:
        QTabWidget *tabs;

		QQuailPrefPage *blistPage;
		QQuailPrefPage *convPage;
		QQuailPrefPage *notifyPage;
		QQuailPrefPage *awayIdlePage;
		QQuailPrefPage *proxyPage;
		QQuailPrefPage *pluginPage;
};

#endif /* _QUAIL_PREFS_DIALOG_H_ */
