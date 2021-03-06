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
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPixmap>
#include <QPushButton>
#include <QTabWidget>
#include <QVBoxLayout>

quail_account_editor::quail_account_editor(PurpleAccount *account,
                                         QWidget *parent,
                                         QString name)
    : QDialog(parent)
    , m_account(account)
    , m_plugin(NULL)
    , m_prpl_info(NULL)
    , userSplitEntries(NULL)
    , newProxyType(PURPLE_PROXY_USE_GLOBAL)
{
    setWindowTitle(name);
    if (m_account == NULL)
	{
		if (purple_plugins_get_protocols() != NULL)
		{
            m_plugin = (PurplePlugin *)purple_plugins_get_protocols()->data;

            m_prpl_info = PURPLE_PLUGIN_PROTOCOL_INFO(m_plugin);

            m_protocol_id = m_plugin->info->id;
		}
	}
	else
	{
        m_protocol_id = purple_account_get_protocol_id(m_account);

        if ((m_plugin = purple_plugins_find_with_id(m_protocol_id.toLatin1())) != NULL)
            m_prpl_info = PURPLE_PLUGIN_PROTOCOL_INFO(m_plugin);
	}

	buildInterface();
}

quail_account_editor::~quail_account_editor()
{
	if (userSplitEntries != NULL)
		g_list_free(userSplitEntries);

    m_protocol_option_entries.clear();
}

void
quail_account_editor::buildInterface()
{
    if (m_account == NULL)
        setWindowTitle(tr("Add Account"));
	else
        setWindowTitle(tr("Edit Account"));

    QVBoxLayout *layout = new QVBoxLayout(this);
    tabs = new quail_tab_widget(this);
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
quail_account_editor::buildTabs()
{
	QWidget *widget;

	tabList.clear();

	widget = buildAccountTab();
	tabs->addTab(widget, tr("Account"));
    tabList.append(widget);

    if (m_plugin != NULL)
	{
		widget = buildProtocolTab();

		tabs->addTab(widget,
            quail_protocol_utils::getProtocolIcon(m_plugin),
            m_plugin->info->name);

		tabList.append(widget);
	}

	widget = buildProxyTab();
	tabs->addTab(widget, tr("Proxy"));
	tabList.append(widget);

	/* Ensure that the Protocol tab is only enabled if it contains stuff. */
    tabs->setTabEnabled(tabs->indexOf(protocolWidget), (m_protocol_option_entries.count() > 0));

    tabs->setCurrentWidget(accountWidget);
}

QWidget *
quail_account_editor::buildAccountTab()
{
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
    if (m_prpl_info != NULL)
	{
        if (m_prpl_info->options & OPT_PROTO_NO_PASSWORD)
		{
			passwordLabel->hide();
			passwordEntry->hide();
			rememberPassCheck->hide();
		}

        if (m_prpl_info->register_user == NULL)
			registerButton->hide();
	}

    return accountWidget;
}

QWidget *
quail_account_editor::buildProtocolTab()
{
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

    m_protocol_option_entries.clear();

    if (m_prpl_info != NULL)
	{
        for (l = m_prpl_info->protocol_options; l != NULL; l = l->next)
		{
			PurpleAccountOption *option = (PurpleAccountOption *)l->data;

			switch (purple_account_option_get_type(option))
			{
                case PURPLE_PREF_BOOLEAN:
                    if (m_account == NULL ||
                        m_protocol_id != purple_account_get_protocol_id(m_account))
					{
                        purple_account_option_get_default_bool(option);
					}
					else
                        purple_account_get_bool(m_account,
								purple_account_option_get_setting(option),
								purple_account_option_get_default_bool(option));

                    check = new QCheckBox(this);
                    check->setText(purple_account_option_get_text(option));
                    grid->addWidget(check, row, 0, 1, 2);
					row++;

                    m_protocol_option_entries.insert(option->pref_name, check);

					break;

                case PURPLE_PREF_INT:
                    if (m_account == NULL ||
                        m_protocol_id != purple_account_get_protocol_id(m_account))
					{
						intValue = purple_account_option_get_default_int(option);
					}
					else
					{
                        intValue = purple_account_get_int(m_account,
								purple_account_option_get_setting(option),
								purple_account_option_get_default_int(option));
					}

					buf  = purple_account_option_get_text(option);
					buf += ":";

					grid->addWidget(new QLabel(buf, frame), row, 0);

					entry = new QLineEdit(QString::number(intValue), frame);
					grid->addWidget(entry, row++, 1);

                    m_protocol_option_entries.insert(option->pref_name, entry);

					break;

                case PURPLE_PREF_STRING:
                    if (m_account == NULL ||
                        m_protocol_id != purple_account_get_protocol_id(m_account))
					{
						strValue =
							purple_account_option_get_default_string(option);
					}
					else
					{
                        strValue = purple_account_get_string(m_account,
								purple_account_option_get_setting(option),
								purple_account_option_get_default_string(option));
					}

					buf  = purple_account_option_get_text(option);
					buf += ":";

					grid->addWidget(new QLabel(buf, frame), row, 0);

					entry = new QLineEdit(strValue, frame);
					grid->addWidget(entry, row++, 1);

                    m_protocol_option_entries.insert(option->pref_name, entry);

					break;

				default:
					break;
			}
		}
	}

    return protocolWidget;
}

QWidget *
quail_account_editor::buildProxyTab()
{
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

	/* Proxy Type */
	grid->addWidget(new QLabel(tr("Proxy Type:"), frame), row, 0);
    proxyDropDown = new QComboBox(frame);
	grid->addWidget(proxyDropDown, row++, 1);
	row++;
	/* Connect the signal */
	connect(proxyDropDown, SIGNAL(activated(int)),
			this, SLOT(proxyTypeChanged(int)));

	/* Fill it. */
    proxyDropDown->addItem(tr("Use Global Proxy Settings"));
    proxyDropDown->addItem(tr("No Proxy"));
    proxyDropDown->addItem(tr("HTTP"));
    proxyDropDown->addItem(tr("SOCKS 4"));
    proxyDropDown->addItem(tr("SOCKS 5"));

	/* Host */
	grid->addWidget(new QLabel(tr("Host:"), frame), row, 0);
	proxyHost = new QLineEdit(frame);
	grid->addWidget(proxyHost, row++, 1);
	/* Port */
	grid->addWidget(new QLabel(tr("Port:"), frame), row, 0);
	proxyPort = new QLineEdit(frame);
	grid->addWidget(proxyPort, row++, 1);
	/* Username */
	grid->addWidget(new QLabel(tr("Username:"), frame), row, 0);
	proxyUsername = new QLineEdit(frame);
	grid->addWidget(proxyUsername, row++, 1);
	/* Password */
	grid->addWidget(new QLabel(tr("Password:"), frame), row, 0);
	proxyPassword = new QLineEdit(frame);
	grid->addWidget(proxyPassword, row++, 1);
	/* Set the values for everything. */
    if (m_account != NULL &&
        (proxyInfo = purple_account_get_proxy_info(m_account)) != NULL)
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
        proxyDropDown->setCurrentIndex((int)newProxyType + 1);
		proxyHost->setReadOnly(true);
		proxyPort->setReadOnly(true);
		proxyUsername->setReadOnly(true);
		proxyPassword->setReadOnly(true);
	}
    return proxyWidget;
}

void
quail_account_editor::buildLoginOpts(QGridLayout *grid, QWidget *parent,
								   int &row)
{
	QLineEdit *entry;
	QString username;
	GList *userSplits = NULL, *l, *l2;

	/* Protocol */
	grid->addWidget(new QLabel(tr("Protocol:"), parent), row, 0);
    protocolList = new quail_protocol_box(parent);
    protocolList->setCurrentProtocol(m_protocol_id);

	grid->addWidget(protocolList, row++, 1);

	/* Connect the signal */
	connect(protocolList, SIGNAL(activated(int)),
			this, SLOT(protocolChanged(int)));

	/* Screen Name */
	grid->addWidget(new QLabel(tr("Screenname:"), parent), row, 0);
	screenNameEntry = new QLineEdit(parent);
	grid->addWidget(screenNameEntry, row++, 1);

	/* User split stuff. */
    if (m_prpl_info != NULL)
        userSplits = m_prpl_info->user_splits;

    if (m_account != NULL)
        username = purple_account_get_username(m_account);

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

        if (m_account != NULL)
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

    if (m_account != NULL)
        passwordEntry->setText(purple_account_get_password(m_account));

	/* Alias */
	grid->addWidget(new QLabel(tr("Alias:"), parent), row, 0);
	aliasEntry = new QLineEdit(parent);
	grid->addWidget(aliasEntry, row++, 1);

    if (m_account != NULL)
        aliasEntry->setText(purple_account_get_alias(m_account));

	/* Remember Password */
	rememberPassCheck = new QCheckBox(tr("Remember Password"), parent);
    grid->addWidget(rememberPassCheck, row, 0, 1, 2);
	row++;

    if (m_account != NULL)
		rememberPassCheck->setChecked(
                purple_account_get_remember_password(m_account));
	else
		rememberPassCheck->setChecked(true);

	/* Auto-Login */
	autoLoginCheck = new QCheckBox(tr("Auto-Login"), parent);
    grid->addWidget(autoLoginCheck, row, 0, 1, 2);
	row++;

    if (m_account != NULL)
		autoLoginCheck->setChecked(
                purple_account_get_enabled(m_account, UI_ID));

	/*
	 * We want to hide a couple of things if the protocol doesn't want
	 * a password.
	 */
    if (m_prpl_info != NULL &&
        (m_prpl_info->options & OPT_PROTO_NO_PASSWORD))
	{
		passwordLabel->hide();
		passwordEntry->hide();
		rememberPassCheck->hide();
	}
}

void
quail_account_editor::buildUserOpts(QGridLayout *grid, QWidget *parent,
								  int &row)
{
	/* New mail notifications */
	mailNotificationCheck = new QCheckBox(tr("New Mail Notifications"), parent);
    grid->addWidget(mailNotificationCheck, row, 0, 1, 2);
	row++;

	/* TODO: Buddy Icon support */

    if (m_account != NULL)
		mailNotificationCheck->setChecked(
                purple_account_get_check_mail(m_account));
    if (m_prpl_info != NULL && !(m_prpl_info->options & OPT_PROTO_MAIL_CHECK))
		mailNotificationCheck->hide();
}

void
quail_account_editor::proxyTypeChanged(int index)
{
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
quail_account_editor::protocolChanged(int index)
{

//	GList *l;
    QString protocolId = protocolList->itemData(index).toString();
    purple_debug(PURPLE_DEBUG_MISC, "QQuailAccountEditor::protocolChanged", protocolId.toLatin1());
//    l = g_list_nth(purple_plugins_get_protocols(), protocolCount.toInt());

//	if (l == NULL)
//	{
//        purple_debug(PURPLE_DEBUG_FATAL, "QQuailAccountEditor",
//				   "Protocol switched to is not in list!\n");
//		abort();
//	}

    //plugin = (PurplePlugin *)l->data;
    if (protocolId == "google-talk"
            || protocolId == "facebook")
        m_plugin = purple_plugins_find_with_id("prpl-jabber");
    else
        m_plugin = purple_plugins_find_with_id(protocolId.toLatin1());

    if (m_plugin == NULL)
    {
        purple_debug(PURPLE_DEBUG_FATAL, "QQuailAccountEditor",
                   "Protocol switched to is not in list!\n");
        abort();
    }

    m_prpl_info   = PURPLE_PLUGIN_PROTOCOL_INFO(m_plugin);
    m_protocol_id = protocolId;

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
quail_account_editor::registerClicked()
{
    purple_account_register(m_account);

	registerButton->setEnabled(false);
}


void
quail_account_editor::slotRejected()
{
    close();
}

void
quail_account_editor::slotAccept()
{
    QString str, username, protocolId;
	GList *l, *l2;
    bool newAccount = (m_account == NULL);

    if (m_protocol_id == "google-talk"
            || m_protocol_id == "facebook")
        protocolId = "prpl-jabber";
    else
        protocolId = m_protocol_id;

    if (m_account == NULL)
	{
		/* New Account */
		username = screenNameEntry->text();

        m_account = purple_account_new(username.toStdString().c_str(),
                                     protocolId.toStdString().c_str());
	}
	else
	{
		/* Protocol */
        purple_account_set_protocol_id(m_account, protocolId.toStdString().c_str());
	}
	/* Clear the existing settings. */
    purple_account_clear_settings(m_account);
	/* Alias */
	str = aliasEntry->text();

	if (!str.isEmpty())
        purple_account_set_alias(m_account, str.toStdString().c_str());
	else
        purple_account_set_alias(m_account, NULL);
	/* Buddy Icon */
	/* TODO */

	/* Remember Password */
    purple_account_set_remember_password(m_account,
									   rememberPassCheck->isChecked());
	/* Check Mail */
    if (m_prpl_info->options & OPT_PROTO_MAIL_CHECK)
        purple_account_set_check_mail(m_account,
									mailNotificationCheck->isChecked());
	/* Auto-Login */
    purple_account_set_enabled(m_account, UI_ID,
								autoLoginCheck->isChecked());
	/* Password */
	str = passwordEntry->text();

	if (!str.isEmpty())
        purple_account_set_password(m_account, str.toStdString().c_str());
	else
        purple_account_set_password(m_account, NULL);

	/* Build the username string. */
	username = screenNameEntry->text();
    for (l = m_prpl_info->user_splits, l2 = userSplitEntries;
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
    purple_account_set_username(m_account, username.toStdString().c_str());
	/* Add the protocol settings */
    for (l = m_prpl_info->protocol_options;
         l != NULL;
         l = l->next)
	{
        PurplePrefType type;
		PurpleAccountOption *option = (PurpleAccountOption *)l->data;
        QWidget *widget = m_protocol_option_entries.value(option->pref_name);
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
                    purple_debug(PURPLE_DEBUG_WARNING
                                 , "quail_account_editor::slotAccept"
                                 , "MISSING WIDGET");
                } else {
                    value = entry->text();
                    purple_account_set_string(m_account,
                                              setting,
                                              value.toStdString().c_str());
                }
				break;

            case PURPLE_PREF_INT:
                bool ok;

				entry = (QLineEdit *)widget;
				intValue = entry->text().toInt(&ok);

				if (ok)
                    purple_account_set_int(m_account, setting, intValue);
				else
                    purple_account_set_int(m_account, setting, 0);

				break;

            case PURPLE_PREF_BOOLEAN:
				checkbox = (QCheckBox *)widget;
                purple_account_set_bool(m_account, setting,
									  checkbox->isChecked());
				break;

			default:
				break;
		}
	}
	/* Set the proxy information */
    if (newProxyType == PURPLE_PROXY_NONE)
        purple_account_set_proxy_info(m_account, NULL);
	else
	{
        PurpleProxyInfo *proxyInfo = purple_account_get_proxy_info(m_account);

		/* Create the proxy info if it doesn't exist. */
		if (proxyInfo == NULL)
		{
			proxyInfo = purple_proxy_info_new();
            purple_account_set_proxy_info(m_account, proxyInfo);
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
            bool ok;
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
        purple_accounts_add(m_account);
	QDialog::accept();

    emit signalAccountUpdated();
}
