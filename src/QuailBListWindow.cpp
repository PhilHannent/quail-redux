/**
 * @file QQuailBListWindow.cpp Buddy list window
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
#include "QuailAccountsWindow.h"
#include "QuailBListWindow.h"
#include "QuailBlistItem.h"
#include "QuailBuddyList.h"
#include "QuailConvButton.h"
#include "QuailConvWindow.h"
#include "QuailDialogs.h"
#include "QuailMainWindow.h"
#include "QuailStatusSelector.h"

#include <libpurple/debug.h>
#include <libpurple/prefs.h>
#include <libpurple/request.h>
#include <libpurple/signals.h>

#include <QAction>
#include <QDebug>
#include <QLayout>
#include <QMessageBox>
#include <QMenu>
#include <QPushButton>
#include <QToolBar>
#include <QToolButton>

quail_blist_window::quail_blist_window(QMainWindow *parent)
	: QMainWindow(), parentMainWindow(parent), convsMenu(NULL)
{
    qDebug() << "QQuailBListWindow::QQuailBListWindow";
	buildInterface();
}

quail_blist_window::~quail_blist_window()
{
	if (convsMenu != NULL)
		delete convsMenu;
}

void
quail_blist_window::buildInterface()
{
    qDebug() << "QQuailBListWindow::buildInterface";
    setWindowIcon(QIcon(":/data/images/logo.png"));
    newChatIconSet = QIcon(QPixmap(":/data/images/actions/new-chat.png"));

    openChatIconSet = QIcon(QPixmap(":/data/images/actions/chat.png"));
    newImIconSet = QIcon(QPixmap(":/data/images/actions/new-im.png"));
    sendImIconSet = QIcon(QPixmap(":/data/images/actions/send-im.png"));

	/* Setup the toolbar */
	buildToolBar();

	/* Setup the buddy list */
    buddylist = new quail_buddy_list(this);
    connect(buddylist, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            this, SLOT(nodeChanged(QTreeWidgetItem*, int)));

    connect(buddylist, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem*)),
            this, SLOT(nodeChanged(QTreeWidgetItem *, QTreeWidgetItem *)));

    connect(buddylist, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),
            this, SLOT(doubleClickList(QTreeWidgetItem *, int)));

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

    QVBoxLayout *vbox = new QVBoxLayout(this);
    vbox->addWidget(buddylist);
    quail_status_selector *statusSelector = new quail_status_selector(this);
    vbox->addWidget(statusSelector);

    QWidget *window = new QWidget(this);
    window->setLayout(vbox);
    setCentralWidget(window);
}

void
quail_blist_window::buildToolBar()
{
    qDebug() << "QQuailBListWindow::buildToolBar";
	QAction *a;
	QToolButton *button;
	toolbar = new QToolBar(this);
    //toolbar->setMovable(false);

//    metersButton = new QAction(this);
//    metersButton->setText("Connection Meters");
//    toolbar->addAction(metersButton);
//    connect(metersButton, SIGNAL(triggered()), parentMainWindow, SLOT(slot_show_meters()));

    /* Buddy List */
    blistButton = new QAction(QIcon(QPixmap(":/data/images/actions/blist.png")),
                              tr("Buddy List"),
                              this);
    blistButton->setChecked(true);
    toolbar->addAction(blistButton);
    connect(blistButton, SIGNAL(toggled(bool)),
            this, SLOT(blistToggled(bool)));

    /* Accounts */
    a = new QAction(QIcon(QPixmap(":/data/images/actions/accounts.png")),
                    tr("Accounts"),
                    this);
    toolbar->addAction(a);
    connect(a, SIGNAL(triggered(bool)),
            parentMainWindow, SLOT(showAccountsWindow()));

    /* Conversations */
    a = new QAction(QIcon(QPixmap(":/data/images/actions/conversations.png")),
                    tr("Conversations"),
                    this);
    toolbar->addAction(a);
    connect(a, SIGNAL(triggered(bool)),
            parentMainWindow, SLOT(showConvWindow()));

    /* Now we're going to construct the toolbar on the right. */
    toolbar->addSeparator();

    /* IM */
    imButton = new QAction(QIcon(QPixmap(":/data/images/actions/new-im.png")),
                    tr("Send IM"),
                    this);
    toolbar->addAction(imButton);
    imButton->setEnabled(false);

    connect(imButton, SIGNAL(triggered(bool)),
			this, SLOT(openImSlot()));

	/* Chat */
    chatButton = new QAction(newChatIconSet, tr("Open Chat"), this);
    toolbar->addAction(chatButton);
    chatButton->setEnabled(false);

    connect(chatButton, SIGNAL(triggered(bool)),
			this, SLOT(openChatSlot()));

	toolbar->addSeparator();

	/* Add */
    button = new QToolButton(toolbar);
	button->setAutoRaise(true);
    button->setIcon(QIcon(QPixmap(":/data/images/actions/add.png")));
    button->setEnabled(true);
    button->setPopupMode(QToolButton::InstantPopup);
	addButton = button;
    toolbar->addWidget(addButton);

    addMenu = new QMenu(button);
    button->setMenu(addMenu);

	/* Add Buddy */
    addBuddyButton = new QAction(QIcon(QPixmap(":/data/images/actions/user.png")),
                                 tr("Add Buddy"),
                                 this);
    addMenu->addAction(addBuddyButton);
    connect(addBuddyButton, SIGNAL(triggered(bool)),
			this, SLOT(showAddBuddy()));

	/* Add Chat */
    addChatButton = new QAction(newChatIconSet, tr("Add Chat"), this);
    addMenu->addAction(addChatButton);
    connect(addChatButton, SIGNAL(triggered(bool)),
			this, SLOT(showAddChat()));

	/* Add Group */
    addGroupButton = new QAction(QIcon(QPixmap(":/data/images/actions/new-group.png")),
                                 tr("Add Group"),
                                 this);
    addMenu->addAction(addGroupButton);
    connect(addGroupButton, SIGNAL(triggered(bool)),
			this, SLOT(showAddGroup()));

	/* Remove */
    removeButton = new QAction(QIcon(QPixmap(":/data/images/actions/remove.png")),
                               tr("Remove"),
                               this);
    toolbar->addAction(removeButton);
    removeButton->setEnabled(true);
    connect(removeButton, SIGNAL(triggered(bool)),
			this, SLOT(showRemoveBuddy()));

	toolbar->addSeparator();

	/* Settings menu */
    button = new QToolButton(toolbar);
    button->setAutoRaise(true);
    button->setIcon(QIcon(QPixmap(":/data/images/actions/settings.png")));
    settingsMenu = new QMenu(this);
    button->setMenu(settingsMenu);
    button->setPopupMode(QToolButton::InstantPopup);
    toolbar->addWidget(button);

	/* Show Offline Buddies */
    showOfflineButton = new QAction(QIcon(QPixmap(":/data/images/actions/offline_buddies.png")),
                                    tr("Show Offline Buddies"),
                                    this);
    showOfflineButton->setChecked(purple_prefs_get_bool("/quail/blist/show_offline_buddies"));
    settingsMenu->addAction(showOfflineButton);
    connect(showOfflineButton, SIGNAL(toggled(bool)),
			this, SLOT(showOfflineBuddies(bool)));

	/* Separator */
    settingsMenu->addSeparator();

	/* Preferences */
    a = new QAction(tr("Preferences"), this);
    settingsMenu->addAction(a);
    connect(a, SIGNAL(triggered(bool)),
            parentMainWindow, SLOT(showPrefWindow()));

    this->addToolBar(toolbar);
    qDebug() << "QQuailBListWindow::buildToolBar.end";
}

void
quail_blist_window::setBlist(PurpleBuddyList *list)
{
    buddylist->setBlist(list);
}

PurpleBuddyList *
quail_blist_window::getBlist() const
{
    return buddylist->getBlist();
}

void
quail_blist_window::accountSignedOn(PurpleAccount *account)
{
    qDebug() << "QQuailBListWindow::accountSignedOn";
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
quail_blist_window::accountSignedOff(PurpleAccount *)
{
    qDebug() << "QQuailBListWindow::accountSignedOff";
    if (purple_connections_get_all() == NULL)
	{
		imButton->setEnabled(false);
		chatButton->setEnabled(false);
		addButton->setEnabled(false);
		removeButton->setEnabled(false);
	}
}

void
quail_blist_window::updateNode(PurpleBlistNode *node)
{
    qDebug() << "QQuailBListWindow::updateNode";
    buddylist->updateNode(node);
}

void
quail_blist_window::reloadList()
{
    qDebug() << "QQuailBListWindow::reloadList";
    buddylist->reload(true);
}

/**************************************************************************
 * Slots
 **************************************************************************/
void
quail_blist_window::currentNodeChanged(QTreeWidgetItem *item,
                                      QTreeWidgetItem *prevItem)
{
    nodeChanged(item, prevItem);
}

void
quail_blist_window::nodeChanged(QTreeWidgetItem *_item, QTreeWidgetItem * /*prev*/)
{
    qDebug() << "QQuailBListWindow::nodeChanged";
    quail_blist_item *item;
	PurpleBlistNode *node;

	if (_item == NULL)
	{
        imButton->setIcon(newImIconSet);
        chatButton->setIcon(newChatIconSet);
		removeButton->setEnabled(false);
	}
	else
	{
        item = (quail_blist_item *)_item;
		node = item->getBlistNode();

		imButton->setEnabled(true);
		chatButton->setEnabled(true);

        if (PURPLE_BLIST_NODE_IS_BUDDY(node) || PURPLE_BLIST_NODE_IS_CONTACT(node))
		{
            imButton->setIcon(sendImIconSet);
            chatButton->setIcon(newChatIconSet);
		}
        else if (PURPLE_BLIST_NODE_IS_CHAT(node))
		{
            imButton->setIcon(newImIconSet);
            chatButton->setIcon(openChatIconSet);
		}
		else
		{
            imButton->setIcon(newImIconSet);
            chatButton->setIcon(newChatIconSet);
		}

		removeButton->setEnabled(true);
	}
}

void
quail_blist_window::doubleClickList(QTreeWidgetItem *_item, int /*col*/)
{
    qDebug() << "QQuailBListWindow::doubleClickList";
    quail_blist_item *item;
	PurpleBlistNode *node;

    item = (quail_blist_item *)_item;
	node = item->getBlistNode();

    if (PURPLE_BLIST_NODE_IS_BUDDY(node) || PURPLE_BLIST_NODE_IS_CONTACT(node))
		openImSlot();
    else if (PURPLE_BLIST_NODE_IS_CHAT(node))
		openChatSlot();
}

void
quail_blist_window::showAddBuddy(PurpleGroup *group)
{
    qDebug() << "QQuailBListWindow::showAddBuddy";
    quail_add_buddy_dialog *dialog;
    quail_blist_item *item;
	PurpleBlistNode *node;

	if (group == NULL)
	{
        item = (quail_blist_item *)buddylist->currentItem();

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

    dialog = new quail_add_buddy_dialog(this);

	if (group != NULL)
		dialog->setGroup(group->name);

    dialog->show();
}

void
quail_blist_window::showAddBuddy()
{
	showAddBuddy(NULL);
}

void
quail_blist_window::showAddChat(PurpleGroup *group)
{
    qDebug() << "QQuailBListWindow::showAddChat()";
    quail_add_chat_dialog *dialog;
    quail_blist_item *item;
	PurpleBlistNode *node;

	if (group == NULL)
	{
        item = (quail_blist_item *)buddylist->currentItem();

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

    dialog = new quail_add_chat_dialog(this);

	if (group != NULL)
		dialog->setGroup(group->name);
    dialog->show();

}
void
quail_blist_window::showAddChat()
{
	showAddChat(NULL);
}

static void
addGroupCb(void *, const char *groupName)
{
    qDebug() << "QQuailBListWindow::addGroupCb()";
	PurpleGroup *group;

	group = purple_group_new(groupName);
	purple_blist_add_group(group, NULL);
    //TODO: See if this is required any more
    //purple_blist_save();
}

void
quail_blist_window::showAddGroup()
{
    qDebug() << "QQuailBListWindow::showAddGroup()";
    purple_request_input(this, tr("Add Group").toStdString().c_str(),
                       tr("Please enter the name of the group to be added.").toStdString().c_str(),
					   NULL,
                       NULL, FALSE, FALSE, NULL,
                       tr("Add").toStdString().c_str(), G_CALLBACK(addGroupCb),
                       tr("Cancel").toStdString().c_str(), NULL,
                       NULL, NULL, NULL,
                       NULL);
}

static void
removeBuddyCb(PurpleBuddy *buddy)
{
    qDebug() << "QQuailBListWindow::removeBuddyCb()";
    if (buddy == NULL)
        return;

    PurpleBlistNode* node = PURPLE_BLIST_NODE(buddy);
    if (PURPLE_BLIST_NODE_IS_BUDDY(node)) {
        purple_blist_remove_buddy((PurpleBuddy*)node);
    } else if (PURPLE_BLIST_NODE_IS_CHAT(node)) {
        purple_blist_remove_chat((PurpleChat*)node);
    } else if (PURPLE_BLIST_NODE_IS_GROUP(node)) {
        purple_blist_remove_group((PurpleGroup*)node);
    } else if (PURPLE_BLIST_NODE_IS_CONTACT(node)) {
        purple_blist_remove_contact((PurpleContact*)node);
    }

}

static void
removeContactCb(PurpleContact *contact)
{
    qDebug() << "QQuailBListWindow::removeContactCb()";
	PurpleBlistNode *bnode, *cnode;
//	PurpleGroup *group;

	if (contact == NULL)
		return;

	cnode = (PurpleBlistNode *)contact;
//	group = (PurpleGroup *)cnode->parent;

	for (bnode = cnode->child; bnode != NULL; bnode = bnode->next)
	{
		PurpleBuddy *buddy = (PurpleBuddy *)bnode;

		if (purple_account_get_connection(buddy->account) != NULL)
		{
//			serv_remove_buddy(purple_account_get_connection(buddy->account),
//							  buddy->name, group->name);
		}
	}

	purple_blist_remove_contact(contact);
}

static void
removeChatCb(PurpleChat *chat)
{
    qDebug() << "QQuailBListWindow::removeChatCb()";
	purple_blist_remove_chat(chat);
}

static void
removeGroupCb(PurpleGroup *group)
{
    qDebug() << "QQuailBListWindow::removeGroupCb()";
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
                    PurpleConversation *conv;

                    conv = purple_find_conversation_with_account(PURPLE_CONV_TYPE_CHAT,
                                                                 buddy->name,
                                                                 buddy->account);

					if (purple_account_is_connected(buddy->account))
					{
//						serv_remove_buddy(purple_account_get_connection(
//							buddy->account), buddy->name, group->name);
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
}

void
quail_blist_window::showRemoveBuddy()
{
    qDebug() << "QQuailBListWindow::showRemoveBuddy()";
    quail_blist_item *item;
	PurpleBlistNode *node;

    item = (quail_blist_item *)buddylist->currentItem();
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
quail_blist_window::showConfirmRemoveBuddy(PurpleBuddy *buddy)
{
    qDebug() << "QQuailBListWindow::showConfirmRemoveBuddy()";
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
quail_blist_window::showConfirmRemoveContact(PurpleContact *contact)
{
    qDebug() << "QQuailBListWindow::showConfirmRemoveContact()";
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
quail_blist_window::showConfirmRemoveChat(PurpleChat *chat)
{
    qDebug() << "QQuailBListWindow::showConfirmRemoveChat()";
    QString name = purple_chat_get_name(chat);

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
quail_blist_window::showConfirmRemoveGroup(PurpleGroup *group)
{
    qDebug() << "QQuailBListWindow::showConfirmRemoveGroup()";
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
quail_blist_window::showOfflineBuddies(bool on)
{
    qDebug() << "QQuailBListWindow::showOfflineBuddies()";
    purple_prefs_set_bool("/quail/blist/show_offline_buddies", on);

	buddylist->reload(true);
}

void
quail_blist_window::blistToggled(bool )
{
    qDebug() << "QQuailBListWindow::blistToggled()";
    //TODO: Check to see if this should toggle
    blistButton->setChecked(true);
}

void
quail_blist_window::openImSlot(PurpleBuddy *buddy)
{
    qDebug() << "QQuailBListWindow::openImSlot()";
	if (buddy == NULL)
		buddy = buddylist->getSelectedBuddy();

	if (buddy != NULL)
	{
        qDebug() << "QQuailBListWindow::openImSlot().1";
        PurpleConversation *conv;
//        QQuailConvWindow *win;

        conv = purple_conversation_new(PURPLE_CONV_TYPE_IM, buddy->account,
                                     buddy->name);

        purple_conversation_present(conv);
//        win = purple_conversation_get_window(conv);
//        win->raise();
//        purple_conv_window_raise(win);

//        purple_conv_window_switch_conversation(win,
//                purple_conversation_get_index(conv));
	}
	else
	{
        qDebug() << "QQuailBListWindow::openImSlot().1";
        quail_new_im_dialog *dialog;

        dialog = new quail_new_im_dialog(this);

        dialog->show();
	}
    qDebug() << "QQuailBListWindow::openImSlot().end";
}

void
quail_blist_window::openImSlot()
{
	openImSlot(NULL);
}

void
quail_blist_window::openChatSlot(PurpleChat *chat)
{
    qDebug() << "QQuailBListWindow::openChatSlot()";
    quail_blist_item *item;
	PurpleBlistNode *node = NULL;

	if (chat == NULL)
	{
        qDebug() << "QQuailBListWindow::openChatSlot().1";
        item = (quail_blist_item *)buddylist->currentItem();

		if (item != NULL)
		{
            qDebug() << "QQuailBListWindow::openChatSlot().2";
			node = item->getBlistNode();

            if (PURPLE_BLIST_NODE_IS_CHAT(node))
				chat = (PurpleChat *)node;
		}
	}
    qDebug() << "QQuailBListWindow::openChatSlot().3";
	if (chat != NULL)
	{
        qDebug() << "QQuailBListWindow::openChatSlot().4";
		serv_join_chat(purple_account_get_connection(chat->account),
					   chat->components);
	}
	else
	{
        qDebug() << "QQuailBListWindow::openChatSlot().5";
        quail_join_chat_dialog *dialog;

        dialog = new quail_join_chat_dialog(this);

        dialog->show();
	}
    qDebug() << "QQuailBListWindow::openChatSlot().end";
}

void
quail_blist_window::openChatSlot()
{
	openChatSlot(NULL);
}

/**************************************************************************
 * callbacks
 **************************************************************************/
static void
signedOnCb(PurpleConnection *gc, PurpleBuddyList *blist)
{
    qDebug() << "QQuailBListWindow::signedOnCb()";
    quail_blist_window *qblist = (quail_blist_window *)blist->ui_data;

	qblist->accountSignedOn(purple_connection_get_account(gc));
}

static void
signedOffCb(PurpleConnection *gc, PurpleBuddyList *blist)
{
    qDebug() << "QQuailBListWindow::signedOffCb()";
    quail_blist_window *qblist = (quail_blist_window *)blist->ui_data;

	qblist->accountSignedOff(purple_connection_get_account(gc));
}

/**************************************************************************
 * blist UI
 **************************************************************************/
static void
qQuailBlistNewList(PurpleBuddyList *blist)
{
    qDebug() << "QQuailBListWindow::qQuailBlistNewList()";
    quail_blist_window *win = qQuailGetMainWindow()->getBlistWindow();
	blist->ui_data = win;
    win->setBlist(blist);

	/* Setup some signal handlers */
	purple_signal_connect(purple_connections_get_handle(), "signed-on",
                        blist->ui_data, PURPLE_CALLBACK(signedOnCb), blist);
	purple_signal_connect(purple_connections_get_handle(), "signed-off",
                        blist->ui_data, PURPLE_CALLBACK(signedOffCb), blist);
}

static void
qQuailBlistNewNode(PurpleBlistNode * )
{
    //qDebug() << "QQuailBListWindow::qQuailBlistNewNode()";
    /* This is meant to happen in the BuddyList update functions */
}

static void
qQuailBlistShow(PurpleBuddyList *blist)
{
    qDebug() << "QQuailBListWindow::qQuailBlistShow()";
    quail_blist_window *blist_win = (quail_blist_window *)blist->ui_data;

	blist_win->show();
}

static void
qQuailBlistUpdate(PurpleBuddyList *blist, PurpleBlistNode *node)
{
    qDebug() << "QQuailBListWindow::qQuailBlistUpdate()";
    quail_blist_window *blist_win = (quail_blist_window *)blist->ui_data;
	blist_win->updateNode(node);
}

static void
qQuailBlistRemove(PurpleBuddyList *blist, PurpleBlistNode *node)
{
    //qDebug() << "QQuailBListWindow::qQuailBlistRemove()";
    quail_blist_window *blist_win = (quail_blist_window *)blist->ui_data;
    quail_blist_item *item = (quail_blist_item *)node->ui_data;
    quail_blist_item *parent;

	if (item != NULL)
	{
        parent = (quail_blist_item *)item->parent();

		delete item;

		if (parent != NULL)
			blist_win->updateNode(parent->getBlistNode());
	}

	node->ui_data = NULL;
}

static void
qQuailBlistDestroy(PurpleBuddyList *blist)
{
    qDebug() << "QQuailBListWindow::qQuailBlistDestroy()";
    quail_blist_window *qblist = (quail_blist_window *)blist->ui_data;

	delete qblist;

	blist->ui_data = NULL;
}

static void
qQuailBlistSetVisible(PurpleBuddyList *blist, gboolean show)
{
    qDebug() << "QQuailBListWindow::qQuailBlistSetVisible()";
    quail_blist_window *qblist = (quail_blist_window *)blist->ui_data;

	if (show)
		qblist->show();
	else
		qblist->hide();
}

static PurpleBlistUiOps blistUiOps =
{
	qQuailBlistNewList,
	qQuailBlistNewNode,
	qQuailBlistShow,
	qQuailBlistUpdate,
	qQuailBlistRemove,
	qQuailBlistDestroy,
	qQuailBlistSetVisible,
    NULL, /* request_add_buddy */
    NULL, /* request_add_chat */
    NULL, /* request_add_group */
    NULL, /* save_node */
    NULL, /* remove_node */
    NULL, /* save_account */
    NULL
};

PurpleBlistUiOps *
qQuailGetBlistUiOps()
{
	return &blistUiOps;
}
