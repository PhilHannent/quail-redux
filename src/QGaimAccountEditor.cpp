#include "QGaimAccountEditor.h"
#include "QGaimProtocolBox.h"
#include "QGaimProtocolUtils.h"
#include "base.h"

#include <libgaim/accountopt.h>

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpixmap.h>
#include <qtabwidget.h>
#include <qvbox.h>

QGaimAccountEditor::QGaimAccountEditor(GaimAccount *account)
	: QMainWindow(), account(account), plugin(NULL), prplInfo(NULL),
	  userSplitEntries(NULL), protocolOptEntries(NULL)
{
	GaimProtocol protocol;

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
}

void
QGaimAccountEditor::buildInterface()
{
	if (account == NULL)
		setCaption(tr("Add Account"));
	else
		setCaption(tr("Edit Account"));

	tabs = new QTabWidget(this);
	tabs->setMargin(6);

	setCentralWidget(tabs);

	tabs->addTab(buildAccountTab(), "Account");

	if (plugin != NULL)
	{
		QPixmap *pixmap = QGaimProtocolUtils::getProtocolIcon(plugin);

		if (pixmap == NULL)
			tabs->addTab(buildProtocolTab(), plugin->info->name);
		else
		{
			tabs->addTab(buildProtocolTab(), QIconSet(*pixmap),
						 plugin->info->name);

			delete pixmap;
		}
	}

	tabs->addTab(buildProxyTab(), "Proxy");
}

QWidget *
QGaimAccountEditor::buildAccountTab()
{
	QFrame *sep;
	QVBox *vbox;
	QWidget *spacer;

	/* Create the main vbox. */
	vbox = new QVBox(this);
	vbox->setSpacing(5);

	buildLoginOpts(vbox);

	/* Separator */
	sep = new QFrame(vbox);
	sep->setFrameStyle(QFrame::HLine | QFrame::Sunken);

	buildUserOpts(vbox);

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
	return new QLabel("Protocol", this);
}

QWidget *
QGaimAccountEditor::buildProxyTab()
{
	return new QLabel("Proxy", this);
}

QWidget *
QGaimAccountEditor::buildLoginOpts(QWidget *parent)
{
	QFrame *frame;
	QGridLayout *grid;
	QLineEdit *entry;
	QString username;
	GList *userSplits, *l, *l2;
	int row = 0;

	/* Create the Login Options group box. */
	frame = new QFrame(parent);
	grid = new QGridLayout(frame, 1, 1);
	grid->setSpacing(4);

	/* Protocol */
	grid->addWidget(new QLabel(tr("Protocol:"), frame), row, 0);
	protocolList = new QGaimProtocolBox(frame, "protocol combo");
	grid->addWidget(protocolList, row++, 1);

	/* Screen Name */
	grid->addWidget(new QLabel(tr("Screenname:"), frame), row, 0);
	screenNameEntry = new QLineEdit(frame);
	grid->addWidget(screenNameEntry, row++, 1);

	/* User split stuff. */
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

		grid->addWidget(new QLabel(buf, frame), row, 0);
		entry = new QLineEdit(frame);
		grid->addWidget(entry, row++, 1);

		userSplitEntries = g_list_append(userSplitEntries, entry);
	}

	for (l  = g_list_last(userSplitEntries),
		 l2 = g_list_last(userSplits);
		 l != NULL && l2 != NULL;
		 l = l->prev, l2 = l2->prev)
	{
		GaimAccountUserSplit *split = (GaimAccountUserSplit *)l2->data;
		QString value;

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
	passwordLabel = new QLabel(tr("Password:"), frame);
	grid->addWidget(passwordLabel, row, 0);
	passwordEntry = new QLineEdit(frame);
	passwordEntry->setReadOnly(true);
	grid->addWidget(passwordEntry, row++, 1);

	if (account != NULL)
		passwordEntry->setText(gaim_account_get_password(account));

	/* Alias */
	grid->addWidget(new QLabel(tr("Alias:"), frame), row, 0);
	aliasEntry = new QLineEdit(frame);
	grid->addWidget(aliasEntry, row++, 1);

	if (account != NULL)
		aliasEntry->setText(gaim_account_get_alias(account));

	/* Remember Password */
	rememberPassCheck = new QCheckBox(tr("Remember Password"), frame);
	grid->addMultiCellWidget(rememberPassCheck, row, row, 1, 2);
	row++;

	if (account != NULL)
		rememberPassCheck->setChecked(
				gaim_account_get_remember_password(account));
	else
		rememberPassCheck->setChecked(true);

	/* Auto-Login */
	autoLoginCheck = new QCheckBox(tr("Auto-Login"), frame);
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

	return frame;
}

QWidget *
QGaimAccountEditor::buildUserOpts(QWidget *parent)
{
	return new QLabel("", parent);
}
