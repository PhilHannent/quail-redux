/**
 * @file QGaimBListWindow.cpp Buddy list window
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
#include "QGaimBListWindow.h"
#include "QGaimBuddyList.h"
#include "QGaimConvButton.h"
#include "QGaimConvWindow.h"
#include "QGaimDialogs.h"
#include "QGaim.h"
#include "base.h"

#include <libgaim/debug.h>
#include <libgaim/multi.h>
#include <libgaim/prefs.h>
#include <libgaim/request.h>
#include <libgaim/signals.h>

#include <qpe/resource.h>

#include <qaction.h>
#include <qheader.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlistview.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>


/**************************************************************************
 * QGaimBListWindow
 **************************************************************************/
QGaimBListWindow::QGaimBListWindow()
	: QMainWindow(), convsMenu(NULL)
{
	buildInterface();
}

QGaimBListWindow::~QGaimBListWindow()
{
	if (convsMenu != NULL)
		delete convsMenu;
}

void
QGaimBListWindow::buildInterface()
{
	setToolBarsMovable(FALSE);

	newChatIconSet  = Resource::loadPixmap("gaim/new-chat");
	openChatIconSet = Resource::loadPixmap("gaim/chat");
	newImIconSet    = Resource::loadPixmap("gaim/new-im");
	sendImIconSet   = Resource::loadPixmap("gaim/send-im");

	/* Setup the toolbar */
	buildToolBar();

	/* Setup the buddy list */
	buddylist = new QGaimBuddyList(this, "BuddyList");

	connect(buddylist, SIGNAL(currentChanged(QListViewItem *)),
			this, SLOT(nodeChanged(QListViewItem *)));
	connect(buddylist, SIGNAL(doubleClicked(QListViewItem *)),
			this, SLOT(doubleClickList(QListViewItem *)));
	connect(buddylist, SIGNAL(openIm(GaimBuddy *)),
			this, SLOT(openImSlot(GaimBuddy *)));

	setCentralWidget(buddylist);
}

void
QGaimBListWindow::buildToolBar()
{
	QAction *a;
	QLabel *label;
	QToolButton *button;

	toolbar = new QToolBar(this);
	toolbar->setHorizontalStretchable(TRUE);

	/* IM */
	a = new QAction(tr("Send IM"),
					QIconSet(Resource::loadPixmap("gaim/new-im")),
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
			this, SLOT(openChat()));

	toolbar->addSeparator();

	/* Add */
	button = new QToolButton(toolbar, "add");
	button->setAutoRaise(true);
	button->setPixmap(Resource::loadPixmap("gaim/add"));
	button->setEnabled(false);
	addButton = button;

	addMenu = new QPopupMenu(button);
	button->setPopup(addMenu);
	button->setPopupDelay(0);

	/* Add Buddy */
	a = new QAction(tr("Add Buddy"),
					QIconSet(Resource::loadPixmap("gaim/user")),
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
					QIconSet(Resource::loadPixmap("gaim/new-group")),
					QString::null, 0, this, 0);
	addGroupButton = a;
	a->addTo(addMenu);

	connect(a, SIGNAL(activated()),
			this, SLOT(showAddGroup()));

	/* Remove */
	a = new QAction(tr("Remove"),
					QIconSet(Resource::loadPixmap("gaim/remove")),
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
	button->setPixmap(Resource::loadPixmap("gaim/settings"));

	settingsMenu = new QPopupMenu(button);
	button->setPopup(settingsMenu);
	button->setPopupDelay(0);

	/* Show Offline Buddies */
	a = new QAction(tr("Show Offline Buddies"),
					QIconSet(Resource::loadPixmap("gaim/offline_buddies")),
					QString::null, 0, this, 0, true);
	a->setOn(gaim_prefs_get_bool("/gaim/qpe/blist/show_offline_buddies"));
	showOfflineButton = a;
	a->addTo(settingsMenu);

	connect(a, SIGNAL(toggled(bool)),
			this, SLOT(showOfflineBuddies(bool)));

	/* Add some whitespace. */
	label = new QLabel(toolbar);
	label->setText("");
	toolbar->setStretchableWidget(label);

	/* Now we're going to construct the toolbar on the right. */
	toolbar->addSeparator();

	/* Buddy List */
	blistButton = new QAction(tr("Buddy List"),
							  Resource::loadPixmap("gaim/blist"),
							  QString::null, 0, this, 0, true);
	blistButton->setOn(true);
	blistButton->addTo(toolbar);

	connect(blistButton, SIGNAL(toggled(bool)),
			this, SLOT(blistToggled(bool)));

	/* Accounts */
	a = new QAction(tr("Accounts"),
					Resource::loadPixmap("gaim/accounts"),
					QString::null, 0, this, 0);
	a->addTo(toolbar);

	connect(a, SIGNAL(activated()),
			this, SLOT(showAccountsWindow()));

	/* Conversations */
	button = new QGaimConvButton(toolbar, "conversations");
}

void
QGaimBListWindow::setGaimBlist(GaimBuddyList *list)
{
	buddylist->setGaimBlist(list);
}

GaimBuddyList *
QGaimBListWindow::getGaimBlist() const
{
	return buddylist->getGaimBlist();
}

void
QGaimBListWindow::accountSignedOn(GaimAccount *)
{
	chatButton->setEnabled(true);
	addButton->setEnabled(true);
	addBuddyButton->setEnabled(true);
	addGroupButton->setEnabled(true);
}

void
QGaimBListWindow::accountSignedOff(GaimAccount *)
{
	if (gaim_connections_get_all() == NULL)
	{
		imButton->setEnabled(false);
		chatButton->setEnabled(false);
		addButton->setEnabled(false);
		removeButton->setEnabled(false);
	}
}

void
QGaimBListWindow::updateNode(GaimBlistNode *node)
{
	buddylist->updateNode(node);
}

/**************************************************************************
 * Slots
 **************************************************************************/
void
QGaimBListWindow::nodeChanged(QListViewItem *_item)
{
	QGaimBListItem *item;
	GaimBlistNode *node;

	if (_item == NULL)
	{
		imButton->setIconSet(newImIconSet);
		chatButton->setIconSet(newChatIconSet);
		removeButton->setEnabled(false);
	}
	else
	{
		item = (QGaimBListItem *)_item;
		node = item->getBlistNode();

		imButton->setEnabled(true);
		chatButton->setEnabled(true);

		if (GAIM_BLIST_NODE_IS_BUDDY(node) || GAIM_BLIST_NODE_IS_CONTACT(node))
		{
			imButton->setIconSet(sendImIconSet);
			chatButton->setIconSet(newChatIconSet);
		}
		else if (GAIM_BLIST_NODE_IS_CHAT(node))
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
QGaimBListWindow::doubleClickList(QListViewItem *_item)
{
	QGaimBListItem *item;
	GaimBlistNode *node;

	item = (QGaimBListItem *)_item;
	node = item->getBlistNode();

	if (GAIM_BLIST_NODE_IS_BUDDY(node) || GAIM_BLIST_NODE_IS_CONTACT(node))
		openImSlot();
	else if (GAIM_BLIST_NODE_IS_CHAT(node))
		openChat();
}

void
QGaimBListWindow::showAddBuddy()
{
	QGaimAddBuddyDialog *dialog;
	QGaimBListItem *item;
	GaimBlistNode *node;
	GaimGroup *group = NULL;

	item = (QGaimBListItem *)buddylist->selectedItem();

	if (item != NULL)
	{
		node = item->getBlistNode();

		if (GAIM_BLIST_NODE_IS_GROUP(node))
		{
			group = (GaimGroup *)node;
		}
		else if (GAIM_BLIST_NODE_IS_BUDDY(node) ||
				 GAIM_BLIST_NODE_IS_CHAT(node))
		{
			group = ((GaimGroup *)node->parent);
		}
	}

	dialog = new QGaimAddBuddyDialog(this, "", true);

	if (group != NULL)
		dialog->setGroup(group->name);

	dialog->showMaximized();
}

void
QGaimBListWindow::showAddChat()
{
	QGaimAddChatDialog *dialog;
	QGaimBListItem *item;
	GaimBlistNode *node;
	GaimGroup *group = NULL;

	item = (QGaimBListItem *)buddylist->selectedItem();

	if (item != NULL)
	{
		node = item->getBlistNode();

		if (GAIM_BLIST_NODE_IS_GROUP(node))
		{
			group = (GaimGroup *)node;
		}
		else if (GAIM_BLIST_NODE_IS_BUDDY(node) ||
				 GAIM_BLIST_NODE_IS_CHAT(node))
		{
			group = ((GaimGroup *)node->parent);
		}
	}

	dialog = new QGaimAddChatDialog(this, "", true);

	if (group != NULL)
		dialog->setGroup(group->name);

	dialog->showMaximized();
}

static void
addGroupCb(void *, const char *groupName)
{
	GaimGroup *group;

	group = gaim_group_new(groupName);
	gaim_blist_add_group(group, NULL);
	gaim_blist_save();
}

void
QGaimBListWindow::showAddGroup()
{
	gaim_request_input(this, tr("Add Group"),
					   tr("Please enter the name of the group to be added."),
					   NULL,
					   NULL, FALSE, FALSE,
					   tr("Add"), G_CALLBACK(addGroupCb),
					   tr("Cancel"), NULL, NULL);
}

static void
removeBuddyCb(GaimBuddy *buddy)
{
	GaimGroup *group;
	GaimConversation *conv;
	QString name;

	if (buddy == NULL)
		return;

	group = gaim_find_buddys_group(buddy);
	name = buddy->name;

	serv_remove_buddy(gaim_account_get_connection(buddy->account), name,
					  group->name);
	gaim_blist_remove_buddy(buddy);
	gaim_blist_save();

	conv = gaim_find_conversation(name);

	if (conv != NULL)
		gaim_conversation_update(conv, GAIM_CONV_UPDATE_REMOVE);
}

static void
removeContactCb(GaimContact *contact)
{
	GaimBlistNode *bnode, *cnode;
	GaimGroup *group;

	if (contact == NULL)
		return;

	cnode = (GaimBlistNode *)contact;
	group = (GaimGroup *)cnode->parent;

	for (bnode = cnode->child; bnode != NULL; bnode = bnode->next)
	{
		GaimBuddy *buddy = (GaimBuddy *)bnode;

		if (gaim_account_get_connection(buddy->account) != NULL)
		{
			serv_remove_buddy(gaim_account_get_connection(buddy->account),
							  buddy->name, group->name);
		}
	}

	gaim_blist_remove_contact(contact);
}

static void
removeChatCb(GaimChat *chat)
{
	gaim_blist_remove_chat(chat);
	gaim_blist_save();
}

static void
removeGroupCb(GaimGroup *group)
{
	GaimBlistNode *node, *child;

	for (node = ((GaimBlistNode *)group)->child;
		 node != NULL;
		 node = node->next)
	{
		if (GAIM_BLIST_NODE_IS_CONTACT(node))
		{
			for (child = node->child; child != NULL; child = child->next)
			{
				if (GAIM_BLIST_NODE_IS_BUDDY(child))
				{
					GaimBuddy *buddy = (GaimBuddy *)child;
					GaimConversation *conv;

					conv = gaim_find_conversation(buddy->name);

					if (gaim_account_is_connected(buddy->account))
					{
						serv_remove_buddy(gaim_account_get_connection(
							buddy->account), buddy->name, group->name);
						gaim_blist_remove_buddy(buddy);

						if (conv != NULL)
						{
							gaim_conversation_update(conv,
								GAIM_CONV_UPDATE_REMOVE);
						}
					}
				}
			}
		}
		else if (GAIM_BLIST_NODE_IS_CHAT(node))
		{
			GaimChat *chat = (GaimChat *)node;

			if (gaim_account_is_connected(chat->account))
				gaim_blist_remove_chat(chat);
		}
	}

	gaim_blist_remove_group(group);
	gaim_blist_save();
}

void
QGaimBListWindow::showRemoveBuddy()
{
	QGaimBListItem *item;
	GaimBlistNode *node;

	item = (QGaimBListItem *)buddylist->selectedItem();
	node = item->getBlistNode();

	if (GAIM_BLIST_NODE_IS_BUDDY(node))
	{
		showConfirmRemoveBuddy((GaimBuddy *)node);
	}
	else if (GAIM_BLIST_NODE_IS_CONTACT(node))
	{
		showConfirmRemoveContact((GaimContact *)node);
	}
	else if (GAIM_BLIST_NODE_IS_CHAT(node))
	{
		showConfirmRemoveChat((GaimChat *)node);
	}
	else if (GAIM_BLIST_NODE_IS_GROUP(node))
	{
		showConfirmRemoveGroup((GaimGroup *)node);
	}
}

void
QGaimBListWindow::showConfirmRemoveBuddy(GaimBuddy *buddy)
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
QGaimBListWindow::showConfirmRemoveContact(GaimContact *contact)
{
	GaimBuddy *buddy = gaim_contact_get_priority_buddy(contact);

	if (buddy == NULL)
		return;

	if (((GaimBlistNode *)contact)->child == (GaimBlistNode *)buddy &&
		((GaimBlistNode *)buddy)->next == NULL)
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
QGaimBListWindow::showConfirmRemoveChat(GaimChat *chat)
{
	QString name = gaim_chat_get_display_name(chat);

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
QGaimBListWindow::showConfirmRemoveGroup(GaimGroup *group)
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
QGaimBListWindow::showOfflineBuddies(bool on)
{
	gaim_prefs_set_bool("/gaim/qpe/blist/show_offline_buddies", on);

	buddylist->reload(true);
}

void
QGaimBListWindow::showAccountsWindow()
{
	qGaimGetHandle()->showAccountsWindow();
}

void
QGaimBListWindow::blistToggled(bool)
{
	blistButton->setOn(true);
}

void
QGaimBListWindow::openImSlot(GaimBuddy *buddy)
{
	if (buddy == NULL)
		buddy = buddylist->getSelectedBuddy();

	if (buddy != NULL)
	{
		GaimConversation *conv;
		GaimConvWindow *win;

		conv = gaim_conversation_new(GAIM_CONV_IM, buddy->account,
									 buddy->name);

		win = gaim_conversation_get_window(conv);
		gaim_conv_window_raise(win);

		gaim_conv_window_switch_conversation(win,
				gaim_conversation_get_index(conv));
	}
	else
	{
		QGaimNewImDialog *dialog;

		dialog = new QGaimNewImDialog(this);

		dialog->showMaximized();
	}
}

void
QGaimBListWindow::openImSlot()
{
	openImSlot(NULL);
}

void
QGaimBListWindow::openChat()
{
	QGaimBListItem *item;
	GaimBlistNode *node;

	item = (QGaimBListItem *)buddylist->selectedItem();

	if (item == NULL)
		return;

	node = item->getBlistNode();

	if (GAIM_BLIST_NODE_IS_CHAT(node))
	{
		GaimChat *chat = (GaimChat *)item->getBlistNode();

		serv_join_chat(gaim_account_get_connection(chat->account),
					   chat->components);
	}
	else
	{
		QGaimJoinChatDialog *dialog;

		dialog = new QGaimJoinChatDialog(this, "", true);

		dialog->showMaximized();
	}
}


/**************************************************************************
 * Gaim callbacks
 **************************************************************************/
static void
signedOnCb(GaimConnection *gc, GaimBuddyList *blist)
{
	QGaimBListWindow *qblist = (QGaimBListWindow *)blist->ui_data;

	qblist->accountSignedOn(gaim_connection_get_account(gc));
}

static void
signedOffCb(GaimConnection *gc, GaimBuddyList *blist)
{
	QGaimBListWindow *qblist = (QGaimBListWindow *)blist->ui_data;

	qblist->accountSignedOff(gaim_connection_get_account(gc));
}

/**************************************************************************
 * blist UI
 **************************************************************************/
static void
qGaimBlistNewList(GaimBuddyList *blist)
{
	QGaimBListWindow *win = qGaimGetHandle()->getBlistWindow();
	blist->ui_data = win;
	win->setGaimBlist(blist);

	/* Setup some signal handlers */
	gaim_signal_connect(gaim_connections_get_handle(), "signed-on",
						blist->ui_data, GAIM_CALLBACK(signedOnCb), blist);
	gaim_signal_connect(gaim_connections_get_handle(), "signed-off",
						blist->ui_data, GAIM_CALLBACK(signedOffCb), blist);
}

static void
qGaimBlistNewNode(GaimBlistNode *)
{
}

static void
qGaimBlistShow(GaimBuddyList *blist)
{
	QGaimBListWindow *blist_win = (QGaimBListWindow *)blist->ui_data;

	blist_win->show();
}

static void
qGaimBlistUpdate(GaimBuddyList *blist, GaimBlistNode *node)
{
	QGaimBListWindow *blist_win = (QGaimBListWindow *)blist->ui_data;
	blist_win->updateNode(node);
}

static void
qGaimBlistRemove(GaimBuddyList *, GaimBlistNode *node)
{
	QListViewItem *parent;
	QGaimBListItem *item = (QGaimBListItem *)node->ui_data;

	if (item != NULL)
	{
		parent = item->parent();

		delete item;

		if (parent != NULL && parent->childCount() == 0)
			delete parent;
	}

	node->ui_data = NULL;
}

static void
qGaimBlistDestroy(GaimBuddyList *blist)
{
	QGaimBListWindow *qblist = (QGaimBListWindow *)blist->ui_data;

	delete qblist;

	blist->ui_data = NULL;
}

static void
qGaimBlistSetVisible(GaimBuddyList *blist, gboolean show)
{
	QGaimBListWindow *qblist = (QGaimBListWindow *)blist->ui_data;

	if (show)
		qblist->show();
	else
		qblist->hide();
}

static GaimBlistUiOps blistUiOps =
{
	qGaimBlistNewList,
	qGaimBlistNewNode,
	qGaimBlistShow,
	qGaimBlistUpdate,
	qGaimBlistRemove,
	qGaimBlistDestroy,
	qGaimBlistSetVisible,
	NULL,
	NULL,
	NULL
};

GaimBlistUiOps *
qGaimGetBlistUiOps()
{
	return &blistUiOps;
}
