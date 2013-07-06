/**
 * @file QQuailDialogs.cpp Small dialogs that don't belong in their own files
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
#include "QuailDialogs.h"
#include "QuailAccountBox.h"

#include <libpurple/blist.h>
#include <libpurple/conversation.h>
//#include <libpurple/multi.h>
#include <libpurple/notify.h>
#include <libpurple/prpl.h>

#include <QComboBox>
#include <QFrame>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

/**************************************************************************
 * QQuailAddBuddyDialog
 **************************************************************************/
QQuailAddBuddyDialog::QQuailAddBuddyDialog(QWidget *parent, const char *name,
										 WFlags fl)
	: QDialog(parent, name, fl)
{
	buildInterface();
}

void
QQuailAddBuddyDialog::setScreenName(const QString &screenName)
{
	screenNameEntry->setText(screenName);
}

void
QQuailAddBuddyDialog::setAlias(const QString &alias)
{
	aliasEntry->setText(alias);
}

void
QQuailAddBuddyDialog::setGroup(const QString &group)
{
	groupCombo->lineEdit()->setText(group);
}

void
QQuailAddBuddyDialog::setAccount(PurpleAccount *account)
{
	accountCombo->setCurrentAccount(account);
}

void
QQuailAddBuddyDialog::buildInterface()
{
	QFrame *frame;
	QGridLayout *grid;
	QLabel *label;
	QLabel *spacer;
	QVBox *vbox;
	QVBoxLayout *layout;

	setCaption(tr("Add Buddy"));

	layout = new QVBoxLayout(this);
	layout->setAutoAdd(true);

	vbox = new QVBox(this);
	vbox->setSpacing(5);
	vbox->setMargin(6);

	label = new QLabel(tr("<p>Please enter the screen name of the person you "
						  "would like to add to your buddy list. You may "
						  "optionally enter an alias, or nickname, for the "
						  "buddy. The alias will be displayed in place of "
						  "the screen name whenever possible.</p>"),
					   vbox);

	frame = new QFrame(vbox);
	grid = new QGridLayout(frame, 1, 1);
	grid->setSpacing(5);

	/* Username */
	grid->addWidget(new QLabel(tr("Screen Name:"), frame), 0, 0);
	screenNameEntry = new QLineEdit(frame);
	grid->addWidget(screenNameEntry, 0, 1);

	/* Alias */
	grid->addWidget(new QLabel(tr("Alias:"), frame), 1, 0);
	aliasEntry = new QLineEdit(frame);
	grid->addWidget(aliasEntry, 1, 1);

	/* Group */
	grid->addWidget(new QLabel(tr("Group:"), frame), 2, 0);
	groupCombo = new QComboBox(true, frame, "group");
	grid->addWidget(groupCombo, 2, 1);

	populateGroupCombo();

	/* Account */
	grid->addWidget(new QLabel(tr("Account:"), frame), 3, 0);
	accountCombo = new QQuailAccountBox(false, frame, "account");
	grid->addWidget(accountCombo, 3, 1);

	/* Add a spacer. */
	spacer = new QLabel("", vbox);
	vbox->setStretchFactor(spacer, 1);
}

void
QQuailAddBuddyDialog::populateGroupCombo()
{
	PurpleBlistNode *node = purple_get_blist()->root;

	if (node == NULL)
		groupCombo->insertItem(tr("Buddies"));
	else
	{
		for (; node != NULL; node = node->next)
		{
			if (GAIM_BLIST_NODE_IS_GROUP(node))
			{
				PurpleGroup *g = (PurpleGroup *)node;

				groupCombo->insertItem(g->name);
			}
		}
	}
}

void
QQuailAddBuddyDialog::accept()
{
    PurpleConversation *conv;
	QString screenname = screenNameEntry->text();
	QString alias = aliasEntry->text();
	QString group = groupCombo->currentText();
	PurpleAccount *account;
	PurpleBuddy *b;
	PurpleGroup *g;

	if (screenname.isEmpty())
	{
		purple_notify_error(this, tr("Add Buddy"),
						  tr("You must specify a screen name to add."), NULL);
		return;
	}

	conv = purple_find_conversation(screenname);

	if ((g = purple_find_group(group)) == NULL)
	{
		g = purple_group_new(group);
		purple_blist_add_group(g, NULL);
	}

	account = accountCombo->getCurrentAccount();

	b = purple_buddy_new(account, screenname,
					   (alias.isEmpty() ? NULL : (const char *)alias));

	purple_blist_add_buddy(b, NULL, g, NULL);
	serv_add_buddy(purple_account_get_connection(account), screenname, g);

	if (conv != NULL)
		purple_conversation_update(conv, GAIM_CONV_UPDATE_ADD);

	purple_blist_save();

	QDialog::accept();
}


/**************************************************************************
 * QQuailAddChatDialog
 **************************************************************************/
QQuailAddChatDialog::QQuailAddChatDialog(QWidget *parent, const char *name,
									   WFlags fl)
	: QDialog(parent, name, fl)
{
	buildInterface();
}

void
QQuailAddChatDialog::setAlias(const QString &alias)
{
	aliasEntry->setText(alias);
}

void
QQuailAddChatDialog::setGroup(const QString &group)
{
	groupCombo->lineEdit()->setText(group);
}

void
QQuailAddChatDialog::setAccount(PurpleAccount *account)
{
	accountCombo->setCurrentAccount(account);
}

void
QQuailAddChatDialog::buildInterface()
{
	QLabel *label;
	QLabel *spacer;
	QVBox *vbox;
	QVBoxLayout *layout;

	setCaption(tr("Add Chat"));

	layout = new QVBoxLayout(this);
	layout->setAutoAdd(true);

	vbox = new QVBox(this);
	vbox->setSpacing(5);
	vbox->setMargin(6);

	label = new QLabel(tr("<p>Please enter an alias and the appropriate "
						  "information about the chat you would like to add "
						  "to your buddy list.</p>"),
					   vbox);

	labels.setAutoDelete(true);
	widgets.setAutoDelete(true);

	widgetsFrame = new QFrame(vbox);
	grid = new QGridLayout(widgetsFrame, 1, 1);
	grid->setSpacing(5);

	/* Account */
	grid->addWidget(new QLabel(tr("Account:"), widgetsFrame), 0, 0);
	accountCombo = new QQuailAccountBox(false, widgetsFrame, "account");
	grid->addWidget(accountCombo, 0, 1);

	/* Connect the signal */
	connect(accountCombo, SIGNAL(activated(int)),
			this, SLOT(accountChanged(int)));

	/* Alias */
	aliasLabel = new QLabel(tr("Alias:"), widgetsFrame);
	grid->addWidget(aliasLabel, 1, 0);
	aliasEntry = new QLineEdit(widgetsFrame);
	grid->addWidget(aliasEntry, 1, 1);

	/* Group */
	groupLabel = new QLabel(tr("Group:"), widgetsFrame);
	grid->addWidget(groupLabel, 2, 0);
	groupCombo = new QComboBox(true, widgetsFrame, "group");
	grid->addWidget(groupCombo, 2, 1);

	populateGroupCombo();

	/* Add a spacer. */
	spacer = new QLabel("", vbox);
	vbox->setStretchFactor(spacer, 1);

	rebuildWidgetsFrame();
}

void
QQuailAddChatDialog::populateGroupCombo()
{
	PurpleBlistNode *node = purple_get_blist()->root;

	if (node == NULL)
		groupCombo->insertItem(tr("Buddies"));
	else
	{
		for (; node != NULL; node = node->next)
		{
			if (GAIM_BLIST_NODE_IS_GROUP(node))
			{
				PurpleGroup *g = (PurpleGroup *)node;

				groupCombo->insertItem(g->name);
			}
		}
	}
}

void
QQuailAddChatDialog::rebuildWidgetsFrame()
{
	GList *chatInfoList, *l;
	QLabel *label;
	PurpleConnection *gc;
	struct proto_chat_entry *pce;
	int row;

	gc = purple_account_get_connection(accountCombo->getCurrentAccount());

	labels.clear();
	widgets.clear();

	chatInfoList = GAIM_PLUGIN_PROTOCOL_INFO(gc->prpl)->chat_info(gc);

	for (l = chatInfoList, row = 1; l != NULL; l = l->next, row++)
	{
		pce = (struct proto_chat_entry *)l->data;

		label = new QLabel(tr(pce->label), widgetsFrame);
		grid->addWidget(label, row, 0);
		labels.append(label);
		label->show();

		if (pce->is_int)
		{
			QSpinBox *spinbox = new QSpinBox(pce->min, pce->max, 1,
											 widgetsFrame);
			spinbox->setValue(pce->min);

			grid->addWidget(spinbox, row, 1);

			widgets.append(spinbox);

			spinbox->show();
		}
		else
		{
			QLineEdit *edit = new QLineEdit(pce->def, widgetsFrame);

			grid->addWidget(edit, row, 1);

			widgets.append(edit);

			edit->show();
		}

		g_free(pce);
	}

	g_list_free(chatInfoList);

	grid->addWidget(aliasLabel, row, 0);
	grid->addWidget(aliasEntry, row, 1);
	row++;

	grid->addWidget(groupLabel, row, 0);
	grid->addWidget(groupCombo, row, 1);
}

void
QQuailAddChatDialog::accountChanged(int)
{
	rebuildWidgetsFrame();
}

void
QQuailAddChatDialog::accept()
{
	PurpleConnection *gc;
	PurpleChat *chat;
	PurpleGroup *group;
	GList *chatInfoList, *l;
	GHashTable *components;
	QString alias = aliasEntry->text();
	QString groupName = groupCombo->currentText();
	QWidget *widget;
	struct proto_chat_entry *pce;

	gc = purple_account_get_connection(accountCombo->getCurrentAccount());

	components = g_hash_table_new_full(g_str_hash, g_str_equal,
									   g_free, g_free);

	chatInfoList = GAIM_PLUGIN_PROTOCOL_INFO(gc->prpl)->chat_info(gc);

	for (widget = widgets.first(), l = chatInfoList;
		 widget != NULL && l != NULL;
		 widget = widgets.next(), l = l->next)
	{
		pce = (struct proto_chat_entry *)l->data;

		if (pce->is_int)
		{
			QSpinBox *spinbox = (QSpinBox *)widget;

			g_hash_table_replace(components,
								 g_strdup(pce->identifier),
								 g_strdup(spinbox->cleanText()));
		}
		else
		{
			QLineEdit *edit = (QLineEdit *)widget;

			g_hash_table_replace(components,
								 g_strdup(pce->identifier),
								 g_strdup(edit->text()));
		}

		g_free(pce);
	}

	g_list_free(chatInfoList);

	chat = purple_chat_new(accountCombo->getCurrentAccount(),
							   (alias.isEmpty() ? NULL : (const char *)alias),
							   components);

	if ((group = purple_find_group(groupName)) == NULL)
	{
		group = purple_group_new(groupName);
		purple_blist_add_group(group, NULL);
	}

	if (chat != NULL)
	{
		purple_blist_add_chat(chat, group, NULL);
		purple_blist_save();
	}

	QDialog::accept();
}


/**************************************************************************
 * QQuailNewImDialog
 **************************************************************************/
QQuailNewImDialog::QQuailNewImDialog(QWidget *parent, const char *name,
								   WFlags fl)
	: QDialog(parent, name, fl)
{
	buildInterface();
}

void
QQuailNewImDialog::setScreenName(const QString &screenName)
{
	screenNameEntry->setText(screenName);
}

void
QQuailNewImDialog::setAccount(PurpleAccount *account)
{
	accountCombo->setCurrentAccount(account);
}

void
QQuailNewImDialog::buildInterface()
{
	QFrame *frame;
	QGridLayout *grid;
	QLabel *label;
	QLabel *spacer;
	QVBox *vbox;
	QVBoxLayout *layout;

	setCaption(tr("New Message"));

	layout = new QVBoxLayout(this);
	layout->setAutoAdd(true);

	vbox = new QVBox(this);
	vbox->setSpacing(5);
	vbox->setMargin(6);

	label = new QLabel(tr("<p>Please enter the screen name of the person you "
						  "would like to IM.</p>"),
					   vbox);

	frame = new QFrame(vbox);
	grid = new QGridLayout(frame, 1, 1);
	grid->setSpacing(5);

	/* Screen Name */
	grid->addWidget(new QLabel(tr("Screen Name:"), frame), 0, 0);
	screenNameEntry = new QLineEdit(frame);
	grid->addWidget(screenNameEntry, 0, 1);

	/* Account */
	grid->addWidget(new QLabel(tr("Account:"), frame), 3, 0);
	accountCombo = new QQuailAccountBox(false, frame, "account");
	grid->addWidget(accountCombo, 3, 1);

	/* Add a spacer. */
	spacer = new QLabel("", vbox);
	vbox->setStretchFactor(spacer, 1);
}

void
QQuailNewImDialog::accept()
{
	QString screenname = screenNameEntry->text();
	PurpleAccount *account;
    PurpleConversation *conv;

	if (screenname.isEmpty())
		return;

	account = accountCombo->getCurrentAccount();

	conv = purple_find_conversation(screenname);

	if (conv == NULL)
		conv = purple_conversation_new(GAIM_CONV_IM, account, screenname);
	else
	{
		purple_conv_window_raise(purple_conversation_get_window(conv));

		if (account)
			purple_conversation_set_account(conv, account);
	}

	QDialog::accept();
}

/**************************************************************************
 * QQuailJoinChatDialog
 **************************************************************************/
QQuailJoinChatDialog::QQuailJoinChatDialog(QWidget *parent, const char *name,
										 WFlags fl)
	: QDialog(parent, name, fl)
{
	buildInterface();
}

void
QQuailJoinChatDialog::setAccount(PurpleAccount *account)
{
	accountCombo->setCurrentAccount(account);
}

void
QQuailJoinChatDialog::buildInterface()
{
	QLabel *spacer;
	QVBox *vbox;
	QVBoxLayout *layout;

	setCaption(tr("Join Chat"));

	layout = new QVBoxLayout(this);
	layout->setAutoAdd(true);

	vbox = new QVBox(this);
	vbox->setSpacing(5);
	vbox->setMargin(6);

	widgetsFrame = new QFrame(vbox);
	grid = new QGridLayout(widgetsFrame, 1, 1);
	grid->setSpacing(5);

	labels.setAutoDelete(true);
	widgets.setAutoDelete(true);

	/* Account */
	grid->addWidget(new QLabel(tr("Join Chat As:"), widgetsFrame), 0, 0);
	accountCombo = new QQuailAccountBox(false, widgetsFrame, "account");
	grid->addWidget(accountCombo, 0, 1);

	/* Connect the signal */
	connect(accountCombo, SIGNAL(activated(int)),
			this, SLOT(accountChanged(int)));

	/* Add a spacer. */
	spacer = new QLabel("", vbox);
	vbox->setStretchFactor(spacer, 1);

	rebuildWidgetsFrame();
}

void
QQuailJoinChatDialog::rebuildWidgetsFrame()
{
	GList *chatInfoList, *l;
	QLabel *label;
	PurpleConnection *gc;
	struct proto_chat_entry *pce;
	int row;

	gc = purple_account_get_connection(accountCombo->getCurrentAccount());

	labels.clear();
	widgets.clear();

	chatInfoList = GAIM_PLUGIN_PROTOCOL_INFO(gc->prpl)->chat_info(gc);

	for (l = chatInfoList, row = 1; l != NULL; l = l->next, row++)
	{
		pce = (struct proto_chat_entry *)l->data;

		label = new QLabel(tr(pce->label), widgetsFrame);
		grid->addWidget(label, row, 0);
		labels.append(label);
		label->show();

		if (pce->is_int)
		{
			QSpinBox *spinbox = new QSpinBox(pce->min, pce->max, 1,
											 widgetsFrame);
			spinbox->setValue(pce->min);

			grid->addWidget(spinbox, row, 1);

			widgets.append(spinbox);

			spinbox->show();
		}
		else
		{
			QLineEdit *edit = new QLineEdit(pce->def, widgetsFrame);

			grid->addWidget(edit, row, 1);

			widgets.append(edit);

			edit->show();
		}

		g_free(pce);
	}

	g_list_free(chatInfoList);
}

void
QQuailJoinChatDialog::accountChanged(int)
{
	rebuildWidgetsFrame();
}

void
QQuailJoinChatDialog::accept()
{
	PurpleConnection *gc;
	GHashTable *components;
	QWidget *widget;
	GList *chatInfoList, *l;
	struct proto_chat_entry *pce;

	gc = purple_account_get_connection(accountCombo->getCurrentAccount());

	components = g_hash_table_new_full(g_str_hash, g_str_equal,
									   g_free, g_free);

	chatInfoList = GAIM_PLUGIN_PROTOCOL_INFO(gc->prpl)->chat_info(gc);

	for (widget = widgets.first(), l = chatInfoList;
		 widget != NULL && l != NULL;
		 widget = widgets.next(), l = l->next)
	{
		pce = (struct proto_chat_entry *)l->data;

		if (pce->is_int)
		{
			QSpinBox *spinbox = (QSpinBox *)widget;

			g_hash_table_replace(components,
								 g_strdup(pce->identifier),
								 g_strdup(spinbox->cleanText()));
		}
		else
		{
			QLineEdit *edit = (QLineEdit *)widget;

			g_hash_table_replace(components,
								 g_strdup(pce->identifier),
								 g_strdup(edit->text()));
		}

		g_free(pce);
	}

	g_list_free(chatInfoList);

	serv_join_chat(gc, components);

	g_hash_table_destroy(components);

	QDialog::accept();
}
