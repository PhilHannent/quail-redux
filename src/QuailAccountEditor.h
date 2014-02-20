/**
 * @file QQuailAccountEditor.h Account editor dialog
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
#ifndef _QUAIL_ACCOUNT_EDITOR_H_
#define _QUAIL_ACCOUNT_EDITOR_H_

#include <libpurple/account.h>
#include <libpurple/plugin.h>
#include <libpurple/prpl.h>

#include <QDialog>
#include <QList>
#include <QVariant>
#include <QWidget>

class QCheckBox;
class QComboBox;
class quail_protocol_box;
class QQuailTabWidget;
class QGridLayout;
class QLabel;
class QLineEdit;
class QPixmap;
class QPushButton;
class QTabWidget;
class QVBoxLayout;

class quail_account_editor : public QDialog
{
	Q_OBJECT

	public:
		quail_account_editor(PurpleAccount *account = NULL,
                            QWidget *parent = NULL,
                            QString name = "");
		~quail_account_editor();

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

        void slotAccept();
        void slotRejected();

    signals:
        void signalAccountUpdated();

	private:
		PurpleAccount *account;
		PurplePlugin *m_plugin;
		PurplePluginProtocolInfo *m_prpl_info;

		QString m_protocol_id;

		GList *userSplitEntries;
		GList *protocolOptEntries;

        PurpleProxyType newProxyType;

		QQuailTabWidget *tabs;

        QList<QWidget*> tabList;

		/* Account tab */
        QWidget *accountWidget;
        QVBoxLayout *accountBox;
		quail_protocol_box *protocolList;
		QLineEdit *screenNameEntry;
		QLabel *passwordLabel;
		QLineEdit *passwordEntry;
		QLineEdit *aliasEntry;
		QCheckBox *rememberPassCheck;
		QCheckBox *autoLoginCheck;
		QCheckBox *mailNotificationCheck;
		QPushButton *registerButton;

		/* Protocol tab */
        QWidget *protocolWidget;
        QVBoxLayout *protocolBox;

		/* Proxy tab */
        QWidget *proxyWidget;
        QVBoxLayout *proxyBox;
		QComboBox *proxyDropDown;
		QLineEdit *proxyHost;
		QLineEdit *proxyPort;
		QLineEdit *proxyUsername;
		QLineEdit *proxyPassword;
};

#endif /* _QUAIL_ACCOUNT_EDITOR_H_ */
