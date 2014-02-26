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
class QTabWidget;

class quail_pref_page : public QWidget
{
	Q_OBJECT

	public:
        quail_pref_page(QWidget *parent = 0)
            : QWidget(parent), dirty(false) { }

		void setDirty(bool dirty) { this->dirty = dirty; }
		bool isDirty() const { return dirty; }

        void accept()
            { ; }

	private:
		bool dirty;
};

class quail_blist_pref_page : public quail_pref_page
{
	Q_OBJECT

	public:
        quail_blist_pref_page(QWidget *parent = 0);

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

class quail_conv_pref_page : public quail_pref_page
{
	Q_OBJECT

	public:
        quail_conv_pref_page(QWidget *parent = 0);

		void accept();

	protected:
		void buildInterface();
};

class quail_notify_pref_page : public quail_pref_page
{
	Q_OBJECT

	public:
        quail_notify_pref_page(QWidget *parent = 0);

		void accept();

	protected:
		void buildInterface();

	private:
		QCheckBox *incomingIm;
		QCheckBox *incomingChat;
		QCheckBox *useBuzzer;
		QCheckBox *useLed;
};

class quail_away_pref_page : public quail_pref_page
{
	Q_OBJECT

	public:
        quail_away_pref_page(QWidget *parent = 0);

		void accept();

	protected:
		void buildInterface();
};

class quail_proxy_pref_page : public quail_pref_page
{
	Q_OBJECT

	public:
        quail_proxy_pref_page(QWidget *parent = 0);

		void accept();

	protected:
		void buildInterface();
};

class quail_plugin_pref_page : public quail_pref_page
{
	Q_OBJECT

	public:
        quail_plugin_pref_page(QWidget *parent = 0);

		void accept();

	protected:
		void buildInterface();
};

class quail_prefs_dialog : public QWidget
{
	Q_OBJECT

	public:
        quail_prefs_dialog(QWidget *parent = 0);
        ~quail_prefs_dialog();

    protected slots:
        void slotAccept();
        void slotRejected();

    signals:
        void signalBListWindow();

	protected:
		void buildInterface();

	private:
        QWidget *m_parent;
        QTabWidget *tabs;

        quail_pref_page *blistPage;
        quail_pref_page *convPage;
        quail_pref_page *notifyPage;
        quail_pref_page *awayIdlePage;
        quail_pref_page *proxyPage;
        quail_pref_page *pluginPage;
};

#endif /* _QUAIL_PREFS_DIALOG_H_ */
