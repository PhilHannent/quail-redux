﻿/**
 * @file QQuailBListWindow.cpp Buddy list window
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
#include "QuailBListWindow.h"
#include "QuailConvButton.h"
#include "QuailConvWindow.h"
#include "QuailDialogs.h"
#include "QuailPrefsDialog.h"
#include "QuailMainWindow.h"
#include "base.h"

#include <libpurple/debug.h>
//TODO: Find the replacement for multi.h
//#include <libpurple/multi.h>
#include <libpurple/prefs.h>
#include <libpurple/request.h>
#include <libpurple/signals.h>

//#include <qpe/resource.h>

#include <QAction>
//#include <qheader.h>
#include <QLabel>
#include <QLayout>
#include <QListView>
#include <QMessageBox>
#include <QMenu>
#include <QPushButton>
#include <QToolbar>
#include <QToolButton>


/**************************************************************************
 * QQuailBListWindow
 **************************************************************************/
QQuailBListWindow::QQuailBListWindow(QMainWindow *parent)
	: QMainWindow(), parentMainWindow(parent), convsMenu(NULL)
{
	connect(parent, SIGNAL(pixmapSizeChanged(bool)),
			this, SLOT(setUsesBigPixmaps(bool)));
	setUsesBigPixmaps(parent->usesBigPixmaps());

	buildInterface();
}

QQuailBListWindow::~QQuailBListWindow()
{
	if (convsMenu != NULL)
		delete convsMenu;
}

void
QQuailBListWindow::buildInterface()
{
	newChatIconSet = QIconSet(Resource::loadPixmap("gaim/16x16/new-chat"),
							  Resource::loadPixmap("gaim/32x32/new-chat.png"));

	openChatIconSet = QIconSet(Resource::loadPixmap("gaim/16x16/chat"),
							   Resource::loadPixmap("gaim/32x32/chat"));
	newImIconSet = QIconSet(Resource::loadPixmap("gaim/16x16/new-im"),
							Resource::loadPixmap("gaim/32x32/new-im"));
	sendImIconSet = QIconSet(Resource::loadPixmap("gaim/16x16/send-im"),
							 Resource::loadPixmap("gaim/32x32/send-im"));

	/* Setup the toolbar */
	buildToolBar();

	/* Setup the buddy list */
	buddylist = new QQuailBuddyList(this, "BuddyList");

    connect(buddylist, SIGNAL(currentChanged(QListWidgetItem *)),
            this, SLOT(nodeChanged(QListWidgetItem *)));
    connect(buddylist, SIGNAL(doubleClicked(QListWidgetItem *)),
            this, SLOT(doubleClickList(QListWidgetItem *)));
	connect(buddylist, SIGNAL(openIm(PurpleBuddy *)),
			this, SLOT(openImSlot(PurpleBuddy *)));

	connect(buddylist, SIGNAL(addBuddy(PurpleGroup *)),
			this, SLOT(showAddBuddy(PurpleGroup *)));
	connect(buddylist, SIGNAL(addChat(PurpleGroup *)),
			this, SLOT(showAddChat(PurpleGroup *)));

	connect(buddylist, SIGNAL(joinChat(PurpleChat *)),
			this, SLOT(openChatSlot(PurpleChat *)));

	connect(buddylist, SIGNAL(removeBuddy(PurpleBuddy *)),
			this, SLOT(showConfirmRemoveBuddy(PurpleBuddy *)));
	connect(buddylist, SIGNAL(removeContact(PurpleContact *)),
			this, SLOT(showConfirmRemoveContact(PurpleContact *)));
	connect(buddylist, SIGNAL(removeGroup(PurpleGroup *)),
			this, SLOT(showConfirmRemoveGroup(PurpleGroup *)));
	connect(buddylist, SIGNAL(removeChat(PurpleChat *)),
			this, SLOT(showConfirmRemoveChat(PurpleChat *)));

	setCentralWidget(buddylist);
}

void
QQuailBListWindow::buildToolBar()
{
	QAction *a;
	QLabel *label;
	QToolButton *button;

	setToolBarsMovable(false);

	toolbar = new QToolBar(this);
	toolbar->setHorizontalStretchable(TRUE);

	/* IM */
	a = new QAction(tr("Send IM"),
					QIconSet(Resource::loadPixmap("gaim/16x16/new-im"),
							 Resource::loadPixmap("gaim/32x32/new-im")),
					QString::null, 0, this, 0);
	imButton = a;
	a->addTo(toolbar);
	a->setEnabled(false);

	connect(a, SIGNAL(activated()),
			this, SLOT(openImSlot()));

	/* Chat */
	a = new QAction(tr("Open Chat"), newChatIconSet,
					QString::null, 0, this, 0);
	chatButton = a;
	a->addTo(toolbar);
	a->setEnabled(false);

	connect(a, SIGNAL(activated()),
			this, SLOT(openChatSlot()));

	toolbar->addSeparator();

	/* Add */
	button = new QToolButton(toolbar, "add");
	button->setAutoRaise(true);
	button->setIconSet(QIconSet(Resource::loadPixmap("gaim/16x16/add"),
								Resource::loadPixmap("gaim/32x32/add")));
	button->setEnabled(false);
	addButton = button;

	addMenu = new QPopupMenu(button);
	button->setPopup(addMenu);
	button->setPopupDelay(0);

	/* Add Buddy */
	a = new QAction(tr("Add Buddy"),
					QIconSet(Resource::loadPixmap("gaim/16x16/user"),
							 Resource::loadPixmap("gaim/32x32/user")),
					QString::null, 0, this, 0);
	addBuddyButton = a;
	a->addTo(addMenu);

	connect(a, SIGNAL(activated()),
			this, SLOT(showAddBuddy()));

	/* Add Chat */
	a = new QAction(tr("Add Chat"), newChatIconSet,
					QString::null, 0, this, 0);
	addChatButton = a;
	a->addTo(addMenu);

	connect(a, SIGNAL(activated()),
			this, SLOT(showAddChat()));

	/* Add Group */
	a = new QAction(tr("Add Group"),
					QIconSet(Resource::loadPixmap("gaim/16x16/new-group"),
							 Resource::loadPixmap("gaim/32x32/new-group")),
					QString::null, 0, this, 0);
	addGroupButton = a;
	a->addTo(addMenu);

	connect(a, SIGNAL(activated()),
			this, SLOT(showAddGroup()));

	/* Remove */
	a = new QAction(tr("Remove"),
					QIconSet(Resource::loadPixmap("gaim/16x16/remove"),
							 Resource::loadPixmap("gaim/32x32/remove")),
					QString::null, 0, this, 0);
	removeButton = a;
	a->addTo(toolbar);
	a->setEnabled(false);

	connect(a, SIGNAL(activated()),
			this, SLOT(showRemoveBuddy()));

	toolbar->addSeparator();

	/* Settings menu */
	button = new QToolButton(toolbar, "settings");
	button->setAutoRaise(true);
	button->setIconSet(QIconSet(Resource::loadPixmap("gaim/16x16/settings"),
								Resource::loadPixmap("gaim/32x32/settings")));

	settingsMenu = new QPopupMenu(button);
	button->setPopup(settingsMenu);
	button->setPopupDelay(0);

	/* Show Offline Buddies */
	a = new QAction(tr("Show Offline Buddies"),
		QIconSet(Resource::loadPixmap("gaim/16x16/offline_buddies"),
		Resource::loadPixmap("gaim/32x32/offline_buddies")),
		QString::null, 0, this, 0, true);
	a->setOn(purple_prefs_get_bool("/gaim/qpe/blist/show_offline_buddies"));
	showOfflineButton = a;
	a->addTo(settingsMenu);

	connect(a, SIGNAL(toggled(bool)),
			this, SLOT(showOfflineBuddies(bool)));

	/* Separator */
	settingsMenu->insertSeparator();

	/* Preferences */
	a = new QAction(tr("Preferences"), QString::null, 0, this, 0);
	a->addTo(settingsMenu);

	connect(a, SIGNAL(activated()),
			this, SLOT(showPreferencesSlot()));

	/* Add some whitespace. */
	label = new QLabel(toolbar);
	label->setText("");
	toolbar->setStretchableWidget(label);

	/* Now we're going to construct the toolbar on the right. */
	toolbar->addSeparator();

	/* Buddy List */
	blistButton = new QAction(tr("Buddy List"),
		QIconSet(Resource::loadPixmap("gaim/16x16/blist"),
				 Resource::loadPixmap("gaim/32x32/blist")),
		QString::null, 0, this, 0, true);
	blistButton->setOn(true);
	blistButton->addTo(toolbar);

	connect(blistButton, SIGNAL(toggled(bool)),
			this, SLOT(blistToggled(bool)));

	/* Accounts */
	a = new QAction(tr("Accounts"),
					QIconSet(Resource::loadPixmap("gaim/16x16/accounts"),
							 Resource::loadPixmap("gaim/32x32/accounts")),
					QString::null, 0, this, 0);
	a->addTo(toolbar);

	connect(a, SIGNAL(activated()),
			this, SLOT(showAccountsWindow()));

	/* Conversations */
	button = new QQuailConvButton(toolbar, "conversations");
}

void
QQuailBListWindow::setBlist(PurpleBuddyList *list)
{
    buddylist->setBlist(list);
}

PurpleBuddyList *
QQuailBListWindow::getBlist() const
{
    return buddylist->getBlist();
}

void
QQuailBListWindow::accountSignedOn(PurpleAccount *account)
{
	PurpleBlistNode *gnode, *cnode;

	chatButton->setEnabled(true);
	addButton->setEnabled(true);
	addBuddyButton->setEnabled(true);
	addGroupButton->setEnabled(true);

	for (gnode = purple_get_blist()->root; gnode != NULL; gnode = gnode->next)
	{
        if (!PURPLE_BLIST_NODE_IS_GROUP(gnode))
			continue;

		for (cnode = gnode->child; cnode != NULL; cnode = cnode->next)
		{
			PurpleChat *chat;

            if (!PURPLE_BLIST_NODE_IS_CHAT(cnode))
				continue;

			chat = (PurpleChat *)cnode;

			if (chat->account != account)
				continue;

			if (purple_blist_node_get_bool(cnode, "qpe-autojoin"))
			{
				serv_join_chat(purple_account_get_connection(account),
							   chat->components);
			}
		}
	}
}

void
QQuailBListWindow::accountSignedOff(PurpleAccount *)
{
	if (purple_connections_get_all() == NULL)
	{
		imButton->setEnabled(false);
		chatButton->setEnabled(false);
		addButton->setEnabled(false);
		removeButton->setEnabled(false);
	}
}

void
QQuailBListWindow::updateNode(PurpleBlistNode *node)
{
	buddylist->updateNode(node);
}

void
QQuailBListWindow::reloadList()
{
	buddylist->reload(true);
}

/**************************************************************************
 * Slots
 **************************************************************************/
void
QQuailBListWindow::nodeChanged(QListWidgetItem *_item)
{
	QQuailBListItem *item;
	PurpleBlistNode *node;

	if (_item == NULL)
	{
		imButton->setIconSet(newImIconSet);
		chatButton->setIconSet(newChatIconSet);
		removeButton->setEnabled(false);
	}
	else
	{
		item = (QQuailBListItem *)_item;
		node = item->getBlistNode();

		imButton->setEnabled(true);
		chatButton->setEnabled(true);

        if (PURPLE_BLIST_NODE_IS_BUDDY(node) || PURPLE_BLIST_NODE_IS_CONTACT(node))
		{
			imButton->setIconSet(sendImIconSet);
			chatButton->setIconSet(newChatIconSet);
		}
        else if (PURPLE_BLIST_NODE_IS_CHAT(node))
		{
			imButton->setIconSet(newImIconSet);
			chatButton->setIconSet(openChatIconSet);
		}
		else
		{
			imButton->setIconSet(newImIconSet);
			chatButton->setIconSet(newChatIconSet);
		}

		removeButton->setEnabled(true);
	}
}

void
QQuailBListWindow::doubleClickList(QListWidgetItem *_item)
{
	QQuailBListItem *item;
	PurpleBlistNode *node;

	item = (QQuailBListItem *)_item;
	node = item->getBlistNode();

    if (PURPLE_BLIST_NODE_IS_BUDDY(node) || PURPLE_BLIST_NODE_IS_CONTACT(node))
		openImSlot();
    else if (PURPLE_BLIST_NODE_IS_CHAT(node))
		openChatSlot();
}

void
QQuailBListWindow::showAddBuddy(PurpleGroup *group)
{
	QQuailAddBuddyDialog *dialog;
	QQuailBListItem *item;
	PurpleBlistNode *node;

	if (group == NULL)
	{
		item = (QQuailBListItem *)buddylist->selectedItem();

		if (item != NULL)
		{
			node = item->getBlistNode();

            if (PURPLE_BLIST_NODE_IS_GROUP(node))
			{
				group = (PurpleGroup *)node;
			}
            else if (PURPLE_BLIST_NODE_IS_BUDDY(node) ||
                     PURPLE_BLIST_NODE_IS_CHAT(node))
			{
				group = ((PurpleGroup *)node->parent);
			}
		}
	}

	dialog = new QQuailAddBuddyDialog(this, "", true);

	if (group != NULL)
		dialog->setGroup(group->name);

	dialog->showMaximized();
}

void
QQuailBListWindow::showAddBuddy()
{
	showAddBuddy(NULL);
}

void
QQuailBListWindow::showAddChat(PurpleGroup *group)
{
	QQuailAddChatDialog *dialog;
	QQuailBListItem *item;
	PurpleBlistNode *node;

	if (group == NULL)
	{
		item = (QQuailBListItem *)buddylist->selectedItem();

		if (item != NULL)
		{
			node = item->getBlistNode();

            if (PURPLE_BLIST_NODE_IS_GROUP(node))
			{
				group = (PurpleGroup *)node;
			}
            else if (PURPLE_BLIST_NODE_IS_BUDDY(node) ||
                     PURPLE_BLIST_NODE_IS_CHAT(node))
			{
				group = ((PurpleGroup *)node->parent);
			}
		}
	}

	dialog = new QQuailAddChatDialog(this, "", true);

	if (group != NULL)
		dialog->setGroup(group->name);

	dialog->showMaximized();
}
void
QQuailBListWindow::showAddChat()
{
	showAddChat(NULL);
}

static void
addGroupCb(void *, const char *groupName)
{
	PurpleGroup *group;

	group = purple_group_new(groupName);
	purple_blist_add_group(group, NULL);
	purple_blist_save();
}

void
QQuailBListWindow::showAddGroup()
{
	purple_request_input(this, tr("Add Group"),
					   tr("Please enter the name of the group to be added."),
					   NULL,
					   NULL, FALSE, FALSE,
					   tr("Add"), G_CALLBACK(addGroupCb),
					   tr("Cancel"), NULL, NULL);
}

static void
removeBuddyCb(PurpleBuddy *buddy)
{
	PurpleGroup *group;
	GaimConversation *conv;
	QString name;

	if (buddy == NULL)
		return;

	group = purple_find_buddys_group(buddy);
	name = buddy->name;

	serv_remove_buddy(purple_account_get_connection(buddy->account), name,
					  group->name);
	purple_blist_remove_buddy(buddy);
	purple_blist_save();

	conv = purple_find_conversation(name);

	if (conv != NULL)
        purple_conversation_update(conv, PURPLE_CONV_UPDATE_REMOVE);
}

static void
removeContactCb(PurpleContact *contact)
{
	PurpleBlistNode *bnode, *cnode;
	PurpleGroup *group;

	if (contact == NULL)
		return;

	cnode = (PurpleBlistNode *)contact;
	group = (PurpleGroup *)cnode->parent;

	for (bnode = cnode->child; bnode != NULL; bnode = bnode->next)
	{
		PurpleBuddy *buddy = (PurpleBuddy *)bnode;

		if (purple_account_get_connection(buddy->account) != NULL)
		{
			serv_remove_buddy(purple_account_get_connection(buddy->account),
							  buddy->name, group->name);
		}
	}

	purple_blist_remove_contact(contact);
}

static void
removeChatCb(PurpleChat *chat)
{
	purple_blist_remove_chat(chat);
	purple_blist_save();
}

static void
removeGroupCb(PurpleGroup *group)
{
	PurpleBlistNode *node, *child;

	for (node = ((PurpleBlistNode *)group)->child;
		 node != NULL;
		 node = node->next)
	{
        if (PURPLE_BLIST_NODE_IS_CONTACT(node))
		{
			for (child = node->child; child != NULL; child = child->next)
			{
                if (PURPLE_BLIST_NODE_IS_BUDDY(child))
				{
					PurpleBuddy *buddy = (PurpleBuddy *)child;
					GaimConversation *conv;

					conv = purple_find_conversation(buddy->name);

					if (purple_account_is_connected(buddy->account))
					{
						serv_remove_buddy(purple_account_get_connection(
							buddy->account), buddy->name, group->name);
						purple_blist_remove_buddy(buddy);

						if (conv != NULL)
						{
							purple_conversation_update(conv,
                                PURPLE_CONV_UPDATE_REMOVE);
						}
					}
				}
			}
		}
        else if (PURPLE_BLIST_NODE_IS_CHAT(node))
		{
			PurpleChat *chat = (PurpleChat *)node;

			if (purple_account_is_connected(chat->account))
				purple_blist_remove_chat(chat);
		}
	}

	purple_blist_remove_group(group);
	purple_blist_save();
}

void
QQuailBListWindow::showRemoveBuddy()
{
	QQuailBListItem *item;
	PurpleBlistNode *node;

	item = (QQuailBListItem *)buddylist->selectedItem();
	node = item->getBlistNode();

    if (PURPLE_BLIST_NODE_IS_BUDDY(node))
	{
		showConfirmRemoveBuddy((PurpleBuddy *)node);
	}
    else if (PURPLE_BLIST_NODE_IS_CONTACT(node))
	{
		showConfirmRemoveContact((PurpleContact *)node);
	}
    else if (PURPLE_BLIST_NODE_IS_CHAT(node))
	{
		showConfirmRemoveChat((PurpleChat *)node);
	}
    else if (PURPLE_BLIST_NODE_IS_GROUP(node))
	{
		showConfirmRemoveGroup((PurpleGroup *)node);
	}
}

void
QQuailBListWindow::showConfirmRemoveBuddy(PurpleBuddy *buddy)
{
	QString name = buddy->name;

	int result = QMessageBox::information(this,
			tr("Remove Buddy"),
			tr("<p>You are about to remove %1 from your buddy list.</p>\n"
			   "<p>Do you want to continue?</p>").arg(name),
			tr("&Remove Buddy"), tr("&Cancel"),
			QString::null, 1, 1);

	if (result == 0)
		removeBuddyCb(buddy);
}

void
QQuailBListWindow::showConfirmRemoveContact(PurpleContact *contact)
{
	PurpleBuddy *buddy = purple_contact_get_priority_buddy(contact);

	if (buddy == NULL)
		return;

	if (((PurpleBlistNode *)contact)->child == (PurpleBlistNode *)buddy &&
		((PurpleBlistNode *)buddy)->next == NULL)
	{
		showConfirmRemoveBuddy(buddy);
	}
	else
	{
		int result = QMessageBox::information(this,
			tr("Remove Contact"),
			tr("<p>You are about to remove the contact containing "
			   "%1 and %2 other buddies from your buddy list.</p>\n"
			   "<p>Do you want to continue?</p>").arg(buddy->name,
													  contact->totalsize - 1),
			tr("&Remove Contact"), tr("&Cancel"),
			QString::null, 1, 1);

		if (result == 0)
			removeContactCb(contact);
	}
}

void
QQuailBListWindow::showConfirmRemoveChat(PurpleChat *chat)
{
	QString name = purple_chat_get_display_name(chat);

	int result = QMessageBox::information(this,
			tr("Remove Chat"),
			tr("<p>You are about to remove the chat %1 from "
			   "your buddy list.</p>\n"
			   "<p>Do you want to continue?</p>").arg(name),
			tr("&Remove Chat"), tr("&Cancel"),
			QString::null, 1, 1);

	if (result == 0)
		removeChatCb(chat);
}

void
QQuailBListWindow::showConfirmRemoveGroup(PurpleGroup *group)
{
	QString name = group->name;

	int result = QMessageBox::information(this,
			tr("Remove Group"),
			tr("<p>You are about to remove %1 and all its members from "
			   "your buddy list.</p>\n"
			   "<p>Do you want to continue?</p>").arg(name),
			tr("&Remove Group"), tr("&Cancel"),
			QString::null, 1, 1);

	if (result == 0)
		removeGroupCb(group);
}

void
QQuailBListWindow::showOfflineBuddies(bool on)
{
	purple_prefs_set_bool("/gaim/qpe/blist/show_offline_buddies", on);

	buddylist->reload(true);
}

void
QQuailBListWindow::showPreferencesSlot()
{
	QQuailPrefsDialog *prefsDialog = new QQuailPrefsDialog();

	prefsDialog->showMaximized();
}

void
QQuailBListWindow::showAccountsWindow()
{
	qQuailGetMainWindow()->showAccountsWindow();
}

void
QQuailBListWindow::blistToggled(bool)
{
	blistButton->setOn(true);
}

void
QQuailBListWindow::openImSlot(PurpleBuddy *buddy)
{
	if (buddy == NULL)
		buddy = buddylist->getSelectedBuddy();

	if (buddy != NULL)
	{
		GaimConversation *conv;
		GaimConvWindow *win;

        conv = purple_conversation_new(PURPLE_CONV_IM, buddy->account,
									 buddy->name);

		win = purple_conversation_get_window(conv);
		purple_conv_window_raise(win);

		purple_conv_window_switch_conversation(win,
				purple_conversation_get_index(conv));
	}
	else
	{
		QQuailNewImDialog *dialog;

		dialog = new QQuailNewImDialog(this);

		dialog->showMaximized();
	}
}

void
QQuailBListWindow::openImSlot()
{
	openImSlot(NULL);
}

void
QQuailBListWindow::openChatSlot(PurpleChat *chat)
{
	QQuailBListItem *item;
	PurpleBlistNode *node = NULL;

	if (chat == NULL)
	{
		item = (QQuailBListItem *)buddylist->selectedItem();

		if (item != NULL)
		{
			node = item->getBlistNode();

            if (PURPLE_BLIST_NODE_IS_CHAT(node))
				chat = (PurpleChat *)node;
		}
	}

	if (chat != NULL)
	{
		serv_join_chat(purple_account_get_connection(chat->account),
					   chat->components);
	}
	else
	{
		QQuailJoinChatDialog *dialog;

		dialog = new QQuailJoinChatDialog(this, "", true);

		dialog->showMaximized();
	}
}

void
QQuailBListWindow::openChatSlot()
{
	openChatSlot(NULL);
}

/**************************************************************************
 * Gaim callbacks
 **************************************************************************/
static void
signedOnCb(PurpleConnection *gc, PurpleBuddyList *blist)
{
	QQuailBListWindow *qblist = (QQuailBListWindow *)blist->ui_data;

	qblist->accountSignedOn(purple_connection_get_account(gc));
}

static void
signedOffCb(PurpleConnection *gc, PurpleBuddyList *blist)
{
	QQuailBListWindow *qblist = (QQuailBListWindow *)blist->ui_data;

	qblist->accountSignedOff(purple_connection_get_account(gc));
}

/**************************************************************************
 * blist UI
 **************************************************************************/
static void
qQuailBlistNewList(PurpleBuddyList *blist)
{
	QQuailBListWindow *win = qQuailGetMainWindow()->getBlistWindow();
	blist->ui_data = win;
	win->setGaimBlist(blist);

	/* Setup some signal handlers */
	purple_signal_connect(purple_connections_get_handle(), "signed-on",
                        blist->ui_data, PURPLE_CALLBACK(signedOnCb), blist);
	purple_signal_connect(purple_connections_get_handle(), "signed-off",
                        blist->ui_data, PURPLE_CALLBACK(signedOffCb), blist);
}

static void
qQuailBlistNewNode(PurpleBlistNode *)
{
}

static void
qQuailBlistShow(PurpleBuddyList *blist)
{
	QQuailBListWindow *blist_win = (QQuailBListWindow *)blist->ui_data;

	blist_win->show();
}

static void
qQuailBlistUpdate(PurpleBuddyList *blist, PurpleBlistNode *node)
{
	QQuailBListWindow *blist_win = (QQuailBListWindow *)blist->ui_data;
	blist_win->updateNode(node);
}

static void
qQuailBlistRemove(PurpleBuddyList *blist, PurpleBlistNode *node)
{
	QQuailBListWindow *blist_win = (QQuailBListWindow *)blist->ui_data;
	QQuailBListItem *item = (QQuailBListItem *)node->ui_data;
	QQuailBListItem *parent;

	if (item != NULL)
	{
		parent = (QQuailBListItem *)item->parent();

		delete item;

		if (parent != NULL)
			blist_win->updateNode(parent->getBlistNode());
	}

	node->ui_data = NULL;
}

static void
qQuailBlistDestroy(PurpleBuddyList *blist)
{
	QQuailBListWindow *qblist = (QQuailBListWindow *)blist->ui_data;

	delete qblist;

	blist->ui_data = NULL;
}

static void
qQuailBlistSetVisible(PurpleBuddyList *blist, gboolean show)
{
	QQuailBListWindow *qblist = (QQuailBListWindow *)blist->ui_data;

	if (show)
		qblist->show();
	else
		qblist->hide();
}

static GaimBlistUiOps blistUiOps =
{
	qQuailBlistNewList,
	qQuailBlistNewNode,
	qQuailBlistShow,
	qQuailBlistUpdate,
	qQuailBlistRemove,
	qQuailBlistDestroy,
	qQuailBlistSetVisible,
	NULL,
	NULL,
	NULL
};

GaimBlistUiOps *
qQuailGetBlistUiOps()
{
	return &blistUiOps;
}
