#include "QGaimAccountEditor.h"
#include "QGaimAccountsWindow.h"
#include "QGaimProtocolBox.h"
#include "QGaimProtocolUtils.h"
#include "QGaimTabWidget.h"
#include "base.h"

#include <libgaim/accountopt.h>
#include <libgaim/debug.h>

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpixmap.h>
#include <qtabwidget.h>
#include <qvbox.h>

QGaimAccountEditor::QGaimAccountEditor(GaimAccount *account, QWidget *parent,
									   const char *name, WFlags fl)
	: QDialog(parent, name, fl), account(account), plugin(NULL),
	  prplInfo(NULL), accountsWin(NULL), userSplitEntries(NULL),
	  protocolOptEntries(NULL), newProxyType(GAIM_PROXY_USE_GLOBAL)
{
	if (account == NULL)
	{
		protocolId = "prpl-oscar";
		protocol   = GAIM_PROTO_OSCAR;
	}
	else
	{
		protocolId = gaim_account_get_protocol_id(account);
		protocol   = gaim_account_get_protocol(account);
	}

	if ((plugin = gaim_find_prpl(protocol)) != NULL)
		prplInfo = GAIM_PLUGIN_PROTOCOL_INFO(plugin);

	buildInterface();
}

QGaimAccountEditor::~QGaimAccountEditor()
{
	if (userSplitEntries != NULL)
		g_list_free(userSplitEntries);

	if (protocolOptEntries != NULL)
		g_list_free(protocolOptEntries);
}

void
QGaimAccountEditor::setAccountsWindow(QGaimAccountsWindow *accountsWin)
{
	this->accountsWin = accountsWin;
}

void
QGaimAccountEditor::buildInterface()
{
	if (account == NULL)
		setCaption(tr("Add Account"));
	else
		setCaption(tr("Edit Account"));

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setAutoAdd(true);

	tabs = new QGaimTabWidget(this);
	tabs->setMargin(6);

	buildTabs();
}

void
QGaimAccountEditor::buildTabs()
{
	QWidget *widget;

	tabList.clear();

	widget = buildAccountTab();
	tabs->addTab(widget, tr("Account"));
	tabList.append(widget);

	if (plugin != NULL)
	{
		QPixmap *pixmap = QGaimProtocolUtils::getProtocolIcon(plugin);

		widget = buildProtocolTab();

		if (pixmap == NULL)
			tabs->addTab(widget, plugin->info->name);
		else
		{
			tabs->addTab(widget, QIconSet(*pixmap), plugin->info->name);

			delete pixmap;
		}

		tabList.append(widget);
	}

	widget = buildProxyTab();
	tabs->addTab(widget, tr("Proxy"));
	tabList.append(widget);

	/* Ensure that the Protocol tab is only enabled if it contains stuff. */
	tabs->setTabEnabled(protocolBox, (protocolOptEntries != NULL));

	tabs->showPage(accountBox);
}

QWidget *
QGaimAccountEditor::buildAccountTab()
{
	/* QFrame *sep; */
	QVBox *vbox;
	QWidget *spacer;
	QGridLayout *grid;
	QWidget *frame;
	int row = 0;

	/* Create the main vbox. */
	accountBox = vbox = new QVBox(this);
	vbox->setSpacing(5);

	frame = new QFrame(vbox);
	grid = new QGridLayout(frame, 1, 1);
	grid->setSpacing(5);

	buildLoginOpts(grid, frame, row);

#if 0
	/* Separator */
	sep = new QFrame(frame);
	sep->setFrameStyle(QFrame::HLine | QFrame::Sunken);
	grid->addMultiCellWidget(sep, row, row, 0, 2);
	row++;
#endif

	buildUserOpts(grid, frame, row);

	/* Add a spacer. */
	spacer = new QLabel("", vbox);
	vbox->setStretchFactor(spacer, 1);

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

	return vbox;
}

QWidget *
QGaimAccountEditor::buildProtocolTab()
{
	QVBox *vbox;
	QFrame *frame;
	QWidget *spacer;
	QGridLayout *grid;
	QCheckBox *check;
	QLineEdit *entry;
	GList *l;
	QString buf;
	int row = 0;
	QString strValue;
	int intValue;
	bool boolValue;

	/* Create the main vbox */
	protocolBox = vbox = new QVBox(this);
	vbox->setSpacing(5);

	frame = new QFrame(vbox);
	grid = new QGridLayout(frame, 1, 1);
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
			GaimAccountOption *option = (GaimAccountOption *)l->data;

			switch (gaim_account_option_get_type(option))
			{
				case GAIM_PREF_BOOLEAN:
					if (account == NULL ||
						gaim_account_get_protocol(account) != protocol)
					{
						boolValue =
							gaim_account_option_get_default_bool(option);
					}
					else
						boolValue = gaim_account_get_bool(account,
								gaim_account_option_get_setting(option),
								gaim_account_option_get_default_bool(option));

					check = new QCheckBox(gaim_account_option_get_text(option),
										  vbox);
					grid->addMultiCellWidget(check, row, row, 1, 2);
					row++;

					protocolOptEntries =
						g_list_append(protocolOptEntries, check);

					break;

				case GAIM_PREF_INT:
					if (account == NULL ||
						gaim_account_get_protocol(account) != protocol)
					{
						intValue = gaim_account_option_get_default_int(option);
					}
					else
					{
						intValue = gaim_account_get_int(account,
								gaim_account_option_get_setting(option),
								gaim_account_option_get_default_int(option));
					}

					buf  = gaim_account_option_get_text(option);
					buf += ":";

					grid->addWidget(new QLabel(buf, frame), row, 0);

					entry = new QLineEdit(QString::number(intValue), frame);
					grid->addWidget(entry, row++, 1);

					protocolOptEntries =
						g_list_append(protocolOptEntries, entry);

					break;

				case GAIM_PREF_STRING:
					if (account == NULL ||
						gaim_account_get_protocol(account) != protocol)
					{
						strValue =
							gaim_account_option_get_default_string(option);
					}
					else
					{
						strValue = gaim_account_get_string(account,
								gaim_account_option_get_setting(option),
								gaim_account_option_get_default_string(option));
					}

					buf  = gaim_account_option_get_text(option);
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

	/* Add a spacer. */
	spacer = new QLabel("", vbox);
	vbox->setStretchFactor(spacer, 1);

	return vbox;
}

QWidget *
QGaimAccountEditor::buildProxyTab()
{
	QVBox *vbox;
	QWidget *spacer;
	QGridLayout *grid;
	QWidget *frame;
	GaimProxyInfo *proxyInfo;
	int row = 0;

	/* Create the main vbox */
	proxyBox = vbox = new QVBox(this);
	vbox->setSpacing(5);

	frame = new QFrame(vbox);
	grid = new QGridLayout(frame, 1, 1);
	grid->setSpacing(5);

	/* Proxy Type */
	grid->addWidget(new QLabel(tr("Proxy Type:"), frame), row, 0);
	proxyDropDown = new QComboBox(frame, "proxy type");
	grid->addWidget(proxyDropDown, row++, 1);
	row++;

	/* Connect the signal */
	connect(proxyDropDown, SIGNAL(activated(int)),
			this, SLOT(proxyTypeChanged(int)));

	/* Fill it. */
	proxyDropDown->insertItem(tr("Use Global Proxy Settings"));
	proxyDropDown->insertItem(tr("No Proxy"));
	proxyDropDown->insertItem(tr("HTTP"));
	proxyDropDown->insertItem(tr("SOCKS 4"));
	proxyDropDown->insertItem(tr("SOCKS 5"));

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

	/* Add a spacer. */
	spacer = new QLabel("", vbox);
	vbox->setStretchFactor(spacer, 1);

	/* Set the values for everything. */
	if (account != NULL &&
		(proxyInfo = gaim_account_get_proxy_info(account)) != NULL)
	{
		GaimProxyType type = gaim_proxy_info_get_type(proxyInfo);

		proxyDropDown->setCurrentItem((int)type + 1);

		if (type == GAIM_PROXY_NONE || type == GAIM_PROXY_USE_GLOBAL)
		{
			proxyHost->setReadOnly(true);
			proxyPort->setReadOnly(true);
			proxyUsername->setReadOnly(true);
			proxyPassword->setReadOnly(true);
		}
		else
		{
			proxyHost->setText(gaim_proxy_info_get_host(proxyInfo));
			proxyPort->setText(
				QString::number(gaim_proxy_info_get_port(proxyInfo)));
			proxyUsername->setText(gaim_proxy_info_get_username(proxyInfo));
			proxyPassword->setText(gaim_proxy_info_get_password(proxyInfo));
		}
	}
	else
	{
		proxyHost->setReadOnly(true);
		proxyPort->setReadOnly(true);
		proxyUsername->setReadOnly(true);
		proxyPassword->setReadOnly(true);
	}

	return vbox;
}

void
QGaimAccountEditor::buildLoginOpts(QGridLayout *grid, QWidget *parent,
								   int &row)
{
	QLineEdit *entry;
	QString username;
	GList *userSplits = NULL, *l, *l2;

	/* Protocol */
	grid->addWidget(new QLabel(tr("Protocol:"), parent), row, 0);
	protocolList = new QGaimProtocolBox(parent, "protocol combo");
	protocolList->setCurrentProtocol(protocol);

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
		username = gaim_account_get_username(account);

	if (userSplitEntries != NULL)
	{
		g_list_free(userSplitEntries);
		userSplitEntries = NULL;
	}

	for (l = userSplits; l != NULL; l = l->next)
	{
		GaimAccountUserSplit *split = (GaimAccountUserSplit *)l->data;
		QString buf;

		buf  = gaim_account_user_split_get_text(split);
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
		GaimAccountUserSplit *split;
		QString value;

		entry = (QLineEdit *)l->data;
		split = (GaimAccountUserSplit *)l2->data;

		if (account != NULL)
		{
			int i;

			i = username.find(gaim_account_user_split_get_separator(split));

			if (i != -1)
			{
				value    = username.mid(i + 1);
				username = username.left(i);
			}
		}

		if (value.isEmpty())
			value = gaim_account_user_split_get_default_value(split);

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
		passwordEntry->setText(gaim_account_get_password(account));

	/* Alias */
	grid->addWidget(new QLabel(tr("Alias:"), parent), row, 0);
	aliasEntry = new QLineEdit(parent);
	grid->addWidget(aliasEntry, row++, 1);

	if (account != NULL)
		aliasEntry->setText(gaim_account_get_alias(account));

	/* Remember Password */
	rememberPassCheck = new QCheckBox(tr("Remember Password"), parent);
	grid->addMultiCellWidget(rememberPassCheck, row, row, 1, 2);
	row++;

	if (account != NULL)
		rememberPassCheck->setChecked(
				gaim_account_get_remember_password(account));
	else
		rememberPassCheck->setChecked(true);

	/* Auto-Login */
	autoLoginCheck = new QCheckBox(tr("Auto-Login"), parent);
	grid->addMultiCellWidget(autoLoginCheck, row, row, 1, 2);
	row++;

	if (account != NULL)
		autoLoginCheck->setChecked(
				gaim_account_get_auto_login(account, "qpe-gaim"));

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
QGaimAccountEditor::buildUserOpts(QGridLayout *grid, QWidget *parent,
								  int &row)
{
	/* New mail notifications */
	mailNotificationCheck = new QCheckBox(tr("New Mail Notifications"), parent);
	grid->addMultiCellWidget(mailNotificationCheck, row, row, 1, 2);
	row++;

	/* TODO: Buddy Icon support */

	if (account != NULL)
		mailNotificationCheck->setChecked(
				gaim_account_get_check_mail(account));

	if (prplInfo != NULL && !(prplInfo->options & OPT_PROTO_MAIL_CHECK))
		mailNotificationCheck->hide();
}

void
QGaimAccountEditor::proxyTypeChanged(int index)
{
	newProxyType = (GaimProxyType)(index - 1);

	if (newProxyType == GAIM_PROXY_USE_GLOBAL ||
		newProxyType == GAIM_PROXY_NONE)
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
QGaimAccountEditor::protocolChanged(int index)
{
	GList *l;

	l = g_list_nth(gaim_plugins_get_protocols(), index);

	if (l == NULL)
	{
		gaim_debug(GAIM_DEBUG_FATAL, "QGaimAccountEditor",
				   "Protocol switched to is not in list!\n");
		abort();
	}

	plugin = (GaimPlugin *)l->data;

	prplInfo   = GAIM_PLUGIN_PROTOCOL_INFO(plugin);
	protocol   = prplInfo->protocol;
	protocolId = plugin->info->id;

	tabs->removePage(accountBox);
	tabs->removePage(protocolBox);
	tabs->removePage(proxyBox);

#if 0
	delete accountBox;
	delete protocolBox;
	delete proxyBox;
#endif

	buildTabs();
}

void
QGaimAccountEditor::accept()
{
	QString str, username;
	GList *l, *l2;
	bool newAccount = (account == NULL);

	if (account == NULL)
	{
		/* New Account */
		username = screenNameEntry->text();

		account = gaim_account_new(username, protocolId);
	}
	else
	{
		/* Protocol */
		gaim_account_set_protocol_id(account, protocolId);
	}

	/* Clear the existing settings. */
	gaim_account_clear_settings(account);

	/* Alias */
	str = aliasEntry->text();

	if (!str.isEmpty())
		gaim_account_set_alias(account, str);
	else
		gaim_account_set_alias(account, NULL);

	/* Buddy Icon */
	/* TODO */

	/* Remember Password */
	gaim_account_set_remember_password(account,
									   rememberPassCheck->isChecked());

	/* Check Mail */
	if (prplInfo->options & OPT_PROTO_MAIL_CHECK)
		gaim_account_set_check_mail(account,
									mailNotificationCheck->isChecked());

	/* Auto-Login */
	gaim_account_set_auto_login(account, "qpe-gaim",
								autoLoginCheck->isChecked());

	/* Password */
	str = passwordEntry->text();

	if (!str.isEmpty())
		gaim_account_set_password(account, str);
	else
		gaim_account_set_password(account, NULL);

	/* Build the username string. */
	username = screenNameEntry->text();

	for (l = prplInfo->user_splits, l2 = userSplitEntries;
		 l != NULL && l2 != NULL;
		 l = l->next, l2 = l2->next)
	{
		GaimAccountUserSplit *split = (GaimAccountUserSplit *)l->data;
		QLineEdit *entry = (QLineEdit *)l2->data;
		char sep[2] = " ";
		char *tmp;

		str = entry->text();

		*sep = gaim_account_user_split_get_separator(split);

		tmp = g_strconcat(username, sep,
						  (!str.isEmpty() ? str.latin1() :
						   gaim_account_user_split_get_default_value(split)),
						  NULL);

		username = tmp;

		g_free(tmp);
	}

	gaim_account_set_username(account, username);

	/* Add the protocol settings */
	for (l = prplInfo->protocol_options, l2 = protocolOptEntries;
		 l != NULL && l2 != NULL;
		 l = l->next, l2 = l2->next)
	{
		GaimPrefType type;
		GaimAccountOption *option = (GaimAccountOption *)l->data;
		QWidget *widget = (QWidget *)l2->data;
		QLineEdit *entry;
		QCheckBox *checkbox;
		const char *setting;
		QString value;
		int intValue;

		type    = gaim_account_option_get_type(option);
		setting = gaim_account_option_get_setting(option);

		switch (type)
		{
			case GAIM_PREF_STRING:
				entry = (QLineEdit *)widget;
				value = entry->text();
				gaim_account_set_string(account, setting, value);
				break;

			case GAIM_PREF_INT:
				bool ok;

				entry = (QLineEdit *)widget;
				intValue = entry->text().toInt(&ok);

				if (ok)
					gaim_account_set_int(account, setting, intValue);
				else
					gaim_account_set_int(account, setting, 0);

				break;

			case GAIM_PREF_BOOLEAN:
				checkbox = (QCheckBox *)widget;
				gaim_account_set_bool(account, setting,
									  checkbox->isChecked());
				break;

			default:
				break;
		}
	}

	/* Set the proxy information */
	if (newProxyType == GAIM_PROXY_NONE)
		gaim_account_set_proxy_info(account, NULL);
	else
	{
		GaimProxyInfo *proxyInfo;

		proxyInfo = gaim_account_get_proxy_info(account);

		/* Create the proxy info if it doesn't exist. */
		if (proxyInfo == NULL)
		{
			proxyInfo = gaim_proxy_info_new();
			gaim_account_set_proxy_info(account, proxyInfo);
		}

		/* Type */
		gaim_proxy_info_set_type(proxyInfo, newProxyType);

		/* Host */
		str = proxyHost->text();

		if (!str.isEmpty())
			gaim_proxy_info_set_host(proxyInfo, str);
		else
			gaim_proxy_info_set_host(proxyInfo, NULL);

		/* Port */
		str = proxyPort->text();

		if (!str.isEmpty())
		{
			bool ok;
			int intVal;

			intVal = str.toInt(&ok);

			if (ok)
				gaim_proxy_info_set_port(proxyInfo, intVal);
			else
				gaim_proxy_info_set_port(proxyInfo, 0);
		}
		else
			gaim_proxy_info_set_port(proxyInfo, 0);

		/* Username */
		str = proxyUsername->text();

		if (!str.isEmpty())
			gaim_proxy_info_set_username(proxyInfo, str);
		else
			gaim_proxy_info_set_username(proxyInfo, NULL);

		/* Password */
		str = proxyPassword->text();

		if (!str.isEmpty())
			gaim_proxy_info_set_password(proxyInfo, str);
		else
			gaim_proxy_info_set_password(proxyInfo, NULL);
	}

	if (newAccount)
		gaim_accounts_add(account);

	QDialog::accept();

	if (accountsWin != NULL)
		accountsWin->updateAccounts();
}
