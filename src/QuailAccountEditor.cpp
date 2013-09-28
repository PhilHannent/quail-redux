﻿/**
 * @file QQuailAccountEditor.cpp Account editor dialog
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
#include "QuailAccountEditor.h"
#include "QuailProtocolBox.h"
#include "QuailProtocolUtils.h"
#include "QuailTabWidget.h"

#include <libpurple/accountopt.h>
#include <libpurple/debug.h>

#include <QDialogButtonBox>
#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPixmap>
#include <QPushButton>
#include <QTabWidget>
#include <QVBoxLayout>

QQuailAccountEditor::QQuailAccountEditor(PurpleAccount *account,
                                         QWidget *parent,
                                         QString name)
    : QDialog(parent), account(account), plugin(NULL),
      prplInfo(NULL), userSplitEntries(NULL),
      protocolOptEntries(NULL), newProxyType(PURPLE_PROXY_USE_GLOBAL)
{
    qDebug() << "QQuailAccountEditor::QQuailAccountEditor";
    setWindowTitle(name);
	if (account == NULL)
	{
        qDebug() << "QQuailAccountEditor::QQuailAccountEditor.1";
		if (purple_plugins_get_protocols() != NULL)
		{
            qDebug() << "QQuailAccountEditor::QQuailAccountEditor.1a";
			plugin = (PurplePlugin *)purple_plugins_get_protocols()->data;

            prplInfo = PURPLE_PLUGIN_PROTOCOL_INFO(plugin);

			protocolId = plugin->info->id;
		}
	}
	else
	{
        qDebug() << "QQuailAccountEditor::QQuailAccountEditor.2";
		protocolId = purple_account_get_protocol_id(account);

        if ((plugin = purple_plugins_find_with_id(protocolId.toLatin1())) != NULL)
            prplInfo = PURPLE_PLUGIN_PROTOCOL_INFO(plugin);
	}

	buildInterface();
}

QQuailAccountEditor::~QQuailAccountEditor()
{
	if (userSplitEntries != NULL)
		g_list_free(userSplitEntries);

	if (protocolOptEntries != NULL)
		g_list_free(protocolOptEntries);
}

void
QQuailAccountEditor::buildInterface()
{
    qDebug() << "QQuailAccountEditor::buildInterface";
	if (account == NULL)
        setWindowTitle(tr("Add Account"));
	else
        setWindowTitle(tr("Edit Account"));

    QVBoxLayout *layout = new QVBoxLayout(this);
	tabs = new QQuailTabWidget(this);
    layout->addWidget(tabs);
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel,
                                                       Qt::Horizontal,
                                                       this);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(slotAccept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(slotRejected()));
    layout->addWidget(buttonBox);

	buildTabs();
}

void
QQuailAccountEditor::buildTabs()
{
    qDebug() << "QQuailAccountEditor::buildTabs";
	QWidget *widget;

	tabList.clear();

	widget = buildAccountTab();
	tabs->addTab(widget, tr("Account"));
    tabList.append(widget);

	if (plugin != NULL)
	{
		widget = buildProtocolTab();

		tabs->addTab(widget,
            QQuailProtocolUtils::getProtocolIcon(plugin),
			plugin->info->name);

		tabList.append(widget);
	}

	widget = buildProxyTab();
	tabs->addTab(widget, tr("Proxy"));
	tabList.append(widget);

	/* Ensure that the Protocol tab is only enabled if it contains stuff. */
    tabs->setTabEnabled(tabs->indexOf(protocolWidget), (protocolOptEntries != NULL));

    tabs->setCurrentWidget(accountWidget);
}

QWidget *
QQuailAccountEditor::buildAccountTab()
{
    qDebug() << "QQuailAccountEditor::buildAccountTab";
	/* QFrame *sep; */
    QVBoxLayout *vbox;
    QHBoxLayout *hbox;
	QGridLayout *grid;
	QWidget *frame;
	int row = 0;
    accountWidget = new QWidget(this);
	/* Create the main vbox. */
    accountBox = vbox = new QVBoxLayout(accountWidget);
    accountWidget->setLayout(accountBox);
    vbox->setSpacing(5);

    frame = new QFrame(accountWidget);
    grid = new QGridLayout();
    grid->addWidget(frame);
	grid->setSpacing(5);
    vbox->addLayout(grid);

	buildLoginOpts(grid, frame, row);

#if 0
	/* Separator */
	sep = new QFrame(frame);
	sep->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    grid->addWidget(sep, row, row, 0, 2);
	row++;
#endif

	buildUserOpts(grid, frame, row);

	/* Add the hbox */
    hbox = new QHBoxLayout();
    hbox->addLayout(vbox);
	hbox->setSpacing(5);

	/* Add the register button. */
    registerButton = new QPushButton(tr("Register Account"));
    hbox->addWidget(registerButton);
	connect(registerButton, SIGNAL(clicked()),
			this, SLOT(registerClicked()));

	/*
	 * We want to hide a couple of things if the protocol doesn't want
	 * a password.
	 */
	if (prplInfo != NULL)
	{
		if (prplInfo->options & OPT_PROTO_NO_PASSWORD)
		{
			passwordLabel->hide();
			passwordEntry->hide();
			rememberPassCheck->hide();
		}

		if (prplInfo->register_user == NULL)
			registerButton->hide();
	}

    return accountWidget;
}

QWidget *
QQuailAccountEditor::buildProtocolTab()
{
    qDebug() << "QQuailAccountEditor::buildProtocolTab";
    QVBoxLayout *vbox;
	QFrame *frame;
	QGridLayout *grid;
	QCheckBox *check;
	QLineEdit *entry;
	GList *l;
	QString buf;
	int row = 0;
	QString strValue;
	int intValue;

    protocolWidget = new QWidget(this);
	/* Create the main vbox */
    protocolBox = vbox = new QVBoxLayout();
    protocolWidget->setLayout(protocolBox);
	vbox->setSpacing(5);

    frame = new QFrame(this);
    vbox->addWidget(frame);
    grid = new QGridLayout(frame);
	grid->setSpacing(5);

	if (protocolOptEntries != NULL)
	{
		g_list_free(protocolOptEntries);
		protocolOptEntries = NULL;
	}

	if (prplInfo != NULL)
	{
		for (l = prplInfo->protocol_options; l != NULL; l = l->next)
		{
			PurpleAccountOption *option = (PurpleAccountOption *)l->data;

			switch (purple_account_option_get_type(option))
			{
                case PURPLE_PREF_BOOLEAN:
					if (account == NULL ||
						protocolId != purple_account_get_protocol_id(account))
					{
                        purple_account_option_get_default_bool(option);
					}
					else
                        purple_account_get_bool(account,
								purple_account_option_get_setting(option),
								purple_account_option_get_default_bool(option));

                    check = new QCheckBox(this);
                    check->setText(purple_account_option_get_text(option));
                    grid->addWidget(check, row, 0, 1, 2);
					row++;

					protocolOptEntries =
						g_list_append(protocolOptEntries, check);

					break;

                case PURPLE_PREF_INT:
					if (account == NULL ||
						protocolId != purple_account_get_protocol_id(account))
					{
						intValue = purple_account_option_get_default_int(option);
					}
					else
					{
						intValue = purple_account_get_int(account,
								purple_account_option_get_setting(option),
								purple_account_option_get_default_int(option));
					}

					buf  = purple_account_option_get_text(option);
					buf += ":";

					grid->addWidget(new QLabel(buf, frame), row, 0);

					entry = new QLineEdit(QString::number(intValue), frame);
					grid->addWidget(entry, row++, 1);

					protocolOptEntries =
						g_list_append(protocolOptEntries, entry);

					break;

                case PURPLE_PREF_STRING:
					if (account == NULL ||
						protocolId != purple_account_get_protocol_id(account))
					{
						strValue =
							purple_account_option_get_default_string(option);
					}
					else
					{
						strValue = purple_account_get_string(account,
								purple_account_option_get_setting(option),
								purple_account_option_get_default_string(option));
					}

					buf  = purple_account_option_get_text(option);
					buf += ":";

					grid->addWidget(new QLabel(buf, frame), row, 0);

					entry = new QLineEdit(strValue, frame);
					grid->addWidget(entry, row++, 1);

					protocolOptEntries =
						g_list_append(protocolOptEntries, entry);

					break;

				default:
					break;
			}
		}
	}

    return protocolWidget;
}

QWidget *
QQuailAccountEditor::buildProxyTab()
{
    qDebug() << "QQuailAccountEditor::buildProxyTab";
    QVBoxLayout *vbox;
	QGridLayout *grid;
	QWidget *frame;
    PurpleProxyInfo *proxyInfo;
	int row = 0;
    proxyWidget = new QWidget(this);
	/* Create the main vbox */
    proxyBox = vbox = new QVBoxLayout();
    proxyWidget->setLayout(proxyBox);
    vbox->setSpacing(5);
    frame = new QFrame(this);
    vbox->addWidget(frame);
    grid = new QGridLayout(frame);
	grid->setSpacing(5);
    qDebug() << "QQuailAccountEditor::buildProxyTab.2";

	/* Proxy Type */
	grid->addWidget(new QLabel(tr("Proxy Type:"), frame), row, 0);
    proxyDropDown = new QComboBox(frame);
	grid->addWidget(proxyDropDown, row++, 1);
	row++;
    qDebug() << "QQuailAccountEditor::buildProxyTab.3";
	/* Connect the signal */
	connect(proxyDropDown, SIGNAL(activated(int)),
			this, SLOT(proxyTypeChanged(int)));

	/* Fill it. */
    proxyDropDown->addItem(tr("Use Global Proxy Settings"));
    proxyDropDown->addItem(tr("No Proxy"));
    proxyDropDown->addItem(tr("HTTP"));
    proxyDropDown->addItem(tr("SOCKS 4"));
    proxyDropDown->addItem(tr("SOCKS 5"));
    qDebug() << "QQuailAccountEditor::buildProxyTab.4";
	/* Host */
	grid->addWidget(new QLabel(tr("Host:"), frame), row, 0);
	proxyHost = new QLineEdit(frame);
	grid->addWidget(proxyHost, row++, 1);
    qDebug() << "QQuailAccountEditor::buildProxyTab.5";
	/* Port */
	grid->addWidget(new QLabel(tr("Port:"), frame), row, 0);
	proxyPort = new QLineEdit(frame);
	grid->addWidget(proxyPort, row++, 1);
    qDebug() << "QQuailAccountEditor::buildProxyTab.6";
	/* Username */
	grid->addWidget(new QLabel(tr("Username:"), frame), row, 0);
	proxyUsername = new QLineEdit(frame);
	grid->addWidget(proxyUsername, row++, 1);
    qDebug() << "QQuailAccountEditor::buildProxyTab.7";
	/* Password */
	grid->addWidget(new QLabel(tr("Password:"), frame), row, 0);
	proxyPassword = new QLineEdit(frame);
	grid->addWidget(proxyPassword, row++, 1);
    qDebug() << "QQuailAccountEditor::buildProxyTab.8";
	/* Set the values for everything. */
	if (account != NULL &&
		(proxyInfo = purple_account_get_proxy_info(account)) != NULL)
	{
        PurpleProxyType type = purple_proxy_info_get_type(proxyInfo);

        proxyDropDown->setCurrentIndex((int)type + 1);

        if (type == PURPLE_PROXY_NONE || type == PURPLE_PROXY_USE_GLOBAL)
		{
			proxyHost->setReadOnly(true);
			proxyPort->setReadOnly(true);
			proxyUsername->setReadOnly(true);
			proxyPassword->setReadOnly(true);
		}
		else
		{
			proxyHost->setText(purple_proxy_info_get_host(proxyInfo));
			proxyPort->setText(
				QString::number(purple_proxy_info_get_port(proxyInfo)));
			proxyUsername->setText(purple_proxy_info_get_username(proxyInfo));
			proxyPassword->setText(purple_proxy_info_get_password(proxyInfo));
		}
	}
	else
	{
		proxyHost->setReadOnly(true);
		proxyPort->setReadOnly(true);
		proxyUsername->setReadOnly(true);
		proxyPassword->setReadOnly(true);
	}
    qDebug() << "QQuailAccountEditor::buildProxyTab.end";
    return proxyWidget;
}

void
QQuailAccountEditor::buildLoginOpts(QGridLayout *grid, QWidget *parent,
								   int &row)
{
    qDebug() << "QQuailAccountEditor::buildLoginOpts";
	QLineEdit *entry;
	QString username;
	GList *userSplits = NULL, *l, *l2;

	/* Protocol */
	grid->addWidget(new QLabel(tr("Protocol:"), parent), row, 0);
    protocolList = new QQuailProtocolBox(parent);
	protocolList->setCurrentProtocol(protocolId);

	grid->addWidget(protocolList, row++, 1);

	/* Connect the signal */
	connect(protocolList, SIGNAL(activated(int)),
			this, SLOT(protocolChanged(int)));

	/* Screen Name */
	grid->addWidget(new QLabel(tr("Screenname:"), parent), row, 0);
	screenNameEntry = new QLineEdit(parent);
	grid->addWidget(screenNameEntry, row++, 1);

	/* User split stuff. */
	if (prplInfo != NULL)
		userSplits = prplInfo->user_splits;

	if (account != NULL)
		username = purple_account_get_username(account);

	if (userSplitEntries != NULL)
	{
		g_list_free(userSplitEntries);
		userSplitEntries = NULL;
	}

	for (l = userSplits; l != NULL; l = l->next)
	{
		PurpleAccountUserSplit *split = (PurpleAccountUserSplit *)l->data;
		QString buf;

		buf  = purple_account_user_split_get_text(split);
		buf += ":";

		grid->addWidget(new QLabel(buf, parent), row, 0);
		entry = new QLineEdit(parent);
		grid->addWidget(entry, row++, 1);

		userSplitEntries = g_list_append(userSplitEntries, entry);
	}

	for (l  = g_list_last(userSplitEntries),
		 l2 = g_list_last(userSplits);
		 l != NULL && l2 != NULL;
		 l = l->prev, l2 = l2->prev)
	{
		PurpleAccountUserSplit *split;
		QString value;

		entry = (QLineEdit *)l->data;
		split = (PurpleAccountUserSplit *)l2->data;

		if (account != NULL)
		{
			int i;

            i = username.indexOf(purple_account_user_split_get_separator(split));

			if (i != -1)
			{
				value    = username.mid(i + 1);
				username = username.left(i);
			}
		}

		if (value.isEmpty())
			value = purple_account_user_split_get_default_value(split);

		if (!value.isEmpty())
			entry->setText(value);
	}

	if (!username.isEmpty())
		screenNameEntry->setText(username);


	/* Password */
	passwordLabel = new QLabel(tr("Password:"), parent);
	grid->addWidget(passwordLabel, row, 0);
	passwordEntry = new QLineEdit(parent);
	passwordEntry->setEchoMode(QLineEdit::Password);
	grid->addWidget(passwordEntry, row++, 1);

	if (account != NULL)
		passwordEntry->setText(purple_account_get_password(account));

	/* Alias */
	grid->addWidget(new QLabel(tr("Alias:"), parent), row, 0);
	aliasEntry = new QLineEdit(parent);
	grid->addWidget(aliasEntry, row++, 1);

	if (account != NULL)
		aliasEntry->setText(purple_account_get_alias(account));

	/* Remember Password */
	rememberPassCheck = new QCheckBox(tr("Remember Password"), parent);
    grid->addWidget(rememberPassCheck, row, 0, 1, 2);
	row++;

	if (account != NULL)
		rememberPassCheck->setChecked(
				purple_account_get_remember_password(account));
	else
		rememberPassCheck->setChecked(true);

	/* Auto-Login */
	autoLoginCheck = new QCheckBox(tr("Auto-Login"), parent);
    grid->addWidget(autoLoginCheck, row, 0, 1, 2);
	row++;

	if (account != NULL)
		autoLoginCheck->setChecked(
                purple_account_get_enabled(account, UI_ID));

	/*
	 * We want to hide a couple of things if the protocol doesn't want
	 * a password.
	 */
	if (prplInfo != NULL &&
		(prplInfo->options & OPT_PROTO_NO_PASSWORD))
	{
		passwordLabel->hide();
		passwordEntry->hide();
		rememberPassCheck->hide();
	}
}

void
QQuailAccountEditor::buildUserOpts(QGridLayout *grid, QWidget *parent,
								  int &row)
{
    qDebug() << "QQuailAccountEditor::buildUserOpts";
	/* New mail notifications */
	mailNotificationCheck = new QCheckBox(tr("New Mail Notifications"), parent);
    qDebug() << "QQuailAccountEditor::buildUserOpts.1";
    grid->addWidget(mailNotificationCheck, row, 0, 1, 2);
    qDebug() << "QQuailAccountEditor::buildUserOpts.2";
	row++;

	/* TODO: Buddy Icon support */

	if (account != NULL)
		mailNotificationCheck->setChecked(
				purple_account_get_check_mail(account));
    qDebug() << "QQuailAccountEditor::buildUserOpts.3";
	if (prplInfo != NULL && !(prplInfo->options & OPT_PROTO_MAIL_CHECK))
		mailNotificationCheck->hide();
    qDebug() << "QQuailAccountEditor::buildUserOpts.end";
}

void
QQuailAccountEditor::proxyTypeChanged(int index)
{
    qDebug() << "QQuailAccountEditor::proxyTypeChanged";
    newProxyType = (PurpleProxyType)(index - 1);

    if (newProxyType == PURPLE_PROXY_USE_GLOBAL ||
        newProxyType == PURPLE_PROXY_NONE)
	{
		proxyHost->setReadOnly(true);
		proxyPort->setReadOnly(true);
		proxyUsername->setReadOnly(true);
		proxyPassword->setReadOnly(true);
	}
	else
	{
		proxyHost->setReadOnly(false);
		proxyPort->setReadOnly(false);
		proxyUsername->setReadOnly(false);
		proxyPassword->setReadOnly(false);
	}
}

void
QQuailAccountEditor::protocolChanged(int index)
{
    qDebug() << "QQuailAccountEditor::protocolChanged";
	GList *l;

	l = g_list_nth(purple_plugins_get_protocols(), index);

	if (l == NULL)
	{
        purple_debug(PURPLE_DEBUG_FATAL, "QQuailAccountEditor",
				   "Protocol switched to is not in list!\n");
		abort();
	}

	plugin = (PurplePlugin *)l->data;

    prplInfo   = PURPLE_PLUGIN_PROTOCOL_INFO(plugin);
	protocolId = plugin->info->id;

    tabs->removeTab(tabs->indexOf(accountWidget));
    tabs->removeTab(tabs->indexOf(protocolWidget));
    tabs->removeTab(tabs->indexOf(proxyWidget));

#if 0
	delete accountBox;
	delete protocolBox;
	delete proxyBox;
#endif

	buildTabs();
}

void
QQuailAccountEditor::registerClicked()
{
    qDebug() << "QQuailAccountEditor::registerClicked";
	purple_account_register(account);

	registerButton->setEnabled(false);
}


void
QQuailAccountEditor::slotRejected()
{
    close();
}

void
QQuailAccountEditor::slotAccept()
{
    qDebug() << "QQuailAccountEditor::slotAccept";
	QString str, username;
	GList *l, *l2;
	bool newAccount = (account == NULL);

	if (account == NULL)
	{
		/* New Account */
		username = screenNameEntry->text();

        account = purple_account_new(username.toStdString().c_str(),
                                     protocolId.toStdString().c_str());
	}
	else
	{
		/* Protocol */
        purple_account_set_protocol_id(account, protocolId.toStdString().c_str());
	}
	/* Clear the existing settings. */
	purple_account_clear_settings(account);
	/* Alias */
	str = aliasEntry->text();

	if (!str.isEmpty())
        purple_account_set_alias(account, str.toStdString().c_str());
	else
		purple_account_set_alias(account, NULL);
	/* Buddy Icon */
	/* TODO */

	/* Remember Password */
	purple_account_set_remember_password(account,
									   rememberPassCheck->isChecked());
	/* Check Mail */
	if (prplInfo->options & OPT_PROTO_MAIL_CHECK)
		purple_account_set_check_mail(account,
									mailNotificationCheck->isChecked());
	/* Auto-Login */
    purple_account_set_enabled(account, UI_ID,
								autoLoginCheck->isChecked());
	/* Password */
	str = passwordEntry->text();

	if (!str.isEmpty())
        purple_account_set_password(account, str.toStdString().c_str());
	else
		purple_account_set_password(account, NULL);

	/* Build the username string. */
	username = screenNameEntry->text();
	for (l = prplInfo->user_splits, l2 = userSplitEntries;
		 l != NULL && l2 != NULL;
		 l = l->next, l2 = l2->next)
	{
		PurpleAccountUserSplit *split = (PurpleAccountUserSplit *)l->data;
		QLineEdit *entry = (QLineEdit *)l2->data;
		char sep[2] = " ";
		char *tmp;

		str = entry->text();

		*sep = purple_account_user_split_get_separator(split);

        tmp = g_strconcat(username.toStdString().c_str(), sep,
                          (!str.isEmpty() ? str.toStdString().c_str() :
						   purple_account_user_split_get_default_value(split)),
						  NULL);

		username = tmp;

		g_free(tmp);
	}
    purple_account_set_username(account, username.toStdString().c_str());
	/* Add the protocol settings */
	for (l = prplInfo->protocol_options, l2 = protocolOptEntries;
		 l != NULL && l2 != NULL;
		 l = l->next, l2 = l2->next)
	{
        PurplePrefType type;
		PurpleAccountOption *option = (PurpleAccountOption *)l->data;
		QWidget *widget = (QWidget *)l2->data;
		QLineEdit *entry;
		QCheckBox *checkbox;
		const char *setting;
		QString value;
		int intValue;

		type    = purple_account_option_get_type(option);
		setting = purple_account_option_get_setting(option);
		switch (type)
		{
            case PURPLE_PREF_STRING:
                entry = dynamic_cast<QLineEdit *>(widget);
                if (!entry || entry == NULL) {
                    qWarning() << "QQuailAccountEditor::slotAccept.MISSING Widget";
                } else {
                    value = entry->text();
                    purple_account_set_string(account,
                                              setting,
                                              value.toStdString().c_str());
                }
				break;

            case PURPLE_PREF_INT:
                bool ok;

				entry = (QLineEdit *)widget;
				intValue = entry->text().toInt(&ok);

				if (ok)
					purple_account_set_int(account, setting, intValue);
				else
					purple_account_set_int(account, setting, 0);

				break;

            case PURPLE_PREF_BOOLEAN:
				checkbox = (QCheckBox *)widget;
				purple_account_set_bool(account, setting,
									  checkbox->isChecked());
				break;

			default:
				break;
		}
	}
	/* Set the proxy information */
    if (newProxyType == PURPLE_PROXY_NONE)
		purple_account_set_proxy_info(account, NULL);
	else
	{
        PurpleProxyInfo *proxyInfo = purple_account_get_proxy_info(account);

		/* Create the proxy info if it doesn't exist. */
		if (proxyInfo == NULL)
		{
			proxyInfo = purple_proxy_info_new();
			purple_account_set_proxy_info(account, proxyInfo);
		}

		/* Type */
		purple_proxy_info_set_type(proxyInfo, newProxyType);

		/* Host */
		str = proxyHost->text();

		if (!str.isEmpty())
            purple_proxy_info_set_host(proxyInfo, str.toStdString().c_str());
		else
			purple_proxy_info_set_host(proxyInfo, NULL);

		/* Port */
		str = proxyPort->text();

		if (!str.isEmpty())
		{
            bool ok = false;
            int intVal = str.toInt(&ok);

			if (ok)
				purple_proxy_info_set_port(proxyInfo, intVal);
			else
				purple_proxy_info_set_port(proxyInfo, 0);
		}
		else
			purple_proxy_info_set_port(proxyInfo, 0);

		/* Username */
		str = proxyUsername->text();

		if (!str.isEmpty())
            purple_proxy_info_set_username(proxyInfo, str.toStdString().c_str());
		else
			purple_proxy_info_set_username(proxyInfo, NULL);

		/* Password */
		str = proxyPassword->text();

		if (!str.isEmpty())
            purple_proxy_info_set_password(proxyInfo, str.toStdString().c_str());
		else
			purple_proxy_info_set_password(proxyInfo, NULL);
	}
	if (newAccount)
		purple_accounts_add(account);
	QDialog::accept();

    emit signalAccountUpdated();
}
