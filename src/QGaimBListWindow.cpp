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
#include <libgaim/request.h>

#include <qpe/resource.h>

#include <qaction.h>
#include <qheader.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlistview.h>
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

	/* Setup the toolbar */
	buildToolBar();

	/* Setup the buddy list */
	buddylist = new QGaimBuddyList(this, "BuddyList");

	connect(buddylist, SIGNAL(currentChanged(QListViewItem *)),
			this, SLOT(nodeChanged(QListViewItem *)));
	connect(buddylist, SIGNAL(doubleClicked(QListViewItem *)),
			this, SLOT(sendIm()));

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
					QIconSet(Resource::loadPixmap("gaim/send-im")),
					QString::null, 0, this, 0);
	imButton = a;
	a->addTo(toolbar);
	a->setEnabled(false);

	connect(a, SIGNAL(activated()),
			this, SLOT(sendIm()));

	/* Chat */
	/* TODO */

	/* Get User Info */
	a = new QAction(tr("Get User Information"),
					QIconSet(Resource::loadPixmap("gaim/info")),
					QString::null, 0, this, 0);
	infoButton = a;
	a->addTo(toolbar);
	a->setEnabled(false);

	toolbar->addSeparator();

	/* Add Buddy */
	a = new QAction(tr("Add User"),
					QIconSet(Resource::loadPixmap("gaim/add")),
					QString::null, 0, this, 0);
	addBuddyButton = a;
	a->addTo(toolbar);
	a->setEnabled(false);

	connect(a, SIGNAL(activated()),
			this, SLOT(showAddBuddy()));

	/* New Group */
	a = new QAction(tr("New Group"),
					QIconSet(Resource::loadPixmap("gaim/new-group")),
					QString::null, 0, this, 0);
	addGroupButton = a;
	a->addTo(toolbar);
	a->setEnabled(false);

	/* Remove */
	a = new QAction(tr("Remove"),
					QIconSet(Resource::loadPixmap("gaim/remove")),
					QString::null, 0, this, 0);
	removeButton = a;
	a->addTo(toolbar);
	a->setEnabled(false);

	connect(a, SIGNAL(activated()),
			this, SLOT(showRemoveBuddy()));

	/* Add some whitespace. */
	label = new QLabel(toolbar);
	label->setText("");
	toolbar->setStretchableWidget(label);

	/* Now we're going to construct the toolbar on the right. */
	toolbar->addSeparator();

	/* Buddy List */
	blistButton = button = new QToolButton(toolbar, "blist");
	button->setAutoRaise(true);
	button->setPixmap(Resource::loadPixmap("gaim/blist"));
	button->setToggleButton(true);
	button->setOn(true);

	connect(button, SIGNAL(toggled(bool)),
			this, SLOT(blistToggled(bool)));

	/* Accounts */
	button = new QToolButton(toolbar, "accounts");
	button->setAutoRaise(true);
	button->setPixmap(Resource::loadPixmap("gaim/accounts"));

	connect(button, SIGNAL(clicked()),
			this, SLOT(showAccountsWindow()));

	/* Conversations */
	button = new QGaimConvButton(toolbar, "conversations");
}

void
QGaimBListWindow::setGaimBlist(struct gaim_buddy_list *list)
{
	buddylist->setGaimBlist(list);
}

struct gaim_buddy_list *
QGaimBListWindow::getGaimBlist() const
{
	return buddylist->getGaimBlist();
}

void
QGaimBListWindow::accountSignedOn(GaimAccount *)
{
	addBuddyButton->setEnabled(true);
}

void
QGaimBListWindow::accountSignedOff(GaimAccount *)
{
	if (gaim_connections_get_all() == NULL)
		addBuddyButton->setEnabled(false);
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
		imButton->setEnabled(false);
	}
	else
	{
		item = (QGaimBListItem *)_item;
		node = item->getBlistNode();

		if (GAIM_BLIST_NODE_IS_BUDDY(node))
		{
			imButton->setEnabled(true);
		}
		else
		{
			imButton->setEnabled(false);
		}
	}
}

void
QGaimBListWindow::im()
{
}

void
QGaimBListWindow::chat()
{
}

void
QGaimBListWindow::showAddBuddy()
{
	QGaimAddBuddyDialog *dialog;
	QGaimBListItem *item;
	GaimBlistNode *node;
	struct group *group = NULL;

	item = (QGaimBListItem *)buddylist->selectedItem();

	if (item != NULL)
	{
		node = item->getBlistNode();

		if (GAIM_BLIST_NODE_IS_GROUP(node))
		{
			group = (struct group *)node;
		}
		else if (GAIM_BLIST_NODE_IS_BUDDY(node) ||
				 GAIM_BLIST_NODE_IS_CHAT(node))
		{
			group = ((struct group *)node->parent);
		}
	}

	dialog = new QGaimAddBuddyDialog(this, "", true);

	if (group != NULL)
		dialog->setGroup(group->name);

	dialog->showMaximized();
}

static void
removeBuddyCb(struct buddy *buddy)
{
	struct group *group;
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

void
QGaimBListWindow::showRemoveBuddy()
{
	QGaimBListItem *item;
	GaimBlistNode *node;

	item = (QGaimBListItem *)buddylist->selectedItem();
	node = item->getBlistNode();

	if (GAIM_BLIST_NODE_IS_BUDDY(node))
	{
		struct buddy *buddy = (struct buddy *)item->getBlistNode();
		QString text;
		QString name = buddy->name;

		text = tr("You are about to remove " + name + "from your buddy list.\n"
				  "Do you want to continue?");

		gaim_request_action(this, tr("Remove Buddy"), text, NULL,
							(size_t)-1, buddy, 2,
							(const char *)tr("Yes"),
							G_CALLBACK(removeBuddyCb),
							(const char *)tr("No"), NULL);
	}
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
QGaimBListWindow::sendIm()
{
	QGaimBListItem *item;
	GaimBlistNode *node;

	item = (QGaimBListItem *)buddylist->selectedItem();
	node = item->getBlistNode();

	if (GAIM_BLIST_NODE_IS_BUDDY(node))
	{
		struct buddy *buddy = (struct buddy *)item->getBlistNode();
		GaimConversation *conv;
		GaimWindow *win;

		conv = gaim_conversation_new(GAIM_CONV_IM, buddy->account,
									 buddy->name);

		win = gaim_conversation_get_window(conv);
		gaim_window_raise(win);

		gaim_window_switch_conversation(win, gaim_conversation_get_index(conv));
	}
}


/**************************************************************************
 * Gaim callbacks
 **************************************************************************/
static void
signedOnCb(GaimConnection *gc, struct gaim_buddy_list *blist)
{
	QGaimBListWindow *qblist = (QGaimBListWindow *)blist->ui_data;

	qblist->accountSignedOn(gaim_connection_get_account(gc));
}

static void
signedOffCb(GaimConnection *gc, struct gaim_buddy_list *blist)
{
	QGaimBListWindow *qblist = (QGaimBListWindow *)blist->ui_data;

	qblist->accountSignedOff(gaim_connection_get_account(gc));
}

/**************************************************************************
 * blist UI
 **************************************************************************/
static void
qGaimBlistNewList(struct gaim_buddy_list *blist)
{
	QGaimBListWindow *win = qGaimGetHandle()->getBlistWindow();
	blist->ui_data = win;
	win->setGaimBlist(blist);

	/* Setup some signal handlers */
	gaim_signal_connect(blist->ui_data, event_signon,
						(void *)signedOnCb, blist);
	gaim_signal_connect(blist->ui_data, event_signoff,
						(void *)signedOffCb, blist);
}

static void
qGaimBlistNewNode(GaimBlistNode *)
{
}

static void
qGaimBlistShow(struct gaim_buddy_list *blist)
{
	QGaimBListWindow *blist_win = (QGaimBListWindow *)blist->ui_data;

	blist_win->show();
}

static void
qGaimBlistUpdate(struct gaim_buddy_list *blist, GaimBlistNode *node)
{
	QGaimBListWindow *blist_win = (QGaimBListWindow *)blist->ui_data;
	blist_win->updateNode(node);
}

static void
qGaimBlistRemove(struct gaim_buddy_list *, GaimBlistNode *node)
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
qGaimBlistDestroy(struct gaim_buddy_list *blist)
{
	QGaimBListWindow *qblist = (QGaimBListWindow *)blist->ui_data;

	delete qblist;

	blist->ui_data = NULL;
}

static void
qGaimBlistSetVisible(struct gaim_buddy_list *blist, gboolean show)
{
	QGaimBListWindow *qblist = (QGaimBListWindow *)blist->ui_data;

	if (show)
		qblist->show();
	else
		qblist->hide();
}

static struct gaim_blist_ui_ops blistUiOps =
{
	qGaimBlistNewList,
	qGaimBlistNewNode,
	qGaimBlistShow,
	qGaimBlistUpdate,
	qGaimBlistRemove,
	qGaimBlistDestroy,
	qGaimBlistSetVisible
};

struct gaim_blist_ui_ops *
qGaimGetBlistUiOps()
{
	return &blistUiOps;
}
