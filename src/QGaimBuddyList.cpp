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
#include "QGaimBuddyList.h"
#include "QGaimProtocolUtils.h"
#include "QGaimImageUtils.h"

#include <libgaim/debug.h>
#include <libgaim/multi.h>
#include <libgaim/prefs.h>

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
			if (buddy->idle > 0)
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

		if (buddy->idle > 0)
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
	}
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
QGaimBuddyList::getBuddyStatusIcon(GaimBlistNode *node)
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

	statusImage = statusImage.smoothScale(16, 16);

	if (statusImage.isNull())
		return QPixmap();

	if (se != NULL)
	{
		emblemImage = Resource::loadImage("gaim/status/" + QString(se));

		if (!emblemImage.isNull())
		{
			QGaimImageUtils::blendOnLower(
					statusImage.width()  - emblemImage.width(),
					statusImage.height() - emblemImage.height(),
					emblemImage, statusImage);
			//QGaimImageUtils::blendOnLower(5, 5, emblemImage, statusImage);
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
			this, SLOT(nodeExpanded(QListViewItem *)));
	connect(this, SIGNAL(collapsed(QListViewItem *)),
			this, SLOT(nodeCollapsed(QListViewItem *)));
	connect(this, SIGNAL(rightButtonPressed(QListViewItem *,
											const QPoint &, int)),
			this, SLOT(showContextMenu(QListViewItem *, const QPoint &, int)));

	saveTimer = new QTimer(this);

	connect(saveTimer, SIGNAL(timeout()),
			this, SLOT(saveBlist()));
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

void
QGaimBuddyList::nodeExpanded(QListViewItem *_item)
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
QGaimBuddyList::nodeCollapsed(QListViewItem *_item)
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
		collapseContact(item);
	}
}

void
QGaimBuddyList::showContextMenu(QListViewItem *_item,
								const QPoint &point, int)
{
	QGaimBListItem *item = (QGaimBListItem *)_item;
	QPopupMenu *menu = NULL;
	QAction *a;
	GaimBlistNode *node;

	if (item == NULL)
		return;

	node = item->getBlistNode();

	menu = new QPopupMenu(this);

	if (GAIM_BLIST_NODE_IS_BUDDY(node))
	{
		/* Get User Info */
		a = new QAction(tr("Get Information"),
						QIconSet(Resource::loadPixmap("gaim/info")),
						QString::null, 0, this, 0);
		a->addTo(menu);
		a->setEnabled(false);

		/* IM */
		a = new QAction(tr("IM"),
						QIconSet(Resource::loadPixmap("gaim/send-im")),
						QString::null, 0, this, 0);
		a->addTo(menu);

		connect(a, SIGNAL(activated()),
				this, SLOT(sendIm()));

		/* Separator */
		menu->insertSeparator();

		/* Remove */
		a = new QAction(tr("Remove"),
						QIconSet(Resource::loadPixmap("gaim/remove")),
						QString::null, 0, this, 0);
		a->addTo(menu);

		connect(a, SIGNAL(activated()),
				this, SLOT(showRemoveBuddy()));
	}
	else if (GAIM_BLIST_NODE_IS_CONTACT(node))
	{
		if (item->isExpanded())
		{
			/* Collapse */
			a = new QAction(tr("Collapse"),
							QIconSet(Resource::loadPixmap("gaim/collapse")),
							QString::null, 0, this, 0);
			a->addTo(menu);

			connect(a, SIGNAL(activated()),
					this, SLOT(collapseContact()));

			/* Remove */
			a = new QAction(tr("Remove"),
							QIconSet(Resource::loadPixmap("gaim/remove")),
							QString::null, 0, this, 0);
			a->addTo(menu);

			connect(a, SIGNAL(activated()),
					this, SLOT(showRemoveContact()));
		}
		else
		{
			/* Get User Info */
			a = new QAction(tr("Get Information"),
							QIconSet(Resource::loadPixmap("gaim/info")),
							QString::null, 0, this, 0);
			a->addTo(menu);
			a->setEnabled(false);

			/* IM */
			a = new QAction(tr("IM"),
							QIconSet(Resource::loadPixmap("gaim/send-im")),
							QString::null, 0, this, 0);
			a->addTo(menu);

			connect(a, SIGNAL(activated()),
					this, SLOT(sendIm()));

			/* Separator */
			menu->insertSeparator();

			/* Remove */
			a = new QAction(tr("Remove"),
							QIconSet(Resource::loadPixmap("gaim/remove")),
							QString::null, 0, this, 0);
			a->addTo(menu);

			connect(a, SIGNAL(activated()),
					this, SLOT(showRemoveBuddy()));

			/* Separator */
			menu->insertSeparator();

			/* Expand */
			a = new QAction(tr("Expand"),
							QIconSet(Resource::loadPixmap("gaim/expand")),
							QString::null, 0, this, 0);
			a->addTo(menu);

			connect(a, SIGNAL(activated()),
					this, SLOT(expandContact()));
		}
	}
	else
	{
		delete menu;

		return;
	}

	menu->popup(point);
}

void
QGaimBuddyList::collapseContact()
{
	collapseContact((QGaimBListItem *)selectedItem());
}

void
QGaimBuddyList::collapseContact(QGaimBListItem *item)
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
QGaimBuddyList::expandContact()
{
	expandContact((QGaimBListItem *)selectedItem());
}

void
QGaimBuddyList::expandContact(QGaimBListItem *item)
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
QGaimBuddyList::saveBlist()
{
	gaim_blist_save();
}

void
QGaimBuddyList::addGroup(GaimBlistNode *node)
{
	QGaimBListItem *item = new QGaimBListItem(this, node);
	node->ui_data = item;

	item->setText(0, ((GaimGroup *)node)->name);

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

	if (node->parent->ui_data != NULL &&
		!((QGaimBListItem *)node->parent->ui_data)->isExpanded())
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
