﻿/**
 * @file QQuailDialogs.cpp Small dialogs that don't belong in their own files
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
#include "QuailDialogs.h"
#include "QuailAccountBox.h"
#include "global.h"

#include <libpurple/blist.h>
#include <libpurple/conversation.h>
#include <libpurple/notify.h>
#include <libpurple/prpl.h>

#include <QComboBox>
#include <QDebug>
#include <QFrame>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

static QString
stripUnderscores(QString text)
{
    if (text.startsWith("_")) {
        text = text.remove(0,1);
    }
    return text;
}

quail_dialog::quail_dialog(QWidget *parent) :
    QDialog(parent)
{
}

/**************************************************************************
 * QQuailAddBuddyDialog
 **************************************************************************/
quail_add_buddy_dialog::quail_add_buddy_dialog(QWidget *parent)
    : quail_dialog(parent)
{
	buildInterface();
}

void
quail_add_buddy_dialog::setScreenName(const QString &screenName)
{
	screenNameEntry->setText(screenName);
}

void
quail_add_buddy_dialog::setAlias(const QString &alias)
{
	aliasEntry->setText(alias);
}

void
quail_add_buddy_dialog::setGroup(const QString &group)
{
	groupCombo->lineEdit()->setText(group);
}

void
quail_add_buddy_dialog::setAccount(PurpleAccount *account)
{
	accountCombo->setCurrentAccount(account);
}

void
quail_add_buddy_dialog::buildInterface()
{
	QFrame *frame;
	QGridLayout *grid;
	QLabel *label;
	QLabel *spacer;
    QVBoxLayout *vbox;

    setWindowTitle(tr("Add Buddy"));

    vbox = new QVBoxLayout(this);
	vbox->setSpacing(5);
	vbox->setMargin(6);

	label = new QLabel(tr("<p>Please enter the screen name of the person you "
						  "would like to add to your buddy list. You may "
						  "optionally enter an alias, or nickname, for the "
						  "buddy. The alias will be displayed in place of "
                          "the screen name whenever possible.</p>"));
    vbox->addWidget(label);
    frame = new QFrame();
    vbox->addWidget(frame);
    grid = new QGridLayout();
    frame->setLayout(grid);
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
    groupCombo = new QComboBox();
	grid->addWidget(groupCombo, 2, 1);

	populateGroupCombo();

	/* Account */
	grid->addWidget(new QLabel(tr("Account:"), frame), 3, 0);
    accountCombo = new quail_account_box(false, frame);
	grid->addWidget(accountCombo, 3, 1);

	/* Add a spacer. */
    spacer = new QLabel("");
    vbox->addWidget(spacer);
	vbox->setStretchFactor(spacer, 1);

    QHBoxLayout *hbox = new QHBoxLayout(this);
    okButton = new QPushButton(this);
    okButton->setText(tr("Join"));
    connect(okButton, SIGNAL(released()), this, SLOT(accept()));
    hbox->addWidget(okButton);

    cancelButton = new QPushButton(this);
    cancelButton->setText(tr("Cancel"));
    connect(cancelButton, SIGNAL(released()), this, SLOT(close()));
    hbox->addWidget(cancelButton);
    vbox->addLayout(hbox);

}

void
quail_add_buddy_dialog::populateGroupCombo()
{
	PurpleBlistNode *node = purple_get_blist()->root;

	if (node == NULL)
        groupCombo->addItem(tr("Buddies"));
	else
	{
		for (; node != NULL; node = node->next)
		{
            if (PURPLE_BLIST_NODE_IS_GROUP(node))
			{
				PurpleGroup *g = (PurpleGroup *)node;

                groupCombo->addItem(g->name);
			}
		}
	}
}

void
quail_add_buddy_dialog::accept()
{
    qDebug() << "QQuailAddBuddyDialog::accept()";
    PurpleConversation *conv;
    QString screenname = screenNameEntry->text();
    QString alias = aliasEntry->text();
    QString group = groupCombo->currentText();
    PurpleAccount *account;
    PurpleBuddy *b;
    PurpleGroup *g;

	if (screenname.isEmpty())
	{
        purple_notify_error(this,
                            NULL,
                            tr("Add Buddy").toStdString().c_str(),
                            tr("You must specify a screen name to add.").toStdString().c_str());
		return;
	}

    account = accountCombo->getCurrentAccount();
    conv = purple_find_conversation_with_account(PURPLE_CONV_TYPE_UNKNOWN,
                                                 screenname.toStdString().c_str(),
                                                 account);


    if ((g = purple_find_group(group.toStdString().c_str())) == NULL)
    {
        g = purple_group_new(group.toStdString().c_str());
        purple_blist_add_group(g, NULL);
    }

    b = purple_buddy_new(account, screenname.toStdString().c_str(),
                       (alias.isEmpty() ? NULL : (const char *)alias.toStdString().c_str()));

    purple_blist_add_buddy(b, NULL, g, NULL);

    if (conv != NULL)
        purple_conversation_update(conv, PURPLE_CONV_UPDATE_ADD);


	QDialog::accept();
}


/**************************************************************************
 * QQuailAddChatDialog
 **************************************************************************/
quail_add_chat_dialog::quail_add_chat_dialog(QWidget *parent)
    : quail_dialog(parent)
{
	buildInterface();
}

void
quail_add_chat_dialog::setAlias(const QString &alias)
{
	aliasEntry->setText(alias);
}

void
quail_add_chat_dialog::setGroup(const QString &group)
{
	groupCombo->lineEdit()->setText(group);
}

void
quail_add_chat_dialog::setAccount(PurpleAccount *account)
{
	accountCombo->setCurrentAccount(account);
}

void
quail_add_chat_dialog::buildInterface()
{
	QLabel *label;
	QLabel *spacer;
    QVBoxLayout *vbox;

    setWindowTitle(tr("Add Chat"));

    vbox = new QVBoxLayout(this);
	vbox->setSpacing(5);
	vbox->setMargin(6);

	label = new QLabel(tr("<p>Please enter an alias and the appropriate "
						  "information about the chat you would like to add "
                          "to your buddy list.</p>"));

    vbox->addWidget(label);

    widgetsFrame = new QFrame(this);
    vbox->addWidget(widgetsFrame);
    grid = new QGridLayout();
    widgetsFrame->setLayout(grid);
	grid->setSpacing(5);

	/* Account */
	grid->addWidget(new QLabel(tr("Account:"), widgetsFrame), 0, 0);
    accountCombo = new quail_account_box(false, widgetsFrame);
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
    groupCombo = new QComboBox();
    //widgetsFrame
	grid->addWidget(groupCombo, 2, 1);

	populateGroupCombo();

	/* Add a spacer. */
    spacer = new QLabel("");
    vbox->addWidget(spacer);
	vbox->setStretchFactor(spacer, 1);

    QHBoxLayout *hbox = new QHBoxLayout(this);
    okButton = new QPushButton(this);
    okButton->setText(tr("Join"));
    connect(okButton, SIGNAL(released()), this, SLOT(accept()));
    hbox->addWidget(okButton);

    cancelButton = new QPushButton(this);
    cancelButton->setText(tr("Cancel"));
    connect(cancelButton, SIGNAL(released()), this, SLOT(close()));
    hbox->addWidget(cancelButton);
    vbox->addLayout(hbox);

	rebuildWidgetsFrame();
}

void
quail_add_chat_dialog::populateGroupCombo()
{
	PurpleBlistNode *node = purple_get_blist()->root;

	if (node == NULL)
        groupCombo->addItem(tr("Buddies"));
	else
	{
		for (; node != NULL; node = node->next)
		{
            if (PURPLE_BLIST_NODE_IS_GROUP(node))
			{
				PurpleGroup *g = (PurpleGroup *)node;

                groupCombo->addItem(g->name);
			}
		}
	}
}

void
quail_add_chat_dialog::rebuildWidgetsFrame()
{
	GList *chatInfoList, *l;
	QLabel *label;
	PurpleConnection *gc;
	struct proto_chat_entry *pce;
	int row;

	gc = purple_account_get_connection(accountCombo->getCurrentAccount());

	labels.clear();
	widgets.clear();

    chatInfoList = PURPLE_PLUGIN_PROTOCOL_INFO(gc->prpl)->chat_info(gc);

	for (l = chatInfoList, row = 1; l != NULL; l = l->next, row++)
	{
		pce = (struct proto_chat_entry *)l->data;

		label = new QLabel(tr(pce->label), widgetsFrame);
		grid->addWidget(label, row, 0);
		labels.append(label);
		label->show();

		if (pce->is_int)
		{
            QSpinBox *spinbox = new QSpinBox(widgetsFrame);
            spinbox->setMinimum(pce->min);
            spinbox->setMaximum(pce->max);
			spinbox->setValue(pce->min);

			grid->addWidget(spinbox, row, 1);

			widgets.append(spinbox);

			spinbox->show();
		}
		else
		{
            QLineEdit *edit = new QLineEdit(pce->identifier, widgetsFrame);
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
quail_add_chat_dialog::accountChanged(int)
{
	rebuildWidgetsFrame();
}

void
quail_add_chat_dialog::accept()
{
    qDebug() << "QQuailAddChatDialog::accept()";
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

    chatInfoList = PURPLE_PLUGIN_PROTOCOL_INFO(gc->prpl)->chat_info(gc);
    QListIterator<QWidget*> widgetsIter(widgets);
    widgetsIter.toFront();
    for (widget = widgetsIter.next(), l = chatInfoList;
		 widget != NULL && l != NULL;
         widget = widgetsIter.next(), l = l->next)
	{
		pce = (struct proto_chat_entry *)l->data;

		if (pce->is_int)
		{
			QSpinBox *spinbox = (QSpinBox *)widget;

			g_hash_table_replace(components,
								 g_strdup(pce->identifier),
                                 g_strdup(spinbox->cleanText().toStdString().c_str()));
		}
		else
		{
			QLineEdit *edit = (QLineEdit *)widget;

			g_hash_table_replace(components,
								 g_strdup(pce->identifier),
                                 g_strdup(edit->text().toStdString().c_str()));
		}

		g_free(pce);
	}

	g_list_free(chatInfoList);

	chat = purple_chat_new(accountCombo->getCurrentAccount(),
                               (alias.isEmpty() ? NULL : (const char *)alias.toStdString().c_str()),
							   components);

    if ((group = purple_find_group(groupName.toStdString().c_str())) == NULL)
	{
        group = purple_group_new(groupName.toStdString().c_str());
		purple_blist_add_group(group, NULL);
	}

	if (chat != NULL)
	{
		purple_blist_add_chat(chat, group, NULL);
	}
    qDebug() << "QQuailAddChatDialog::accept().end";
	QDialog::accept();
}


/**************************************************************************
 * QQuailNewImDialog
 **************************************************************************/
quail_new_im_dialog::quail_new_im_dialog(QWidget *parent)
    : quail_dialog(parent)
{
	buildInterface();
}

void
quail_new_im_dialog::setScreenName(const QString &screenName)
{
	screenNameEntry->setText(screenName);
}

void
quail_new_im_dialog::setAccount(PurpleAccount *account)
{
	accountCombo->setCurrentAccount(account);
}

void
quail_new_im_dialog::buildInterface()
{
	QFrame *frame;
	QGridLayout *grid;
	QLabel *label;
	QLabel *spacer;
    QVBoxLayout *vbox;

    setWindowTitle(tr("New Message"));

    vbox = new QVBoxLayout(this);
	vbox->setSpacing(5);
	vbox->setMargin(6);

	label = new QLabel(tr("<p>Please enter the screen name of the person you "
                          "would like to IM.</p>"));
    vbox->addWidget(label);
    frame = new QFrame();
    vbox->addWidget(frame);
    grid = new QGridLayout();
    grid->addWidget(frame);
	grid->setSpacing(5);

	/* Screen Name */
	grid->addWidget(new QLabel(tr("Screen Name:"), frame), 0, 0);
	screenNameEntry = new QLineEdit(frame);
	grid->addWidget(screenNameEntry, 0, 1);

	/* Account */
	grid->addWidget(new QLabel(tr("Account:"), frame), 3, 0);
    accountCombo = new quail_account_box(false, frame);
	grid->addWidget(accountCombo, 3, 1);

	/* Add a spacer. */
    spacer = new QLabel();
    vbox->addWidget(spacer);
	vbox->setStretchFactor(spacer, 1);

    QHBoxLayout *hbox = new QHBoxLayout(this);
    okButton = new QPushButton(this);
    okButton->setText(tr("Join"));
    connect(okButton, SIGNAL(released()), this, SLOT(accept()));
    hbox->addWidget(okButton);

    cancelButton = new QPushButton(this);
    cancelButton->setText(tr("Cancel"));
    connect(cancelButton, SIGNAL(released()), this, SLOT(close()));
    hbox->addWidget(cancelButton);
    vbox->addLayout(hbox);

}

void
quail_new_im_dialog::accept()
{
    qDebug() << "QQuailNewImDialog::accept()";
	QString screenname = screenNameEntry->text();
    PurpleAccount *account;
    PurpleConversation *conv;

	if (screenname.isEmpty())
		return;

    account = accountCombo->getCurrentAccount();

    conv = purple_find_conversation_with_account(PURPLE_CONV_TYPE_IM,
                                                 screenname.toStdString().c_str(),
                                                 account);

    if (conv == NULL)
        conv = purple_conversation_new(PURPLE_CONV_TYPE_IM,
                                       account,
                                       screenname.toStdString().c_str());
    else
    {
        purple_conversation_present(conv);

        if (account)
            purple_conversation_set_account(conv, account);
    }

	QDialog::accept();
}

/**************************************************************************
 * QQuailJoinChatDialog
 **************************************************************************/
quail_join_chat_dialog::quail_join_chat_dialog(QWidget *parent)
    : quail_dialog(parent)
{

	buildInterface();
}

void
quail_join_chat_dialog::setAccount(PurpleAccount *account)
{
	accountCombo->setCurrentAccount(account);
}

void
quail_join_chat_dialog::buildInterface()
{
    QVBoxLayout *vbox;

    setWindowTitle(tr("Join Chat"));

    vbox = new QVBoxLayout(this);
	vbox->setSpacing(5);
	vbox->setMargin(6);

    widgetsFrame = new QFrame();
    vbox->addWidget(widgetsFrame);

    grid = new QGridLayout();
    widgetsFrame->setLayout(grid);
	grid->setSpacing(5);

	/* Account */
	grid->addWidget(new QLabel(tr("Join Chat As:"), widgetsFrame), 0, 0);
    accountCombo = new quail_account_box(false, widgetsFrame);
	grid->addWidget(accountCombo, 0, 1);

	/* Connect the signal */
	connect(accountCombo, SIGNAL(activated(int)),
			this, SLOT(accountChanged(int)));

	rebuildWidgetsFrame();

    QHBoxLayout *hbox = new QHBoxLayout(this);
    okButton = new QPushButton(this);
    okButton->setText(tr("Join"));
    connect(okButton, SIGNAL(released()), this, SLOT(accept()));
    hbox->addWidget(okButton);

    cancelButton = new QPushButton(this);
    cancelButton->setText(tr("Cancel"));
    connect(cancelButton, SIGNAL(released()), this, SLOT(close()));
    hbox->addWidget(cancelButton);
    vbox->addLayout(hbox);
}

void
quail_join_chat_dialog::rebuildWidgetsFrame()
{
	GList *chatInfoList, *l;
	QLabel *label;
	PurpleConnection *gc;
	struct proto_chat_entry *pce;
	int row;

	gc = purple_account_get_connection(accountCombo->getCurrentAccount());

	labels.clear();
	widgets.clear();

    chatInfoList = PURPLE_PLUGIN_PROTOCOL_INFO(gc->prpl)->chat_info(gc);

	for (l = chatInfoList, row = 1; l != NULL; l = l->next, row++)
	{
		pce = (struct proto_chat_entry *)l->data;
        label = new QLabel(stripUnderscores(pce->label), widgetsFrame);
		grid->addWidget(label, row, 0);
		labels.append(label);
		label->show();

		if (pce->is_int)
		{
            QSpinBox *spinbox = new QSpinBox(widgetsFrame);
            spinbox->setRange(pce->min, pce->max);
			spinbox->setValue(pce->min);

			grid->addWidget(spinbox, row, 1);

			widgets.append(spinbox);

			spinbox->show();
		}
		else
		{
            QLineEdit *edit = new QLineEdit(widgetsFrame);
            edit->setText("");

			grid->addWidget(edit, row, 1);

			widgets.append(edit);

			edit->show();
		}

		g_free(pce);
	}

	g_list_free(chatInfoList);
}

void
quail_join_chat_dialog::accountChanged(int)
{
	rebuildWidgetsFrame();
}

void
quail_join_chat_dialog::accept()
{
    qDebug() << "QQuailJoinChatDialog::accept()";
    PurpleConnection *gc;
	GHashTable *components;
	QWidget *widget;
	GList *chatInfoList, *l;
	struct proto_chat_entry *pce;

	gc = purple_account_get_connection(accountCombo->getCurrentAccount());

	components = g_hash_table_new_full(g_str_hash, g_str_equal,
									   g_free, g_free);

    chatInfoList = PURPLE_PLUGIN_PROTOCOL_INFO(gc->prpl)->chat_info(gc);

    QListIterator<QWidget*> widgetIter(widgets);
    widgetIter.toFront();
    for (widget = widgetIter.next(), l = chatInfoList;
		 widget != NULL && l != NULL;
         widget = widgetIter.next(), l = l->next)
	{
		pce = (struct proto_chat_entry *)l->data;

		if (pce->is_int)
		{
            QSpinBox *spinbox = (QSpinBox *)widget;

            g_hash_table_replace(components,
                                 g_strdup(pce->identifier),
                                 g_strdup(spinbox->text().trimmed().toStdString().c_str()));
		}
		else
		{
            QLineEdit *edit = (QLineEdit *)widget;

            g_hash_table_replace(components,
                                 g_strdup(pce->identifier),
                                 g_strdup(edit->text().toStdString().c_str()));
		}

		g_free(pce);
	}

	g_list_free(chatInfoList);

	serv_join_chat(gc, components);

	g_hash_table_destroy(components);

	QDialog::accept();
}
