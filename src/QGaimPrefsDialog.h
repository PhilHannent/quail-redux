/**
 * @file QGaimPrefsWindow.h Preferences window
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
#ifndef _QGAIM_PREFS_DIALOG_H_
#define _QGAIM_PREFS_DIALOG_H_

#include <qdialog.h>

class OTabWidget;
class QCheckBox;

class QGaimPrefPage : public QWidget
{
	Q_OBJECT

	public:
		QGaimPrefPage(QWidget *parent = 0, const char *name = 0)
			: QWidget(parent, name), dirty(false) { }

		void setDirty(bool dirty) { this->dirty = dirty; }
		bool isDirty() const { return dirty; }

		virtual void accept() = 0;

	private:
		bool dirty;
};

class QGaimBlistPrefPage : public QGaimPrefPage
{
	Q_OBJECT

	public:
		QGaimBlistPrefPage(QWidget *parent = 0, const char *name = 0);

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

class QGaimConvPrefPage : public QGaimPrefPage
{
	Q_OBJECT

	public:
		QGaimConvPrefPage(QWidget *parent = 0, const char *name = 0);

		void accept();

	protected:
		void buildInterface();
};

class QGaimNotifyPrefPage : public QGaimPrefPage
{
	Q_OBJECT

	public:
		QGaimNotifyPrefPage(QWidget *parent = 0, const char *name = 0);

		void accept();

	protected:
		void buildInterface();

	private:
		QCheckBox *incomingIm;
		QCheckBox *incomingChat;
		QCheckBox *useBuzzer;
		QCheckBox *useLed;
};

class QGaimAwayPrefPage : public QGaimPrefPage
{
	Q_OBJECT

	public:
		QGaimAwayPrefPage(QWidget *parent = 0, const char *name = 0);

		void accept();

	protected:
		void buildInterface();
};

class QGaimProxyPrefPage : public QGaimPrefPage
{
	Q_OBJECT

	public:
		QGaimProxyPrefPage(QWidget *parent = 0, const char *name = 0);

		void accept();

	protected:
		void buildInterface();
};

class QGaimPluginPrefPage : public QGaimPrefPage
{
	Q_OBJECT

	public:
		QGaimPluginPrefPage(QWidget *parent = 0, const char *name = 0);

		void accept();

	protected:
		void buildInterface();
};

class QGaimPrefsDialog : public QDialog
{
	Q_OBJECT

	public:
		QGaimPrefsDialog(QWidget *parent = NULL, const char *name = NULL,
						 WFlags fl = 0);
		~QGaimPrefsDialog();

		virtual void accept();
		virtual void done(int r);

	protected:
		void buildInterface();

	private:
		OTabWidget *tabs;

		QGaimPrefPage *blistPage;
		QGaimPrefPage *convPage;
		QGaimPrefPage *notifyPage;
		QGaimPrefPage *awayIdlePage;
		QGaimPrefPage *proxyPage;
		QGaimPrefPage *pluginPage;
};

#endif /* _QGAIM_PREFS_DIALOG_H_ */
