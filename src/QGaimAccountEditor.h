/**
 * @file QGaimAccountEditor.h Account editor dialog
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
#ifndef _QGAIM_ACCOUNT_EDITOR_H_
#define _QGAIM_ACCOUNT_EDITOR_H_

#include <libgaim/account.h>
#include <libgaim/plugin.h>
#include <libgaim/prpl.h>

#include <qdialog.h>
#include <qlist.h>
#include <qvariant.h>
#include <qwidget.h>

class QCheckBox;
class QComboBox;
class QGaimAccountsWindow;
class QGaimProtocolBox;
class QGaimTabWidget;
class QGridLayout;
class QLabel;
class QLineEdit;
class QPixmap;
class QTabWidget;
class QVBox;

class QGaimAccountEditor : public QDialog
{
	Q_OBJECT

	public:
		QGaimAccountEditor(GaimAccount *account = NULL,
						   QWidget *parent = NULL, const char *name = NULL,
						   WFlags fl = 0);
		~QGaimAccountEditor();

		void setAccountsWindow(QGaimAccountsWindow *accountsWin);

	protected:
		virtual void buildInterface();
		void buildTabs();

		QWidget *buildAccountTab();
		QWidget *buildProtocolTab();
		QWidget *buildProxyTab();

		void buildLoginOpts(QGridLayout *grid, QWidget *parent, int &row);
		void buildUserOpts(QGridLayout *grid, QWidget *parent, int &row);

	protected slots:
		void proxyTypeChanged(int index);
		void protocolChanged(int index);

		void accept();

	private:
		GaimAccount *account;
		GaimPlugin *plugin;
		GaimPluginProtocolInfo *prplInfo;

		QGaimAccountsWindow *accountsWin;

		QString protocolId;
		GaimProtocol protocol;

		GList *userSplitEntries;
		GList *protocolOptEntries;

		GaimProxyType newProxyType;

		QGaimTabWidget *tabs;

		QList<QWidget> tabList;

		/* Account tab */
		QVBox *accountBox;
		QGaimProtocolBox *protocolList;
		QLineEdit *screenNameEntry;
		QLabel *passwordLabel;
		QLineEdit *passwordEntry;
		QLineEdit *aliasEntry;
		QCheckBox *rememberPassCheck;
		QCheckBox *autoLoginCheck;

		QCheckBox *mailNotificationCheck;

		/* Protocol tab */
		QVBox *protocolBox;

		/* Proxy tab */
		QVBox *proxyBox;
		QComboBox *proxyDropDown;
		QLineEdit *proxyHost;
		QLineEdit *proxyPort;
		QLineEdit *proxyUsername;
		QLineEdit *proxyPassword;
};

#endif /* _QGAIM_ACCOUNT_EDITOR_H_ */
