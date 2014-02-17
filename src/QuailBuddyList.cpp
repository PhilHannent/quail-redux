/**
 * @file QQuailBuddyList.cpp Buddy list widget
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
#include "QuailAction.h"
#include "QuailBuddyList.h"
#include "QuailProtocolUtils.h"
#include "QuailImageUtils.h"
#include "QuailBlistItem.h"

#include <libpurple/debug.h>
#include <libpurple/prefs.h>
#include <libpurple/request.h>
#include <libpurple/server.h>

#include <QAction>
#include <QDebug>
#include <QHeaderView>
#include <QMenu>
#include <QSettings>
#include <QTimer>

#define BUDDY_ICON_SIZE 20


/**************************************************************************
 * QQuailBuddyList static utility functions
 **************************************************************************/
//QPixmap
//QQuailBuddyList::getBuddyStatusIcon2(PurpleBlistNode *node)
//{
//    qDebug() << "QQuailBuddyList::getBuddyStatusIcon";
//    QImage statusImage;
//	QImage emblemImage;
//	QPixmap statusPixmap;
//	const char *protoName = NULL;
//	PurplePlugin *prpl = NULL;
//	PurplePluginProtocolInfo *prplInfo = NULL;
//	PurpleAccount *account;
//	char *se = NULL, *sw = NULL, *nw = NULL, *ne = NULL;

//	if (node == NULL)
//		return QPixmap();
//    qDebug() << "QQuailBuddyList::getBuddyStatusIcon.1";
//    if (PURPLE_BLIST_NODE_IS_BUDDY(node))
//		account = ((PurpleBuddy *)node)->account;
//    else if (PURPLE_BLIST_NODE_IS_CHAT(node))
//		account = ((PurpleChat *)node)->account;
//	else
//		return QPixmap();
//    qDebug() << "QQuailBuddyList::getBuddyStatusIcon.2";
//	prpl = purple_plugins_find_with_id(purple_account_get_protocol_id(account));

//	if (prpl == NULL)
//		return QPixmap();
//    qDebug() << "QQuailBuddyList::getBuddyStatusIcon.3";
//    prplInfo = PURPLE_PLUGIN_PROTOCOL_INFO(prpl);

//	if (prplInfo->list_icon != NULL)
//	{
//        if (PURPLE_BLIST_NODE_IS_BUDDY(node))
//			protoName = prplInfo->list_icon(account, (PurpleBuddy *)node);
//        else if (PURPLE_BLIST_NODE_IS_CHAT(node))
//			protoName = prplInfo->list_icon(account, NULL);
//	}
//    qDebug() << "QQuailBuddyList::getBuddyStatusIcon.4";
////    if (PURPLE_BLIST_NODE_IS_BUDDY(node) &&
////		((PurpleBuddy *)node)->present != GAIM_BUDDY_SIGNING_OFF &&
////		prplInfo->list_emblems != NULL)
////	{
////		prplInfo->list_emblems((PurpleBuddy *)node, &se, &sw, &nw, &ne);
////	}

//	if (se == NULL)
//	{
//		if      (sw != NULL) se = sw;
//		else if (ne != NULL) se = ne;
//		else if (nw != NULL) se = nw;
//	}

//	sw = nw = ne = NULL; /* So that only the se icon will composite. */

////    if (PURPLE_BLIST_NODE_IS_BUDDY(node) &&
////        ((PurpleBuddy *)node)->present == PURPLE_BUDDY_SIGNING_ON)
////    {
////        statusImage = QPixmap(":/data/images/status/login.png");
////    }
////    else if (PURPLE_BLIST_NODE_IS_BUDDY(node) &&
////             ((PurpleBuddy *)node)->present == PURPLE_BUDDY_SIGNING_OFF)
////    {
////        statusImage = QPixmap(":/data/images/status/logout.png");
////    }
////    else
//	{
//        statusImage = QPixmap(":/data/images/protocols/" +
//                              QString(protoName) + ".svg").toImage();
//	}
//    qDebug() << "QQuailBuddyList::getBuddyStatusIcon.5:" << protoName;
//	if (statusImage.isNull())
//		return QPixmap();
//    qDebug() << "QQuailBuddyList::getBuddyStatusIcon.6";
////	if (se != NULL)
////	{
////        emblemImage = QPixmap(":/data/images/status/" + QString(se) + ".png").toImage();

////		if (!emblemImage.isNull())
////		{
////			QQuailImageUtils::blendOnLower(
////					statusImage.width()  - emblemImage.width(),
////					statusImage.height() - emblemImage.height(),
////					emblemImage, statusImage);
////		}
////	}

//    /* Grey idle buddies. */
//    if (PURPLE_BLIST_NODE_IS_BUDDY(node) &&
//        !PURPLE_BUDDY_IS_ONLINE((PurpleBuddy *)node))
//    {
//        QQuailImageUtils::greyImage(statusImage);
//    }
//    else if (PURPLE_BLIST_NODE_IS_BUDDY(node) &&
//             (purple_presence_is_idle(purple_buddy_get_presence((PurpleBuddy *)node))))
//    {
//        QQuailImageUtils::saturate(statusImage, 0.25);
//    }

//	statusPixmap.convertFromImage(statusImage);
//    qDebug() << "QQuailBuddyList::getBuddyStatusIcon.end";
//	return statusPixmap;
//}


/**************************************************************************
 * QQuailBuddyList
 **************************************************************************/
quail_buddy_list::quail_buddy_list(QWidget *parent)
    : QTreeWidget(parent)
{
    qDebug() << "QQuailBuddyList::QQuailBuddyList";
    //header()->hide();

    QStringList horzHeaders;
    horzHeaders << tr("Status") << tr("Name") << tr("Icon");
    setColumnCount(horzHeaders.size());
    setColumnWidth(0, BUDDY_ICON_SIZE);
    setColumnWidth(1, this->width() - (BUDDY_ICON_SIZE *2));
    setColumnWidth(2, BUDDY_ICON_SIZE);
    setHeaderLabels( horzHeaders );

    setUniformRowHeights(false);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setStyleSheet("QTreeWidget#treeWidget::item { height: " + QString(BUDDY_ICON_SIZE) + "px; }");

    connect(this, SIGNAL(itemExpanded(QTreeWidgetItem *)),
            this, SLOT(nodeExpandedSlot(QTreeWidgetItem *)));
    connect(this, SIGNAL(itemCollapsed(QTreeWidgetItem *)),
            this, SLOT(nodeCollapsedSlot(QTreeWidgetItem *)));
    //TODO: Find a replacement for this
    connect(this, SIGNAL(rightButtonPressed(QTreeWidgetItem *,
											const QPoint &, int)),
            this, SLOT(showContextMenuSlot(QTreeWidgetItem *,
										   const QPoint &, int)));

	saveTimer = new QTimer(this);

	connect(saveTimer, SIGNAL(timeout()),
			this, SLOT(saveBlistSlot()));
}

quail_buddy_list::~quail_buddy_list()
{
	delete saveTimer;
}

void
quail_buddy_list::setBlist(PurpleBuddyList *list)
{
    quailBlist = list;
}

PurpleBuddyList *
quail_buddy_list::getBlist() const
{
    return quailBlist;
}

void
quail_buddy_list::updateNode(PurpleBlistNode *node)
{
    switch (node->type)
	{
        case PURPLE_BLIST_GROUP_NODE:   updateGroup(node);   break;
        case PURPLE_BLIST_CONTACT_NODE: updateContact(node); break;
        case PURPLE_BLIST_BUDDY_NODE:   updateBuddy(node);   break;
        case PURPLE_BLIST_CHAT_NODE:    updateChat(node);    break;
		default:
			return;
	}
}

void
quail_buddy_list::reload(bool remove)
{
    qDebug() << "QQuailBuddyList::reload";
	PurpleBlistNode *group, *cnode, *child;

	if (remove)
		clear();

    for (group = quailBlist->root; group != NULL; group = group->next)
	{
        if (!PURPLE_BLIST_NODE_IS_GROUP(group))
			continue;

		group->ui_data = NULL;

		updateNode(group);

		for (cnode = group->child; cnode != NULL; cnode = cnode->next)
		{
            if (PURPLE_BLIST_NODE_IS_CONTACT(cnode))
			{
				cnode->ui_data = NULL;

				updateNode(cnode);

				for (child = cnode->child; child != NULL; child = child->next)
				{
					child->ui_data = NULL;

					updateNode(child);
				}
			}
            else if (PURPLE_BLIST_NODE_IS_CHAT(cnode))
			{
				cnode->ui_data = NULL;

				updateNode(cnode);
			}
		}
	}
}

PurpleBuddy *
quail_buddy_list::getSelectedBuddy() const
{
    qDebug() << "QQuailBuddyList::getSelectedBuddy";
	QQuailBListItem *item;
	PurpleBlistNode *node;
	PurpleBuddy *buddy = NULL;

    if ((item = (QQuailBListItem *)currentItem()) == NULL)
        return NULL;

	if ((node = item->getBlistNode()) == NULL)
		return NULL;

    if (PURPLE_BLIST_NODE_IS_BUDDY(node))
		buddy = (PurpleBuddy *)node;
    else if (PURPLE_BLIST_NODE_IS_CONTACT(node))
		buddy = purple_contact_get_priority_buddy((PurpleContact *)node);

	return buddy;
}

void
quail_buddy_list::populateBuddyMenu(PurpleBuddy *buddy, QMenu *menu,
								  bool asContact)
{
    qDebug() << "QQuailBuddyList::populateBuddyMenu";
    PurplePlugin *prpl = NULL;
	PurplePluginProtocolInfo *prplInfo = NULL;
    QQuailAction *a;

	prpl = purple_plugins_find_with_id(
		purple_account_get_protocol_id(buddy->account));

	if (prpl != NULL)
        prplInfo = PURPLE_PLUGIN_PROTOCOL_INFO(prpl);

	/* Get User Info */
	if (prplInfo != NULL && prplInfo->get_info != NULL)
	{
//        a = new QQuailAction(QIcon(QPixmap(":/data/images/actions/info.png")),
//                             tr("Get Information"),
//                             this);
//        a->setData(QVariant(buddy));
//        menu->addAction(a);

//		connect(a, SIGNAL(activated(void *)),
//				this, SLOT(userInfoSlot(void *)));
	}

	/* IM */
//    a = new QQuailAction(QIcon(QPixmap(":/data/images/actions/send-im.png")),
//                         tr("IM"),
//                         this);
//    a->setData(buddy);
//    menu->addAction(a);

//	connect(a, SIGNAL(activated(void *)),
//			this, SLOT(sendImSlot(void *)));

	/* Put in the protocol-specific actions */
//	if (prplInfo != NULL && prplInfo->buddy_menu != NULL)
//	{
//		GList *l;
//		PurpleConnection *gc = purple_account_get_connection(buddy->account);

//		for (l = prplInfo->buddy_menu(gc, buddy->name);
//			 l != NULL;
//			 l = l->next)
//		{
//			struct proto_buddy_menu *pbm;

//			pbm = (struct proto_buddy_menu *)l->data;

//            a = new QQuailAction(pbm->label, QString::null, 0, this, 0,
//                                false, buddy);
//            a->setUserData2(pbm);
//            menu->addAction(a);

//			connect(a, SIGNAL(activated(void *, void *)),
//					this, SLOT(protoActionSlot(void *, void *)));
//		}
//	}

	/* Separator */
    menu->addSeparator();

	/* Alias */
	a = new QQuailAction(tr("Alias"),
                        QIcon(QPixmap(":/data/images/actions/alias.png")),
                        this, buddy);
    menu->addAction(a);

	connect(a, SIGNAL(activated(void *)),
			this, SLOT(aliasBuddySlot(void *)));

	/* Remove */
	a = new QQuailAction(tr("Remove"),
                        QIcon(QPixmap(":/data/images/actions/remove.png")),
                        this, buddy);
    menu->addAction(a);

	connect(a, SIGNAL(activated(void *)),
			this, SLOT(removeBuddySlot(void *)));

	if (asContact)
	{
		PurpleBlistNode *cnode = ((PurpleBlistNode *)buddy)->parent;

		/* Separator */
        menu->addSeparator();

		/* Expand */
		a = new QQuailAction(tr("Expand"),
                            QIcon(QPixmap(":/data/images/actions/expand.png")),
                            this);
        menu->addAction(a);

        connect(a, SIGNAL(triggered(bool)),
				this, SLOT(expandContactSlot()));

		if (cnode->child->next != NULL)
		{
			PurpleBlistNode *bnode;
			bool showOffline =
                purple_prefs_get_bool("/quail/blist/show_offline_buddies");

			/* List of other accounts */
			for (bnode = cnode->child; bnode != NULL; bnode = bnode->next)
			{
				PurpleBuddy *buddy2 = (PurpleBuddy *)bnode;

				if (buddy2 == buddy)
					continue;

				if (!purple_account_is_connected(buddy2->account))
					continue;

                if (!showOffline && PURPLE_BUDDY_IS_ONLINE(buddy2))
                {
                    QMenu *subMenu = new QMenu(this);

                    populateBuddyMenu(buddy2, subMenu, false);
                    //subMenu->setIcon(QQuailBuddyList::getBuddyStatusIcon(bnode));
                    subMenu->setTitle(buddy2->name);
                    menu->addMenu(subMenu);
                }
			}
		}
	}
}

void
quail_buddy_list::populateContactMenu(PurpleContact */*contact*/, QMenu *menu)
{
    qDebug() << "QQuailBuddyList::populateContactMenu";
//	PurplePlugin *prpl = NULL;
    //PurplePluginProtocolInfo *prplInfo = NULL;
	QAction *a;

//	prpl = purple_plugins_find_with_id(purple_account_get_protocol_id(
//			purple_contact_get_priority_buddy(contact)->account));

//	if (prpl != NULL)
//        prplInfo = PURPLE_PLUGIN_PROTOCOL_INFO(prpl);

	/* Collapse */
    a = new QAction(QIcon(QPixmap(":/data/images/actions/collapse.png")),
                    tr("Collapse"),
                    this);
    menu->addAction(a);

    connect(a, SIGNAL(triggered(bool)),
			this, SLOT(collapseContactSlot()));

	/* Remove */
    a = new QAction(QIcon(QPixmap(":/data/images/actions/remove.png")),
                    tr("Remove"),
                    this);
    menu->addAction(a);

    connect(a, SIGNAL(triggered(bool)),
			this, SLOT(removeContactSlot()));
}

void
quail_buddy_list::populateChatMenu(PurpleChat *chat, QMenu *menu)
{
    qDebug() << "QQuailBuddyList::populateChatMenu";
    //PurplePlugin *prpl = NULL;
    //PurplePluginProtocolInfo *prplInfo = NULL;
	QAction *a;

//	prpl = purple_plugins_find_with_id(
//		purple_account_get_protocol_id(chat->account));

//	if (prpl != NULL)
//        prplInfo = PURPLE_PLUGIN_PROTOCOL_INFO(prpl);

	/* Join Chat */
    a = new QAction(QIcon(QPixmap(":/data/images/actions/join_chat.png"))
                    , tr("Join Chat")
                    , this);
    menu->addAction(a);

    connect(a, SIGNAL(triggered(bool)),
			this, SLOT(joinChatSlot()));

	/* Auto-Join */
    a = new QAction(tr("Auto-Join"), this);

    if (purple_blist_node_get_bool((PurpleBlistNode *)chat, "autojoin"))
        a->setChecked(true);

    menu->addAction(a);

	connect(a, SIGNAL(toggled(bool)),
			this, SLOT(autoJoinChatSlot(bool)));

	/* Alias */
    a = new QAction(QIcon(QPixmap(":/data/images/actions/alias.png"))
                    , tr("Alias")
                    , this);
    menu->addAction(a);

    connect(a, SIGNAL(triggered(bool)),
			this, SLOT(aliasChatSlot()));

	/* Remove */
    a = new QAction(QIcon(QPixmap(":/data/images/actions/remove.png"))
                    , tr("Remove Chat")
                    , this);
    menu->addAction(a);

    connect(a, SIGNAL(triggered(bool)),
			this, SLOT(removeChatSlot()));
}

void
quail_buddy_list::populateGroupMenu(PurpleGroup *, QMenu *menu)
{
    qDebug() << "QQuailBuddyList::populateGroupMenu";
	QAction *a;

	/* Add a Buddy */
    a = new QAction(QIcon(QPixmap(":/data/images/actions/add.png")),
                    tr("Add a Buddy"),
                    this);
    menu->addAction(a);

    connect(a, SIGNAL(triggered(bool)),
			this, SLOT(addBuddySlot()));

	/* Add a Chat */
    a = new QAction(QIcon(QPixmap(":/data/images/actions/new-chat.png")),
                    tr("Add a Chat"),
                    this);
    menu->addAction(a);

    connect(a, SIGNAL(triggered(bool)),
			this, SLOT(addChatSlot()));

	/* Delete Group */
    a = new QAction(QIcon(QPixmap(":/data/images/actions/remove.png")),
                    tr("Remove Group"),
                    this);
    menu->addAction(a);

    connect(a, SIGNAL(triggered(bool)),
			this, SLOT(removeGroupSlot()));

	/* Rename */
    a = new QAction(tr("Rename"), this);
    menu->addAction(a);

    connect(a, SIGNAL(triggered(bool)),
			this, SLOT(renameGroupSlot()));
}

void
quail_buddy_list::resizeEvent(QResizeEvent *)
{
    //qDebug() << "QQuailBuddyList::resizeEvent";
    setColumnWidth(0, BUDDY_ICON_SIZE);
    setColumnWidth(1, this->width() - (BUDDY_ICON_SIZE *4));
    setColumnWidth(2, BUDDY_ICON_SIZE);
    /* Trigger a geometry and state save */
    if (!saveTimer->isActive())
        saveTimer->start(5000);
}

void
quail_buddy_list::nodeExpandedSlot(QTreeWidgetItem *_item)
{
    qDebug() << "QQuailBuddyList::nodeExpandedSlot";
	QQuailBListItem *item = (QQuailBListItem *)_item;
	PurpleBlistNode *node;

	node = item->getBlistNode();

    if (PURPLE_BLIST_NODE_IS_GROUP(node))
	{
        qDebug() << "QQuailBuddyList::nodeExpandedSlot.setNodeCollapsed:FALSE";
		purple_blist_node_set_bool(node, "collapsed", FALSE);

	}
    else if (PURPLE_BLIST_NODE_IS_CONTACT(node))
	{

	}
}

void
quail_buddy_list::nodeCollapsedSlot(QTreeWidgetItem *_item)
{
    qDebug() << "QQuailBuddyList::nodeCollapsedSlot";
	QQuailBListItem *item = (QQuailBListItem *)_item;
	PurpleBlistNode *node;

	node = item->getBlistNode();

    if (PURPLE_BLIST_NODE_IS_GROUP(node))
	{
        qDebug() << "QQuailBuddyList::nodeCollapsedSlot:collapsed::true";
		purple_blist_node_set_bool(node, "collapsed", TRUE);
	}
    else if (PURPLE_BLIST_NODE_IS_CONTACT(node))
	{
		collapseContactSlot(item);
	}
}

void
quail_buddy_list::collapseContactSlot()
{
    collapseContactSlot((QQuailBListItem *)currentItem());
}

void
quail_buddy_list::collapseContactSlot(QQuailBListItem *item)
{
    qDebug() << "QQuailBuddyList::collapseContactSlot";
	PurpleBlistNode *node;

	g_return_if_fail(item != NULL);

	node = item->getBlistNode();

    g_return_if_fail(PURPLE_BLIST_NODE_IS_CONTACT(node));

	item->setExpanded(false);
//	item->setExpandable(false);

//	while (item->firstChild())
//		delete item->firstChild();

	item->updateInfo();
}

void
quail_buddy_list::expandContactSlot()
{
    expandContactSlot((QQuailBListItem *)currentItem());
}

void
quail_buddy_list::expandContactSlot(QQuailBListItem *item)
{
    qDebug() << "QQuailBuddyList::expandContactSlot";
	PurpleBlistNode *node, *child;

	g_return_if_fail(item != NULL);

	node = item->getBlistNode();

    g_return_if_fail(PURPLE_BLIST_NODE_IS_CONTACT(node));

	item->setExpanded(true);
//	item->setExpandable(true);
//	item->setOpen(true);

	for (child = node->child; child != NULL; child = child->next)
	{
        qDebug() << "QQuailBuddyList::expandContactSlot.1<<<<< set ui_data NULL";
		child->ui_data = NULL;

		updateNode(child);
	}

	item->updateInfo();
}

void
quail_buddy_list::saveBlistSlot()
{
    QSettings appSettings(APP_NAME, APP_MAJOR_VERSION);
    appSettings.setValue("geometry", saveGeometry());
    //appSettings.setValue("state", saveState());
}

//TODO: This needs moving to the items contextMenuEvent
void
quail_buddy_list::showContextMenuSlot(QTreeWidgetItem *_item,
									const QPoint &point, int)
{
    qDebug() << "QQuailBuddyList::showContextMenuSlot";
    QQuailBListItem *item = (QQuailBListItem *)_item;
    QMenu *menu = NULL;
	PurpleBlistNode *node;

	if (item == NULL)
		return;

	node = item->getBlistNode();

    menu = new QMenu(this);

    if (PURPLE_BLIST_NODE_IS_BUDDY(node))
	{
		populateBuddyMenu((PurpleBuddy *)node, menu, false);
	}
    else if (PURPLE_BLIST_NODE_IS_CONTACT(node))
	{
		PurpleContact *contact = (PurpleContact *)node;

		if (item->isExpanded())
		{
			populateContactMenu(contact, menu);
		}
		else
		{
			populateBuddyMenu(purple_contact_get_priority_buddy(contact), menu,
							  true);
		}
	}
    else if (PURPLE_BLIST_NODE_IS_CHAT(node))
	{
		populateChatMenu((PurpleChat *)node, menu);
	}
    else if (PURPLE_BLIST_NODE_IS_GROUP(node))
	{
		populateGroupMenu((PurpleGroup *)node, menu);
	}
	else
	{
		delete menu;

		return;
	}

	menu->popup(point);
}

void
quail_buddy_list::addBuddySlot()
{
    qDebug() << "QQuailBuddyList::addBuddySlot";
	PurpleBlistNode *node;
	QQuailBListItem *item;

    if ((item = (QQuailBListItem *)currentItem()) == NULL)
        return;

	node = item->getBlistNode();

    if (!PURPLE_BLIST_NODE_IS_GROUP(node))
		return;

	emit addBuddy((PurpleGroup *)node);
}

void
quail_buddy_list::addChatSlot()
{
    qDebug() << "QQuailBuddyList::addChatSlot";
	PurpleBlistNode *node;
	QQuailBListItem *item;

    if ((item = (QQuailBListItem *)currentItem()) == NULL)
        return;

	node = item->getBlistNode();

    if (!PURPLE_BLIST_NODE_IS_GROUP(node))
		return;

	emit addChat((PurpleGroup *)node);
}

void
quail_buddy_list::removeGroupSlot()
{
    qDebug() << "QQuailBuddyList::removeGroupSlot";
    PurpleBlistNode *node;
	QQuailBListItem *item;

    if ((item = (QQuailBListItem *)currentItem()) == NULL)
        return;

	node = item->getBlistNode();

    if (!PURPLE_BLIST_NODE_IS_GROUP(node))
		return;

	emit removeGroup((PurpleGroup *)node);
}

//static void
//renameGroupCb(PurpleGroup *group, const char *newName)
//{
//    qDebug() << "QQuailBuddyList::renameGroupCb";
//	purple_blist_rename_group(group, newName);
////	purple_blist_save();
//}

void
quail_buddy_list::renameGroupSlot()
{
    qDebug() << "QQuailBuddyList::renameGroupSlot";
	PurpleBlistNode *node;
	QQuailBListItem *item;
//	PurpleGroup *group;

    if ((item = (QQuailBListItem *)currentItem()) == NULL)
        return;

	node = item->getBlistNode();

    if (!PURPLE_BLIST_NODE_IS_GROUP(node))
		return;

//	group = (PurpleGroup *)node;

//	purple_request_input(NULL, tr("Rename Group"),
//					   tr("Rename Group"),
//					   tr("Please enter a new name for the selected group."),
//					   group->name, FALSE, FALSE,
//					   tr("Rename"), G_CALLBACK(renameGroupCb),
//					   tr("Cancel"), NULL, group);
}

void
quail_buddy_list::userInfoSlot(void *data)
{
    qDebug() << "QQuailBuddyList::userInfoSlot";
	PurpleBuddy *buddy = (PurpleBuddy *)data;

	serv_get_info(purple_account_get_connection(buddy->account), buddy->name);
}

void
quail_buddy_list::sendImSlot(void *data)
{
    qDebug() << "QQuailBuddyList::sendImSlot";
	emit openIm((PurpleBuddy *)data);
}

void
quail_buddy_list::protoActionSlot(void * /*data1*/, void * /*data2*/)
{
    qDebug() << "QQuailBuddyList::protoActionSlot";
//	PurpleBuddy *buddy = (PurpleBuddy *)data1;
//	struct proto_buddy_menu *pbm = (struct proto_buddy_menu *)data2;

//	if (pbm->callback != NULL)
//		pbm->callback(pbm->gc, buddy->name);
}

//static void
//aliasBuddyCb(PurpleBuddy *buddy, const char *newAlias)
//{
//    qDebug() << "QQuailBuddyList::aliasBuddyCb";
//	purple_blist_alias_buddy(buddy, newAlias);
////	purple_blist_save();
//}

void
quail_buddy_list::aliasBuddySlot(void */*data*/)
{
    qDebug() << "QQuailBuddyList::aliasBuddySlot";
//	PurpleBuddy *buddy = (PurpleBuddy *)data;

//	purple_request_input(NULL, tr("Alias Buddy"),
//			tr("Please enter an aliased name for %1.").arg(buddy->name),
//			NULL,
//			buddy->alias, false, false,
//			tr("Alias"), G_CALLBACK(aliasBuddyCb),
//			tr("Cancel"), NULL, buddy);
}

void
quail_buddy_list::removeBuddySlot(void *data)
{
    qDebug() << "QQuailBuddyList::removeBuddySlot";
    emit removeBuddy((PurpleBuddy *)data);
}

void
quail_buddy_list::removeContactSlot()
{
    qDebug() << "QQuailBuddyList::removeContactSlot";
    PurpleBlistNode *node;
	QQuailBListItem *item;

    if ((item = (QQuailBListItem *)currentItem()) == NULL)
        return;

	node = item->getBlistNode();

    if (!PURPLE_BLIST_NODE_IS_CONTACT(node))
		return;

	emit removeContact((PurpleContact *)node);
}

void
quail_buddy_list::joinChatSlot()
{
    qDebug() << "QQuailBuddyList::joinChatSlot";
    PurpleBlistNode *node;
	QQuailBListItem *item;

    if ((item = (QQuailBListItem *)currentItem()) == NULL)
        return;

	node = item->getBlistNode();

    if (!PURPLE_BLIST_NODE_IS_CHAT(node))
		return;

	emit joinChat((PurpleChat *)node);
}

void
quail_buddy_list::autoJoinChatSlot(bool on)
{
    qDebug() << "QQuailBuddyList::autoJoinChatSlot";
    PurpleBlistNode *node;
	QQuailBListItem *item;

    if ((item = (QQuailBListItem *)currentItem()) == NULL)
        return;

	node = item->getBlistNode();

    if (!PURPLE_BLIST_NODE_IS_CHAT(node))
		return;

    purple_blist_node_set_bool(node, "autojoin", on);
//	purple_blist_save();
}

void
quail_buddy_list::removeChatSlot()
{
    qDebug() << "QQuailBuddyList::autoJoinChatSlot";
    PurpleBlistNode *node;
	QQuailBListItem *item;

    if ((item = (QQuailBListItem *)currentItem()) == NULL)
        return;

	node = item->getBlistNode();

    if (!PURPLE_BLIST_NODE_IS_CHAT(node))
		return;

	emit removeChat((PurpleChat *)node);
}

//static void
//aliasChatCb(PurpleChat *chat, const char *newAlias)
//{
//    qDebug() << "QQuailBuddyList::aliasChatCb";
//	purple_blist_alias_chat(chat, newAlias);
////	purple_blist_save();
//}

void
quail_buddy_list::aliasChatSlot()
{
    qDebug() << "QQuailBuddyList::aliasChatSlot";
    PurpleBlistNode *node;
	QQuailBListItem *item;
//	PurpleChat *chat;

    if ((item = (QQuailBListItem *)currentItem()) == NULL)
        return;

	node = item->getBlistNode();

    if (!PURPLE_BLIST_NODE_IS_CHAT(node))
		return;

//	chat = (PurpleChat *)node;

//	purple_request_input(NULL, tr("Alias Chat"),
//					   tr("Please enter an aliased name for this chat."),
//					   NULL,
//					   chat->alias, false, false,
//					   tr("Alias"), G_CALLBACK(aliasChatCb),
//					   tr("Cancel"), NULL, chat);
}

void
quail_buddy_list::addGroup(PurpleBlistNode *node)
{
    qDebug() << "QQuailBuddyList::addGroup";
    QQuailBListItem *item = new QQuailBListItem(this, node);
	node->ui_data = item;

}

void
quail_buddy_list::updateGroup(PurpleBlistNode *node)
{
    qDebug() << "QQuailBuddyList::updateGroup" << node;
    PurpleGroup *group;
	QQuailBListItem *item;

    g_return_if_fail(PURPLE_BLIST_NODE_IS_GROUP(node));

	item = (QQuailBListItem *)node->ui_data;
	group = (PurpleGroup *)node;

    if (purple_prefs_get_bool("/quail/blist/show_empty_groups")    ||
        purple_prefs_get_bool("/quail/blist/show_offline_buddies") ||
		purple_blist_get_group_online_count(group) > 0)
	{
		if (item == NULL)
		{
			addGroup(node);
			item = (QQuailBListItem *)node->ui_data;
		}
        if (!purple_blist_node_get_bool(node, "collapsed"))
        {
            item->setExpanded(true);
        }
	}
	else
	{
		if (item != NULL)
			delete item;
	}
    //qDebug() << "QQuailBuddyList::updateGroup.end";
}

void
quail_buddy_list::updateContact(PurpleBlistNode *node)
{
    qDebug() << "QQuailBuddyList::updateContact";
    PurpleContact *contact;
	PurpleBuddy *buddy;
	QQuailBListItem *item;

    g_return_if_fail(PURPLE_BLIST_NODE_IS_CONTACT(node));

	updateGroup(node->parent);

	item    = (QQuailBListItem *)node->ui_data;
	contact = (PurpleContact *)node;
	buddy   = purple_contact_get_priority_buddy(contact);
    qDebug() << "QQuailBuddyList::updateContact:1:" << buddy->name;
//    qDebug() << "QQuailBuddyList::updateContact:2:" << (buddy != NULL);
//    qDebug() << "QQuailBuddyList::updateContact:3:" << PURPLE_BUDDY_IS_ONLINE(buddy);
//    qDebug() << "QQuailBuddyList::updateContact:4:" << purple_account_is_connected(buddy->account);
//    qDebug() << "QQuailBuddyList::updateContact:4:" << purple_prefs_get_bool("/quail/blist/show_offline_buddies");
    if ((buddy != NULL) &&
         (PURPLE_BUDDY_IS_ONLINE(buddy) ||
          (purple_prefs_get_bool("/quail/blist/show_offline_buddies"))))
    {
        qDebug() << "QQuailBuddyList::updateContact.1";
        if (item == NULL)
        {
            qDebug() << "QQuailBuddyList::updateContact.2";
            node->ui_data = item = new QQuailBListItem(
                (QQuailBListItem *)(node->parent->ui_data), node);
            item->setSizeHint(0, QSize(this->width(), 50));
            //item->setIcon(0, QIcon(QQuailBuddyList::getBuddyStatusIcon(node)));
        }
        else
        {
            qDebug() << "QQuailBuddyList::updateContact.2";
            item->updateInfo();
        }
    }
    else if (item != NULL)
    {
        qDebug() << "QQuailBuddyList::updateContact.3";
        delete item;
    }
//    qDebug() << "QQuailBuddyList::updateContact.end";
}

void
quail_buddy_list::updateBuddy(PurpleBlistNode *node)
{
    qDebug() << "QQuailBuddyList::updateBuddy";
    //PurpleContact *contact;
	PurpleBuddy *buddy;
	QQuailBListItem *item;

    g_return_if_fail(PURPLE_BLIST_NODE_IS_BUDDY(node));

	updateContact(node->parent);

	if (node->parent->ui_data == NULL ||
		(node->parent->ui_data != NULL &&
		 !((QQuailBListItem *)node->parent->ui_data)->isExpanded()))
	{
        qDebug() << "QQuailBuddyList::updateBuddy.no ui_data";
		return;
	}

	buddy   = (PurpleBuddy *)node;
//	contact = (PurpleContact *)node->parent;
	item    = (QQuailBListItem *)node->ui_data;

    if (PURPLE_BUDDY_IS_ONLINE(buddy) ||
        (purple_account_is_connected(buddy->account) &&
         purple_prefs_get_bool("/quail/blist/show_offline_buddies")))
    {
        if (item == NULL)
        {
            node->ui_data = item = new QQuailBListItem(
                (QQuailBListItem *)(node->parent->ui_data), node);
        }
        else
            item->updateInfo();
    }
    else if (item != NULL)
        delete item;
}

void
quail_buddy_list::updateChat(PurpleBlistNode *node)
{
    qDebug() << "QQuailBuddyList::updateChat";
    PurpleChat *chat;
	QQuailBListItem *item;

    g_return_if_fail(PURPLE_BLIST_NODE_IS_CHAT(node));

	updateGroup(node->parent);

	chat = (PurpleChat *)node;
	item = (QQuailBListItem *)node->ui_data;

	if (purple_account_is_connected(chat->account))
	{
		if (item == NULL)
		{
            node->ui_data = item = new QQuailBListItem(
                (QQuailBListItem *)(node->parent->ui_data), node);
		}
		else
			item->updateInfo();
	}
	else if (item != NULL)
		delete item;
}
