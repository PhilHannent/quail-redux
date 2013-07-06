/**
 * @file QQuailAccountEditor.h Account editor dialog
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
#ifndef _QGAIM_ACCOUNT_EDITOR_H_
#define _QGAIM_ACCOUNT_EDITOR_H_

#include <libpurple/account.h>
#include <libpurple/plugin.h>
#include <libpurple/prpl.h>

#include <QDialog>
#include <QList>
#include <QVariant>
#include <QWidget>

class QCheckBox;
class QComboBox;
class QQuailAccountsWindow;
class QQuailProtocolBox;
class QQuailTabWidget;
class QGridLayout;
class QLabel;
class QLineEdit;
class QPixmap;
class QPushButton;
class QTabWidget;
class QVBoxLayout;

class QQuailAccountEditor : public QDialog
{
	Q_OBJECT

	public:
		QQuailAccountEditor(PurpleAccount *account = NULL,
						   QWidget *parent = NULL, const char *name = NULL,
                           Qt::WindowFlags fl = 0);
		~QQuailAccountEditor();

		void setAccountsWindow(QQuailAccountsWindow *accountsWin);

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

		void registerClicked();

		void accept();

	private:
		PurpleAccount *account;
		PurplePlugin *plugin;
		PurplePluginProtocolInfo *prplInfo;

		QQuailAccountsWindow *accountsWin;

		QString protocolId;

		GList *userSplitEntries;
		GList *protocolOptEntries;

        PurpleProxyType newProxyType;

		QQuailTabWidget *tabs;

		QList<QWidget> tabList;

		/* Account tab */
        QVBoxLayout *accountBox;
		QQuailProtocolBox *protocolList;
		QLineEdit *screenNameEntry;
		QLabel *passwordLabel;
		QLineEdit *passwordEntry;
		QLineEdit *aliasEntry;
		QCheckBox *rememberPassCheck;
		QCheckBox *autoLoginCheck;
		QCheckBox *mailNotificationCheck;
		QPushButton *registerButton;

		/* Protocol tab */
        QVBoxLayout *protocolBox;

		/* Proxy tab */
        QVBoxLayout *proxyBox;
		QComboBox *proxyDropDown;
		QLineEdit *proxyHost;
		QLineEdit *proxyPort;
		QLineEdit *proxyUsername;
		QLineEdit *proxyPassword;
};

#endif /* _QGAIM_ACCOUNT_EDITOR_H_ */
