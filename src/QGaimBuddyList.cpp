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

#include <qheader.h>


/**************************************************************************
 * QGaimBListItem
 **************************************************************************/
QGaimBListItem::QGaimBListItem(QListView *parent, GaimBlistNode *node)
	: QListViewItem(parent), node(node)
{
	init();
}

QGaimBListItem::QGaimBListItem(QListViewItem *parent, GaimBlistNode *node)
	: QListViewItem(parent), node(node)
{
	init();
}

QGaimBListItem::~QGaimBListItem()
{
}

GaimBlistNode *
QGaimBListItem::getBlistNode() const
{
	return node;
}

void
QGaimBListItem::updateInfo()
{
	if (GAIM_BLIST_NODE_IS_BUDDY(node))
	{
		struct buddy *buddy = (struct buddy *)node;

		if (buddy->idle > 0)
		{
			time_t t;
			int ihrs, imin;
			char *idle;

			time(&t);

			ihrs = (t - ((struct buddy *)node)->idle) / 3600;
			imin = ((t - ((struct buddy*)node)->idle) / 60) % 60;

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
}

void
QGaimBListItem::paintBranches(QPainter *p, const QColorGroup &cg,
							  int width, int, int height, GUIStyle)
{
	p->fillRect(0, 0, width, height, QBrush(cg.base()));
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

	if (GAIM_BLIST_NODE_IS_BUDDY(node))
		prpl = gaim_find_prpl(gaim_account_get_protocol(
				((struct buddy *)node)->account));
	else if (GAIM_BLIST_NODE_IS_CHAT(node))
		prpl = gaim_find_prpl(gaim_account_get_protocol(
				((struct chat *)node)->account));

	if (prpl == NULL)
		return QPixmap();

	prplInfo = GAIM_PLUGIN_PROTOCOL_INFO(prpl);

	if (prplInfo->list_icon != NULL)
	{
		if (GAIM_BLIST_NODE_IS_BUDDY(node))
			protoName = prplInfo->list_icon(((struct buddy *)node)->account,
											(struct buddy *)node);
		else if (GAIM_BLIST_NODE_IS_CHAT(node))
			protoName = prplInfo->list_icon(((struct chat *)node)->account,
											NULL);
	}

	if (GAIM_BLIST_NODE_IS_BUDDY(node) &&
		((struct buddy *)node)->present != GAIM_BUDDY_SIGNING_OFF &&
		prplInfo->list_emblems != NULL)
	{
		prplInfo->list_emblems((struct buddy *)node, &se, &sw, &nw, &ne);
	}

	if (se == NULL)
	{
		if      (sw != NULL) se = sw;
		else if (ne != NULL) se = ne;
		else if (nw != NULL) se = nw;
	}

	sw = nw = ne = NULL; /* So that only the se icon will composite. */

	if (GAIM_BLIST_NODE_IS_BUDDY(node) &&
		((struct buddy *)node)->present == GAIM_BUDDY_SIGNING_ON)
	{
		statusImage = Resource::loadImage("gaim/status/small/login");
	}
	else if (GAIM_BLIST_NODE_IS_BUDDY(node) &&
			 ((struct buddy *)node)->present == GAIM_BUDDY_SIGNING_OFF)
	{
		statusImage = Resource::loadImage("gaim/status/small/logout");
	}
	else
	{
		statusImage = Resource::loadImage("gaim/protocols/small/" +
										  QString(protoName));
	}

	if (statusImage.isNull())
		return QPixmap();

	if (se != NULL)
	{
		emblemImage = Resource::loadImage("gaim/status/small/" + QString(se));

		if (!emblemImage.isNull())
			QGaimImageUtils::blendOnLower(5, 5, emblemImage, statusImage);
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
	setAllColumnsShowFocus(TRUE);
	setRootIsDecorated(true);

	header()->hide();

	QPEApplication::setStylusOperation(viewport(),
									   QPEApplication::RightOnHold);
}

QGaimBuddyList::~QGaimBuddyList()
{
}

void
QGaimBuddyList::setGaimBlist(struct gaim_buddy_list *list)
{
	gaimBlist = list;
}

struct gaim_buddy_list *
QGaimBuddyList::getGaimBlist() const
{
	return gaimBlist;
}

void
QGaimBuddyList::updateNode(GaimBlistNode *node)
{
	QGaimBListItem *item = (QGaimBListItem *)node->ui_data;
	bool expand = false;
	bool new_entry = true;
	bool show_offline_buddies;
	bool show_empty_groups;

	show_offline_buddies =
		gaim_prefs_get_bool("/gaim/qpe/blist/show_offline_buddies");
	show_empty_groups =
		gaim_prefs_get_bool("/gaim/qpe/blist/show_empty_groups");

	if (item == NULL)
	{
		new_entry = true;

		if (GAIM_BLIST_NODE_IS_BUDDY(node))
		{
			struct buddy *buddy = (struct buddy *)node;

			if (buddy->present != GAIM_BUDDY_OFFLINE ||
				(gaim_account_is_connected(buddy->account) &&
				 show_offline_buddies))
			{
				char *collapsed =
					gaim_group_get_setting((struct group *)node->parent,
										   "collapsed");

				if (node->parent != NULL && node->parent->ui_data == NULL)
				{
					/* Add the buddy's group. */
					addGroup(node->parent);
				}

				expand = !collapsed;

				g_free(collapsed);

				/* Add the buddy here. */
				node->ui_data = new QGaimBListItem(
					(QGaimBListItem *)(node->parent->ui_data), node);
			}
		}
		else if (GAIM_BLIST_NODE_IS_CHAT(node) &&
				 gaim_account_is_connected(((struct chat *)node)->account))
		{
			char *collapsed =
				gaim_group_get_setting((struct group *)node->parent,
									   "collapsed");

			if (node->parent != NULL && node->parent->ui_data == NULL)
			{
				/* Add the chat's group. */
				addGroup(node->parent);
			}

			expand = !collapsed;

			g_free(collapsed);

			/* Add the chat here. */
			node->ui_data = new QGaimBListItem(
				(QGaimBListItem *)node->parent->ui_data, node);
		}
		else if (GAIM_BLIST_NODE_IS_GROUP(node) && show_empty_groups)
		{
			addGroup(node);
			expand = true;
		}
	}
	else if (GAIM_BLIST_NODE_IS_GROUP(node))
	{
		if (gaim_blist_get_group_online_count((struct group *)node) == 0 &&
			!show_empty_groups && !show_offline_buddies)
		{
			item = (QGaimBListItem *)node->ui_data;

			delete item;
			node->ui_data = NULL;
		}
		else
		{
			addGroup(node);
		}
	}

	item = (QGaimBListItem *)node->ui_data;

	if (item == NULL)
		return;

	if (GAIM_BLIST_NODE_IS_CHAT(node))
	{
		struct chat *chat = (struct chat *)node;

		if (gaim_account_is_connected(chat->account))
		{
			if (chat->alias)
				item->setText(0, chat->alias);
			else
			{
				GaimConnection *gc;
				struct proto_chat_entry *pce;
				GList *parts, *tmp;

				gc = gaim_account_get_connection(chat->account);

				parts = GAIM_PLUGIN_PROTOCOL_INFO(gc->prpl)->chat_info(gc);
				pce = (struct proto_chat_entry *)parts->data;
				item->setText(0, (char *)g_hash_table_lookup(chat->components,
															 pce->identifier));

				for (tmp = parts; tmp != NULL; tmp = tmp->next)
					g_free(tmp->data);

				g_list_free(parts);
			}
		}
		else
		{
			delete item;
			node->ui_data = NULL;
		}
	}
	else if (GAIM_BLIST_NODE_IS_BUDDY(node))
	{
		struct buddy *buddy = (struct buddy *)node;

		if (buddy->present != GAIM_BUDDY_OFFLINE ||
			(gaim_account_is_connected(buddy->account) && show_offline_buddies))
		{
			item->updateInfo();
		}
		else
		{
			delete item;
			node->ui_data = NULL;
		}
	}
	else if (GAIM_BLIST_NODE_IS_BUDDY(node) && !new_entry)
	{
		delete item;
		node->ui_data = NULL;
	}

	if (expand)
		item->setOpen(true);
}

void
QGaimBuddyList::reload(bool remove)
{
	GaimBlistNode *group, *child;

	if (remove)
		clear();

	for (group = gaimBlist->root; group != NULL; group = group->next)
	{
		if (!GAIM_BLIST_NODE_IS_GROUP(group))
			continue;

		group->ui_data = NULL;

		updateNode(group);

		for (child = group->child; child != NULL; child = child->next)
		{
			child->ui_data = NULL;

			updateNode(child);
		}
	}
}

void
QGaimBuddyList::addGroup(GaimBlistNode *node)
{
	QGaimBListItem *item = new QGaimBListItem(this, node);
	node->ui_data = item;

	item->setText(0, ((struct group *)node)->name);

	item->setExpandable(true);
}
