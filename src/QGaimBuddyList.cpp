/**
 * @file QGaimBuddyList.cpp Buddy list widget
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
#include "QGaimAction.h"
#include "QGaimBuddyList.h"
#include "QGaimProtocolUtils.h"
#include "QGaimImageUtils.h"

#include <libgaim/debug.h>
#include <libgaim/multi.h>
#include <libgaim/prefs.h>
#include <libgaim/request.h>
#include <libgaim/server.h>

#include <qpe/qpeapplication.h>
#include <qpe/resource.h>

#include <qaction.h>
#include <qheader.h>
#include <qpopupmenu.h>
#include <qtimer.h>


/**************************************************************************
 * QGaimBListItem
 **************************************************************************/
QGaimBListItem::QGaimBListItem(QListView *parent, GaimBlistNode *node)
	: QListViewItem(parent), node(node), expanded(false)
{
	init();
}

QGaimBListItem::QGaimBListItem(QListViewItem *parent, GaimBlistNode *node)
	: QListViewItem(parent), node(node), expanded(false)
{
	init();
}

QGaimBListItem::~QGaimBListItem()
{
	if (node != NULL)
		node->ui_data = NULL;
}

GaimBlistNode *
QGaimBListItem::getBlistNode() const
{
	return node;
}

void
QGaimBListItem::updateInfo()
{
	if (GAIM_BLIST_NODE_IS_CONTACT(node))
	{
		GaimContact *contact = (GaimContact *)node;
		GaimBuddy *buddy = gaim_contact_get_priority_buddy(contact);

		if (buddy == NULL)
			return;

		if (isExpanded())
		{
			QImage image = Resource::loadPixmap("gaim").convertToImage();
			QPixmap pixmap;

			pixmap.convertFromImage(image.smoothScale(16, 16));

			setPixmap(0, pixmap);
		}
		else
		{
			if (buddy->idle > 0 &&
				gaim_prefs_get_bool("/gaim/qpe/blist/show_idle_times"))
			{
				time_t t;
				int ihrs, imin;
				char *idle;

				time(&t);

				ihrs = (t - buddy->idle) / 3600;
				imin = ((t - buddy->idle) / 60) % 60;

				if (ihrs > 0)
					idle = g_strdup_printf("(%d:%02d)", ihrs, imin);
				else
					idle = g_strdup_printf("(%d)", imin);

				QString str = idle;

				setText(1, str);

				g_free(idle);
			}

			setPixmap(0,
				QGaimBuddyList::getBuddyStatusIcon((GaimBlistNode *)buddy));
		}

		setText(0, gaim_get_buddy_alias(buddy));
	}
	else if (GAIM_BLIST_NODE_IS_BUDDY(node))
	{
		GaimBuddy *buddy = (GaimBuddy *)node;

		if (buddy->idle > 0 &&
			gaim_prefs_get_bool("/gaim/qpe/blist/show_idle_times"))
		{
			time_t t;
			int ihrs, imin;
			char *idle;

			time(&t);

			ihrs = (t - ((GaimBuddy *)node)->idle) / 3600;
			imin = ((t - ((GaimBuddy*)node)->idle) / 60) % 60;

			if (ihrs > 0)
				idle = g_strdup_printf("(%d:%02d)", ihrs, imin);
			else
				idle = g_strdup_printf("(%d)", imin);

			QString str = idle;

			setText(1, str);

			g_free(idle);
		}

		setPixmap(0, QGaimBuddyList::getBuddyStatusIcon(node));
		setText(0, gaim_get_buddy_alias(buddy));
	}
	else if (GAIM_BLIST_NODE_IS_CHAT(node))
	{
		GaimChat *chat = (GaimChat *)node;

		setPixmap(0, QGaimProtocolUtils::getProtocolIcon(chat->account));
		setText(0, gaim_chat_get_display_name(chat));
	}
	else if (GAIM_BLIST_NODE_IS_GROUP(node))
	{
		GaimGroup *group = (GaimGroup *)node;

		if (gaim_prefs_get_bool("/gaim/qpe/blist/show_group_count"))
		{
			char *size;

			size = g_strdup_printf("%s (%d/%d)",
								   group->name,
								   gaim_blist_get_group_online_count(group),
								   gaim_blist_get_group_size(group, FALSE));

			setText(0, size);

			g_free(size);
		}
		else
			setText(0, group->name);
	}
}

void
QGaimBListItem::paintCell(QPainter *p, const QColorGroup &cg, int column,
						  int width, int align)
{
	if (GAIM_BLIST_NODE_IS_GROUP(node))
	{
		QFont f = p->font();
		f.setBold(true);
		p->setFont(f);
	}

	QListViewItem::paintCell(p, cg, column, width, align);
}

void
QGaimBListItem::paintBranches(QPainter *p, const QColorGroup &cg,
							  int width, int x, int height, GUIStyle style)
{
	if (GAIM_BLIST_NODE_IS_CHAT(node) || GAIM_BLIST_NODE_IS_BUDDY(node))
	{
		p->fillRect(0, 0, width, height, QBrush(cg.base()));
	}
	else
		return QListViewItem::paintBranches(p, cg, width, x, height, style);
}

void
QGaimBListItem::setExpanded(bool expanded)
{
	this->expanded = expanded;
}

bool
QGaimBListItem::isExpanded() const
{
	return expanded;
}

void
QGaimBListItem::init()
{
	updateInfo();
}


/**************************************************************************
 * QGaimBuddyList static utility functions
 **************************************************************************/
QPixmap
QGaimBuddyList::getBuddyStatusIcon(GaimBlistNode *node, QGaimPixmapSize size)
{
	QImage statusImage;
	QImage emblemImage;
	QPixmap statusPixmap;
	const char *protoName = NULL;
	GaimPlugin *prpl = NULL;
	GaimPluginProtocolInfo *prplInfo = NULL;
	char *se = NULL, *sw = NULL, *nw = NULL, *ne = NULL;

	if (node == NULL)
		return QPixmap();

	if (GAIM_BLIST_NODE_IS_BUDDY(node))
	{
		prpl = gaim_find_prpl(gaim_account_get_protocol(
				((GaimBuddy *)node)->account));
	}
	else if (GAIM_BLIST_NODE_IS_CHAT(node))
	{
		prpl = gaim_find_prpl(gaim_account_get_protocol(
				((GaimChat *)node)->account));
	}

	if (prpl == NULL)
		return QPixmap();

	prplInfo = GAIM_PLUGIN_PROTOCOL_INFO(prpl);

	if (prplInfo->list_icon != NULL)
	{
		if (GAIM_BLIST_NODE_IS_BUDDY(node))
			protoName = prplInfo->list_icon(((GaimBuddy *)node)->account,
											(GaimBuddy *)node);
		else if (GAIM_BLIST_NODE_IS_CHAT(node))
			protoName = prplInfo->list_icon(((GaimChat *)node)->account,
											NULL);
	}

	if (GAIM_BLIST_NODE_IS_BUDDY(node) &&
		((GaimBuddy *)node)->present != GAIM_BUDDY_SIGNING_OFF &&
		prplInfo->list_emblems != NULL)
	{
		prplInfo->list_emblems((GaimBuddy *)node, &se, &sw, &nw, &ne);
	}

	if (se == NULL)
	{
		if      (sw != NULL) se = sw;
		else if (ne != NULL) se = ne;
		else if (nw != NULL) se = nw;
	}

	sw = nw = ne = NULL; /* So that only the se icon will composite. */

	if (GAIM_BLIST_NODE_IS_BUDDY(node) &&
		((GaimBuddy *)node)->present == GAIM_BUDDY_SIGNING_ON)
	{
		statusImage = Resource::loadImage("gaim/status/login");
	}
	else if (GAIM_BLIST_NODE_IS_BUDDY(node) &&
			 ((GaimBuddy *)node)->present == GAIM_BUDDY_SIGNING_OFF)
	{
		statusImage = Resource::loadImage("gaim/status/logout");
	}
	else
	{
		statusImage = Resource::loadImage("gaim/protocols/" +
										  QString(protoName));
	}

	if (size == QGAIM_PIXMAP_LARGE)
		statusImage = statusImage.smoothScale(32, 32);
	else if (size == QGAIM_PIXMAP_SMALL)
		statusImage = statusImage.smoothScale(16, 16);
	else if (size == QGAIM_PIXMAP_MENU)
		statusImage = statusImage.smoothScale(14, 14);

	if (statusImage.isNull())
		return QPixmap();

	if (se != NULL)
	{
		emblemImage = Resource::loadImage("gaim/status/" + QString(se));

		if (!emblemImage.isNull())
		{
			if (size == QGAIM_PIXMAP_SMALL || size == QGAIM_PIXMAP_MENU)
				emblemImage = emblemImage.smoothScale(11, 11);

			QGaimImageUtils::blendOnLower(
					statusImage.width()  - emblemImage.width(),
					statusImage.height() - emblemImage.height(),
					emblemImage, statusImage);
		}
	}

	/* Grey idle buddies. */
	if (GAIM_BLIST_NODE_IS_BUDDY(node) &&
		((GaimBuddy *)node)->present == GAIM_BUDDY_OFFLINE)
	{
		QGaimImageUtils::greyImage(statusImage);
	}
	else if (GAIM_BLIST_NODE_IS_BUDDY(node) &&
			 ((GaimBuddy *)node)->idle)
	{
		QGaimImageUtils::saturate(statusImage, 0.25);
	}

	statusPixmap.convertFromImage(statusImage);

	return statusPixmap;
}

/**************************************************************************
 * QGaimBuddyList
 **************************************************************************/
QGaimBuddyList::QGaimBuddyList(QWidget *parent, const char *name)
	: QListView(parent, name)
{
	addColumn(tr("Buddy"), -1);
	addColumn(tr("Idle"), 50);

	setColumnAlignment(1, AlignRight);

	setAllColumnsShowFocus(TRUE);
	setRootIsDecorated(true);

	header()->hide();

	QPEApplication::setStylusOperation(viewport(),
									   QPEApplication::RightOnHold);

	connect(this, SIGNAL(expanded(QListViewItem *)),
			this, SLOT(nodeExpandedSlot(QListViewItem *)));
	connect(this, SIGNAL(collapsed(QListViewItem *)),
			this, SLOT(nodeCollapsedSlot(QListViewItem *)));
	connect(this, SIGNAL(rightButtonPressed(QListViewItem *,
											const QPoint &, int)),
			this, SLOT(showContextMenuSlot(QListViewItem *,
										   const QPoint &, int)));

	saveTimer = new QTimer(this);

	connect(saveTimer, SIGNAL(timeout()),
			this, SLOT(saveBlistSlot()));
}

QGaimBuddyList::~QGaimBuddyList()
{
	delete saveTimer;
}

void
QGaimBuddyList::setGaimBlist(GaimBuddyList *list)
{
	gaimBlist = list;
}

GaimBuddyList *
QGaimBuddyList::getGaimBlist() const
{
	return gaimBlist;
}

void
QGaimBuddyList::updateNode(GaimBlistNode *node)
{
	switch (node->type)
	{
		case GAIM_BLIST_GROUP_NODE:   updateGroup(node);   break;
		case GAIM_BLIST_CONTACT_NODE: updateContact(node); break;
		case GAIM_BLIST_BUDDY_NODE:   updateBuddy(node);   break;
		case GAIM_BLIST_CHAT_NODE:    updateChat(node);    break;
		default:
			return;
	}
}

void
QGaimBuddyList::reload(bool remove)
{
	GaimBlistNode *group, *cnode, *child;

	if (remove)
		clear();

	for (group = gaimBlist->root; group != NULL; group = group->next)
	{
		if (!GAIM_BLIST_NODE_IS_GROUP(group))
			continue;

		group->ui_data = NULL;

		updateNode(group);

		for (cnode = group->child; cnode != NULL; cnode = cnode->next)
		{
			if (GAIM_BLIST_NODE_IS_CONTACT(cnode))
			{
				cnode->ui_data = NULL;

				updateNode(cnode);

				for (child = cnode->child; child != NULL; child = child->next)
				{
					child->ui_data = NULL;

					updateNode(child);
				}
			}
			else if (GAIM_BLIST_NODE_IS_CHAT(cnode))
			{
				cnode->ui_data = NULL;

				updateNode(cnode);
			}
		}
	}
}

GaimBuddy *
QGaimBuddyList::getSelectedBuddy() const
{
	QGaimBListItem *item;
	GaimBlistNode *node;
	GaimBuddy *buddy = NULL;

	if ((item = (QGaimBListItem *)selectedItem()) == NULL)
		return NULL;

	if ((node = item->getBlistNode()) == NULL)
		return NULL;

	if (GAIM_BLIST_NODE_IS_BUDDY(node))
		buddy = (GaimBuddy *)node;
	else if (GAIM_BLIST_NODE_IS_CONTACT(node))
		buddy = gaim_contact_get_priority_buddy((GaimContact *)node);

	return buddy;
}

void
QGaimBuddyList::populateBuddyMenu(GaimBuddy *buddy, QPopupMenu *menu,
								  bool asContact)
{
	GaimPlugin *prpl = NULL;
	GaimPluginProtocolInfo *prplInfo = NULL;
	QGaimAction *a;

	prpl = gaim_find_prpl(gaim_account_get_protocol(buddy->account));

	if (prpl != NULL)
		prplInfo = GAIM_PLUGIN_PROTOCOL_INFO(prpl);

	/* Get User Info */
	if (prplInfo != NULL && prplInfo->get_info != NULL)
	{
		a = new QGaimAction(tr("Get Information"),
							QIconSet(Resource::loadPixmap("gaim/info")),
							QString::null, 0, this, 0, false, buddy);
		a->addTo(menu);

		connect(a, SIGNAL(activated(void *)),
				this, SLOT(userInfoSlot(void *)));
	}

	/* IM */
	a = new QGaimAction(tr("IM"),
						QIconSet(Resource::loadPixmap("gaim/send-im")),
						QString::null, 0, this, 0, false, buddy);
	a->addTo(menu);

	connect(a, SIGNAL(activated(void *)),
			this, SLOT(sendImSlot(void *)));

	/* Put in the protocol-specific actions */
	if (prplInfo != NULL && prplInfo->buddy_menu != NULL)
	{
		GList *l;
		GaimConnection *gc = gaim_account_get_connection(buddy->account);

		for (l = prplInfo->buddy_menu(gc, buddy->name);
			 l != NULL;
			 l = l->next)
		{
			struct proto_buddy_menu *pbm;

			pbm = (struct proto_buddy_menu *)l->data;

			a = new QGaimAction(pbm->label, QString::null, 0, this, 0,
								false, buddy);
			a->setUserData2(pbm);
			a->addTo(menu);

			connect(a, SIGNAL(activated(void *, void *)),
					this, SLOT(protoActionSlot(void *, void *)));
		}
	}

	/* Separator */
	menu->insertSeparator();

	/* Alias */
	a = new QGaimAction(tr("Alias"),
						QIconSet(Resource::loadPixmap("gaim/alias")),
						QString::null, 0, this, 0, false, buddy);
	a->addTo(menu);

	connect(a, SIGNAL(activated(void *)),
			this, SLOT(aliasBuddySlot(void *)));

	/* Remove */
	a = new QGaimAction(tr("Remove"),
						QIconSet(Resource::loadPixmap("gaim/remove")),
						QString::null, 0, this, 0, false, buddy);
	a->addTo(menu);

	connect(a, SIGNAL(activated(void *)),
			this, SLOT(removeBuddySlot(void *)));

	if (asContact)
	{
		GaimBlistNode *cnode = ((GaimBlistNode *)buddy)->parent;

		/* Separator */
		menu->insertSeparator();

		/* Expand */
		a = new QGaimAction(tr("Expand"),
							QIconSet(Resource::loadPixmap("gaim/expand")),
							QString::null, 0, this, 0);
		a->addTo(menu);

		connect(a, SIGNAL(activated()),
				this, SLOT(expandContactSlot()));

		if (cnode->child->next != NULL)
		{
			GaimBlistNode *bnode;
			bool showOffline =
				gaim_prefs_get_bool("/gaim/qpe/blist/show_offline_buddies");

			/* List of other accounts */
			for (bnode = cnode->child; bnode != NULL; bnode = bnode->next)
			{
				GaimBuddy *buddy2 = (GaimBuddy *)bnode;

				if (buddy2 == buddy)
					continue;

				if (!gaim_account_is_connected(buddy2->account))
					continue;

				if (!showOffline && GAIM_BUDDY_IS_ONLINE(buddy2))
				{
					QPopupMenu *submenu = new QPopupMenu(this);

					populateBuddyMenu(buddy2, submenu, false);

					menu->insertItem(
						QGaimBuddyList::getBuddyStatusIcon(bnode),
						buddy2->name, submenu);
				}
			}
		}
	}
}

void
QGaimBuddyList::populateContactMenu(GaimContact *contact, QPopupMenu *menu)
{
	GaimPlugin *prpl = NULL;
	GaimPluginProtocolInfo *prplInfo = NULL;
	QAction *a;

	prpl = gaim_find_prpl(gaim_account_get_protocol(
			gaim_contact_get_priority_buddy(contact)->account));

	if (prpl != NULL)
		prplInfo = GAIM_PLUGIN_PROTOCOL_INFO(prpl);

	/* Collapse */
	a = new QAction(tr("Collapse"),
					QIconSet(Resource::loadPixmap("gaim/collapse")),
					QString::null, 0, this, 0);
	a->addTo(menu);

	connect(a, SIGNAL(activated()),
			this, SLOT(collapseContactSlot()));

	/* Remove */
	a = new QAction(tr("Remove"),
					QIconSet(Resource::loadPixmap("gaim/remove")),
					QString::null, 0, this, 0);
	a->addTo(menu);

	connect(a, SIGNAL(activated()),
			this, SLOT(removeContactSlot()));
}

void
QGaimBuddyList::populateChatMenu(GaimChat *chat, QPopupMenu *menu)
{
	GaimPlugin *prpl = NULL;
	GaimPluginProtocolInfo *prplInfo = NULL;
	const char *autojoin;
	QAction *a;

	prpl = gaim_find_prpl(gaim_account_get_protocol(chat->account));

	if (prpl != NULL)
		prplInfo = GAIM_PLUGIN_PROTOCOL_INFO(prpl);

	/* Join Chat */
	a = new QAction(tr("Join Chat"),
					QIconSet(Resource::loadPixmap("gaim/join_chat")),
					QString::null, 0, this, 0);
	a->addTo(menu);

	connect(a, SIGNAL(activated()),
			this, SLOT(joinChatSlot()));

	/* Auto-Join */
	a = new QAction(tr("Auto-Join"), QString::null, 0, this, 0, true);

	autojoin = gaim_chat_get_setting(chat, "qpe-autojoin");

	if (!strcmp(autojoin, "true"))
		a->setOn(true);

	a->addTo(menu);

	connect(a, SIGNAL(toggled(bool)),
			this, SLOT(autoJoinChatSlot(bool)));

	/* Alias */
	a = new QAction(tr("Alias"),
					QIconSet(Resource::loadPixmap("gaim/alias")),
					QString::null, 0, this, 0);
	a->addTo(menu);

	connect(a, SIGNAL(activated()),
			this, SLOT(aliasChatSlot()));

	/* Remove */
	a = new QAction(tr("Remove Chat"),
					QIconSet(Resource::loadPixmap("gaim/remove")),
					QString::null, 0, this, 0);
	a->addTo(menu);

	connect(a, SIGNAL(activated()),
			this, SLOT(removeChatSlot()));
}

void
QGaimBuddyList::populateGroupMenu(GaimGroup *, QPopupMenu *menu)
{
	QAction *a;

	/* Add a Buddy */
	a = new QAction(tr("Add a Buddy"),
					QIconSet(Resource::loadPixmap("gaim/add")),
					QString::null, 0, this, 0);
	a->addTo(menu);

	connect(a, SIGNAL(activated()),
			this, SLOT(addBuddySlot()));

	/* Add a Chat */
	a = new QAction(tr("Add a Chat"),
					QIconSet(Resource::loadPixmap("gaim/add")),
					QString::null, 0, this, 0);
	a->addTo(menu);

	connect(a, SIGNAL(activated()),
			this, SLOT(addChatSlot()));

	/* Delete Group */
	a = new QAction(tr("Remove Group"),
					QIconSet(Resource::loadPixmap("gaim/remove")),
					QString::null, 0, this, 0);
	a->addTo(menu);

	connect(a, SIGNAL(activated()),
			this, SLOT(removeGroupSlot()));

	/* Rename */
	a = new QAction(tr("Rename"), QString::null, 0, this, 0);
	a->addTo(menu);

	connect(a, SIGNAL(activated()),
			this, SLOT(renameGroupSlot()));
}

void
QGaimBuddyList::nodeExpandedSlot(QListViewItem *_item)
{
	QGaimBListItem *item = (QGaimBListItem *)_item;
	GaimBlistNode *node;

	node = item->getBlistNode();

	if (GAIM_BLIST_NODE_IS_GROUP(node))
	{
		gaim_group_set_setting((GaimGroup *)node, "collapsed", NULL);

		if (!saveTimer->isActive())
			saveTimer->start(2000, true);
	}
	else if (GAIM_BLIST_NODE_IS_CONTACT(node))
	{

	}
}

void
QGaimBuddyList::nodeCollapsedSlot(QListViewItem *_item)
{
	QGaimBListItem *item = (QGaimBListItem *)_item;
	GaimBlistNode *node;

	node = item->getBlistNode();

	if (GAIM_BLIST_NODE_IS_GROUP(node))
	{
		gaim_group_set_setting((GaimGroup *)node, "collapsed", "true");

		if (!saveTimer->isActive())
			saveTimer->start(2000, true);
	}
	else if (GAIM_BLIST_NODE_IS_CONTACT(node))
	{
		collapseContactSlot(item);
	}
}

void
QGaimBuddyList::collapseContactSlot()
{
	collapseContactSlot((QGaimBListItem *)selectedItem());
}

void
QGaimBuddyList::collapseContactSlot(QGaimBListItem *item)
{
	GaimBlistNode *node;

	g_return_if_fail(item != NULL);

	node = item->getBlistNode();

	g_return_if_fail(GAIM_BLIST_NODE_IS_CONTACT(node));

	item->setExpanded(false);
	item->setExpandable(false);

	while (item->firstChild())
		delete item->firstChild();

	item->updateInfo();
}

void
QGaimBuddyList::expandContactSlot()
{
	expandContactSlot((QGaimBListItem *)selectedItem());
}

void
QGaimBuddyList::expandContactSlot(QGaimBListItem *item)
{
	GaimBlistNode *node, *child;

	g_return_if_fail(item != NULL);

	node = item->getBlistNode();

	g_return_if_fail(GAIM_BLIST_NODE_IS_CONTACT(node));

	item->setExpanded(true);
	item->setExpandable(true);
	item->setOpen(true);

	for (child = node->child; child != NULL; child = child->next)
	{
		child->ui_data = NULL;

		updateNode(child);
	}

	item->updateInfo();
}

void
QGaimBuddyList::saveBlistSlot()
{
	gaim_blist_save();
}

void
QGaimBuddyList::showContextMenuSlot(QListViewItem *_item,
									const QPoint &point, int)
{
	QGaimBListItem *item = (QGaimBListItem *)_item;
	QPopupMenu *menu = NULL;
	GaimBlistNode *node;

	if (item == NULL)
		return;

	node = item->getBlistNode();

	menu = new QPopupMenu(this);

	if (GAIM_BLIST_NODE_IS_BUDDY(node))
	{
		populateBuddyMenu((GaimBuddy *)node, menu, false);
	}
	else if (GAIM_BLIST_NODE_IS_CONTACT(node))
	{
		GaimContact *contact = (GaimContact *)node;

		if (item->isExpanded())
		{
			populateContactMenu(contact, menu);
		}
		else
		{
			populateBuddyMenu(gaim_contact_get_priority_buddy(contact), menu,
							  true);
		}
	}
	else if (GAIM_BLIST_NODE_IS_CHAT(node))
	{
		populateChatMenu((GaimChat *)node, menu);
	}
	else if (GAIM_BLIST_NODE_IS_GROUP(node))
	{
		populateGroupMenu((GaimGroup *)node, menu);
	}
	else
	{
		delete menu;

		return;
	}

	menu->popup(point);
}

void
QGaimBuddyList::addBuddySlot()
{
	GaimBlistNode *node;
	QGaimBListItem *item;

	if ((item = (QGaimBListItem *)selectedItem()) == NULL)
		return;

	node = item->getBlistNode();

	if (!GAIM_BLIST_NODE_IS_GROUP(node))
		return;

	emit addBuddy((GaimGroup *)node);
}

void
QGaimBuddyList::addChatSlot()
{
	GaimBlistNode *node;
	QGaimBListItem *item;

	if ((item = (QGaimBListItem *)selectedItem()) == NULL)
		return;

	node = item->getBlistNode();

	if (!GAIM_BLIST_NODE_IS_GROUP(node))
		return;

	emit addChat((GaimGroup *)node);
}

void
QGaimBuddyList::removeGroupSlot()
{
	GaimBlistNode *node;
	QGaimBListItem *item;

	if ((item = (QGaimBListItem *)selectedItem()) == NULL)
		return;

	node = item->getBlistNode();

	if (!GAIM_BLIST_NODE_IS_GROUP(node))
		return;

	emit removeGroup((GaimGroup *)node);
}

static void
renameGroupCb(GaimGroup *group, const char *newName)
{
	gaim_blist_rename_group(group, newName);
	gaim_blist_save();
}

void
QGaimBuddyList::renameGroupSlot()
{
	GaimBlistNode *node;
	QGaimBListItem *item;
	GaimGroup *group;

	if ((item = (QGaimBListItem *)selectedItem()) == NULL)
		return;

	node = item->getBlistNode();

	if (!GAIM_BLIST_NODE_IS_GROUP(node))
		return;

	group = (GaimGroup *)node;

	gaim_request_input(NULL, tr("Rename Group"),
					   tr("Rename Group"),
					   tr("Please enter a new name for the selected group."),
					   group->name, FALSE, FALSE,
					   tr("Rename"), G_CALLBACK(renameGroupCb),
					   tr("Cancel"), NULL, group);
}

void
QGaimBuddyList::userInfoSlot(void *data)
{
	GaimBuddy *buddy = (GaimBuddy *)data;

	serv_get_info(gaim_account_get_connection(buddy->account), buddy->name);
}

void
QGaimBuddyList::sendImSlot(void *data)
{
	emit openIm((GaimBuddy *)data);
}

void
QGaimBuddyList::protoActionSlot(void *data1, void *data2)
{
	GaimBuddy *buddy = (GaimBuddy *)data1;
	struct proto_buddy_menu *pbm = (struct proto_buddy_menu *)data2;

	if (pbm->callback != NULL)
		pbm->callback(pbm->gc, buddy->name);
}

static void
aliasBuddyCb(GaimBuddy *buddy, const char *newAlias)
{
	gaim_blist_alias_buddy(buddy, newAlias);
	gaim_blist_save();
}

void
QGaimBuddyList::aliasBuddySlot(void *data)
{
	GaimBuddy *buddy = (GaimBuddy *)data;

	gaim_request_input(NULL, tr("Alias Buddy"),
			tr("Please enter an aliased name for %1.").arg(buddy->name),
			NULL,
			buddy->alias, false, false,
			tr("Alias"), G_CALLBACK(aliasBuddyCb),
			tr("Cancel"), NULL, buddy);
}

void
QGaimBuddyList::removeBuddySlot(void *data)
{
	emit removeBuddy((GaimBuddy *)data);
}

void
QGaimBuddyList::removeContactSlot()
{
	GaimBlistNode *node;
	QGaimBListItem *item;

	if ((item = (QGaimBListItem *)selectedItem()) == NULL)
		return;

	node = item->getBlistNode();

	if (!GAIM_BLIST_NODE_IS_CONTACT(node))
		return;

	emit removeContact((GaimContact *)node);
}

void
QGaimBuddyList::joinChatSlot()
{
	GaimBlistNode *node;
	QGaimBListItem *item;

	if ((item = (QGaimBListItem *)selectedItem()) == NULL)
		return;

	node = item->getBlistNode();

	if (!GAIM_BLIST_NODE_IS_CHAT(node))
		return;

	emit joinChat((GaimChat *)node);
}

void
QGaimBuddyList::autoJoinChatSlot(bool on)
{
	GaimBlistNode *node;
	QGaimBListItem *item;
	GaimChat *chat;

	if ((item = (QGaimBListItem *)selectedItem()) == NULL)
		return;

	node = item->getBlistNode();

	if (!GAIM_BLIST_NODE_IS_CHAT(node))
		return;

	chat = (GaimChat *)node;

	gaim_chat_set_setting(chat, "qpe-autojoin", (on ? "true" : NULL));
	gaim_blist_save();
}

void
QGaimBuddyList::removeChatSlot()
{
	GaimBlistNode *node;
	QGaimBListItem *item;

	if ((item = (QGaimBListItem *)selectedItem()) == NULL)
		return;

	node = item->getBlistNode();

	if (!GAIM_BLIST_NODE_IS_CHAT(node))
		return;

	emit removeChat((GaimChat *)node);
}

static void
aliasChatCb(GaimChat *chat, const char *newAlias)
{
	gaim_blist_alias_chat(chat, newAlias);
	gaim_blist_save();
}

void
QGaimBuddyList::aliasChatSlot()
{
	GaimBlistNode *node;
	QGaimBListItem *item;
	GaimChat *chat;

	if ((item = (QGaimBListItem *)selectedItem()) == NULL)
		return;

	node = item->getBlistNode();

	if (!GAIM_BLIST_NODE_IS_CHAT(node))
		return;

	chat = (GaimChat *)node;

	gaim_request_input(NULL, tr("Alias Chat"),
					   tr("Please enter an aliased name for this chat."),
					   NULL,
					   chat->alias, false, false,
					   tr("Alias"), G_CALLBACK(aliasChatCb),
					   tr("Cancel"), NULL, chat);
}

void
QGaimBuddyList::addGroup(GaimBlistNode *node)
{
	QGaimBListItem *item = new QGaimBListItem(this, node);
	node->ui_data = item;

	item->setExpandable(true);
}

void
QGaimBuddyList::updateGroup(GaimBlistNode *node)
{
	GaimGroup *group;
	QGaimBListItem *item;

	g_return_if_fail(GAIM_BLIST_NODE_IS_GROUP(node));

	item = (QGaimBListItem *)node->ui_data;
	group = (GaimGroup *)node;

	if (gaim_prefs_get_bool("/gaim/qpe/blist/show_empty_groups")    ||
		gaim_prefs_get_bool("/gaim/qpe/blist/show_offline_buddies") ||
		gaim_blist_get_group_online_count(group) > 0)
	{
		if (item == NULL)
		{
			addGroup(node);
			item = (QGaimBListItem *)node->ui_data;
		}

		if (!gaim_group_get_setting(group, "collapsed"))
			item->setOpen(true);
	}
	else
	{
		if (item != NULL)
			delete item;
	}
}

void
QGaimBuddyList::updateContact(GaimBlistNode *node)
{
	GaimContact *contact;
	GaimBuddy *buddy;
	QGaimBListItem *item;

	g_return_if_fail(GAIM_BLIST_NODE_IS_CONTACT(node));

	updateGroup(node->parent);

	item    = (QGaimBListItem *)node->ui_data;
	contact = (GaimContact *)node;
	buddy   = gaim_contact_get_priority_buddy(contact);

	if (buddy != NULL &&
		(buddy->present != GAIM_BUDDY_OFFLINE ||
		 (gaim_account_is_connected(buddy->account) &&
		  gaim_prefs_get_bool("/gaim/qpe/blist/show_offline_buddies"))))
	{
		if (item == NULL)
		{
			node->ui_data = item = new QGaimBListItem(
				(QGaimBListItem *)(node->parent->ui_data), node);
		}
		else
			item->updateInfo();
	}
	else if (item != NULL)
		delete item;
}

void
QGaimBuddyList::updateBuddy(GaimBlistNode *node)
{
	GaimContact *contact;
	GaimBuddy *buddy;
	QGaimBListItem *item;

	g_return_if_fail(GAIM_BLIST_NODE_IS_BUDDY(node));

	updateContact(node->parent);

	if (node->parent->ui_data == NULL ||
		(node->parent->ui_data != NULL &&
		 !((QGaimBListItem *)node->parent->ui_data)->isExpanded()))
	{
		return;
	}

	buddy   = (GaimBuddy *)node;
	contact = (GaimContact *)node->parent;
	item    = (QGaimBListItem *)node->ui_data;

	if (buddy->present != GAIM_BUDDY_OFFLINE ||
		(gaim_account_is_connected(buddy->account) &&
		 gaim_prefs_get_bool("/gaim/qpe/blist/show_offline_buddies")))
	{
		if (item == NULL)
		{
			node->ui_data = item = new QGaimBListItem(
				(QGaimBListItem *)(node->parent->ui_data), node);
		}
		else
			item->updateInfo();
	}
	else if (item != NULL)
		delete item;
}

void
QGaimBuddyList::updateChat(GaimBlistNode *node)
{
	GaimChat *chat;
	QGaimBListItem *item;

	g_return_if_fail(GAIM_BLIST_NODE_IS_CHAT(node));

	updateGroup(node->parent);

	chat = (GaimChat *)node;
	item = (QGaimBListItem *)node->ui_data;

	if (gaim_account_is_connected(chat->account))
	{
		if (item == NULL)
		{
			node->ui_data = item = new QGaimBListItem(
				(QGaimBListItem *)(node->parent->ui_data), node);
		}
		else
			item->updateInfo();
	}
	else if (item != NULL)
		delete item;
}
