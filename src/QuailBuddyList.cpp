/**
 * @file QQuailBuddyList.cpp Buddy list widget
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
#include "QuailAction.h"
#include "QuailBuddyList.h"
#include "QuailProtocolUtils.h"
#include "QuailImageUtils.h"

#include <libpurple/debug.h>
#include <libpurple/multi.h>
#include <libpurple/prefs.h>
#include <libpurple/request.h>
#include <libpurple/server.h>

#include <qpe/qpeapplication.h>
#include <qpe/resource.h>

#include <qaction.h>
#include <qheader.h>
#include <qpopupmenu.h>
#include <qtimer.h>


/**************************************************************************
 * QQuailBListItem
 **************************************************************************/
QQuailBListItem::QQuailBListItem(QListView *parent, GaimBlistNode *node)
	: QListViewItem(parent), node(node), expanded(false), dirty(true)
{
	init();
}

QQuailBListItem::QQuailBListItem(QListViewItem *parent, GaimBlistNode *node)
	: QListViewItem(parent), node(node), expanded(false), dirty(true)
{
	init();
}

QQuailBListItem::~QQuailBListItem()
{
	if (node != NULL)
		node->ui_data = NULL;
}

GaimBlistNode *
QQuailBListItem::getBlistNode() const
{
	return node;
}

void
QQuailBListItem::updateInfo()
{
	QQuailPixmapSize pixmapSize;

	if (gaim_prefs_get_bool("/gaim/qpe/blist/show_large_icons"))
		pixmapSize = QGAIM_PIXMAP_LARGE;
	else
		pixmapSize = QGAIM_PIXMAP_SMALL;

	dirty = true;

	if (GAIM_BLIST_NODE_IS_CONTACT(node))
	{
		PurpleContact *contact = (PurpleContact *)node;
		GaimBuddy *buddy = gaim_contact_get_priority_buddy(contact);

		if (buddy == NULL)
			return;

		if (isExpanded())
		{
			QImage image = Resource::loadPixmap("gaim").convertToImage();
			QPixmap pixmap;

			pixmap.convertFromImage(pixmapSize == QGAIM_PIXMAP_SMALL
									? image.smoothScale(16, 16)
									: image.smoothScale(32, 32));

			setPixmap(0, pixmap);
		}
		else
		{
			QString text = "";

			if (buddy->evil > 0)
				text += QString("%1%").arg(buddy->evil);

			if (buddy->idle > 0 &&
				gaim_prefs_get_bool("/gaim/qpe/blist/show_idle_times"))
			{
				time_t t;
				int ihrs, imin;
				QString idle;

				time(&t);

				ihrs = (t - buddy->idle) / 3600;
				imin = ((t - buddy->idle) / 60) % 60;

				if (ihrs > 0)
					idle = QString("(%1:%2)").arg(ihrs).arg(imin, 2);
				else
					idle = QString("(%1)").arg(imin);

				if (text != "")
					text += "  ";

				text += idle;
			}

			setPixmap(0,
				QQuailBuddyList::getBuddyStatusIcon((GaimBlistNode *)buddy,
												   pixmapSize));
			setText(1, text);
		}

		setText(0, gaim_get_buddy_alias(buddy));
	}
	else if (GAIM_BLIST_NODE_IS_BUDDY(node))
	{
		GaimBuddy *buddy = (GaimBuddy *)node;
		QString text = "";

		if (buddy->evil > 0)
			text += QString("%1%").arg(buddy->evil);

		if (buddy->idle > 0 &&
			gaim_prefs_get_bool("/gaim/qpe/blist/show_idle_times"))
		{
			time_t t;
			int ihrs, imin;
			QString idle;

			time(&t);

			ihrs = (t - ((GaimBuddy *)node)->idle) / 3600;
			imin = ((t - ((GaimBuddy*)node)->idle) / 60) % 60;

			if (ihrs > 0)
				idle = QString("(%1:%2)").arg(ihrs).arg(imin, 2);
			else
				idle = QString("(%1)").arg(imin);

			if (text != "")
				text += "  ";

			text += idle;
		}

		setPixmap(0, QQuailBuddyList::getBuddyStatusIcon(node, pixmapSize));
		setText(0, gaim_get_buddy_alias(buddy));
		setText(1, text);
	}
	else if (GAIM_BLIST_NODE_IS_CHAT(node))
	{
		PurpleChat *chat = (PurpleChat *)node;

		setPixmap(0, QQuailProtocolUtils::getProtocolIcon(chat->account,
														 pixmapSize));
		setText(0, gaim_chat_get_display_name(chat));
	}
}

void
QQuailBListItem::paintCell(QPainter *p, const QColorGroup &cg, int column,
						  int width, int align)
{
	QListView *lv = listView();
	const QPixmap *icon = pixmap(column);

	p->fillRect(0, 0, width, height(), cg.base());

	int itMarg = (lv == NULL ? 1 : lv->itemMargin());
	int lmarg = itMarg;

	if (isSelected() && (column == 0 || lv->allColumnsShowFocus()))
	{
		p->fillRect(0, 0, width, height(), cg.highlight());
		p->setPen(cg.highlightedText());
	}
	else
		p->setPen(cg.text());

	if (icon)
	{
		p->drawPixmap(lmarg, (height() - icon->height()) / 2, *icon);
		lmarg += icon->width() + itMarg;
	}

	if (GAIM_BLIST_NODE_IS_BUDDY(node) || GAIM_BLIST_NODE_IS_CONTACT(node))
		paintBuddyInfo(p, cg, column, width, align, lmarg, itMarg);
	else if (GAIM_BLIST_NODE_IS_GROUP(node))
		paintGroupInfo(p, cg, column, width, align, lmarg, itMarg);
	else
		QListViewItem::paintCell(p, cg, column, width, align);

	dirty = false;
}

void
QQuailBListItem::paintBranches(QPainter *p, const QColorGroup &cg,
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
QQuailBListItem::setExpanded(bool expanded)
{
	this->expanded = expanded;
}

bool
QQuailBListItem::isExpanded() const
{
	return expanded;
}

void
QQuailBListItem::init()
{
	updateInfo();
}

void
QQuailBListItem::paintBuddyInfo(QPainter *p, const QColorGroup &cg, int column,
							   int width, int align, int lmarg, int itMarg)
{
	GaimBuddy *buddy;
	PurpleContact *contact = NULL;

	if (GAIM_BLIST_NODE_IS_BUDDY(node))
		buddy = (GaimBuddy *)node;
	else
	{
		contact = (PurpleContact *)node;
		buddy = gaim_contact_get_priority_buddy(contact);
	}

	if (gaim_prefs_get_bool("/gaim/qpe/blist/show_large_icons"))
	{
		if (column > 0)
			return;

		if (!isExpanded())
		{
			if (dirty)
			{
				PurplePlugin *prpl = NULL;
				PurplePluginProtocolInfo *prplInfo = NULL;
				QRect topRect, bottomRect;
				QString statusText;
				QString idleTime;
				QString warning;

				prpl = gaim_plugins_find_with_id(
					gaim_account_get_protocol_id(buddy->account));

				if (prpl != NULL)
					prplInfo = GAIM_PLUGIN_PROTOCOL_INFO(prpl);

				if (prpl != NULL && prplInfo->status_text != NULL &&
					gaim_account_get_connection(buddy->account) != NULL)
				{
					char *tmp = prplInfo->status_text(buddy);

					if (tmp != NULL)
					{
						g_strdelimit(tmp, "\n", ' ');

						statusText = QString(tmp) + " ";

						g_free(tmp);
					}
				}

				if (!isExpanded() && buddy->idle > 0 &&
					gaim_prefs_get_bool("/gaim/qpe/blist/show_idle_times"))
				{
					time_t t;
					int ihrs, imin;

					time(&t);
					ihrs = (t - buddy->idle) / 3600;
					imin = ((t - buddy->idle) / 60) % 60;

					if (ihrs)
					{
						idleTime = QString().sprintf(
							QObject::tr("Idle (%dh%02dm) "), ihrs, imin);
					}
					else
						idleTime = QObject::tr("Idle (%1m) ").arg(imin);
				}

				if (!isExpanded() && buddy->evil > 0 &&
					gaim_prefs_get_bool("/gaim/qpe/blist/show_warning_levels"))
				{
					warning = QObject::tr("Warned (%1%) ").arg(buddy->evil);
				}

				if (!GAIM_BUDDY_IS_ONLINE(buddy) && statusText.isEmpty())
					statusText = QObject::tr("Offline ");

				if (contact != NULL && !isExpanded() && contact->alias != NULL)
					topText = contact->alias;
				else
					topText = gaim_get_buddy_alias(buddy);

				bottomText = statusText + idleTime + warning;

				/* Get the top rect info. */
				topRect = p->boundingRect(lmarg, 0, width - lmarg - itMarg,
										  height(), align | AlignVCenter,
										  topText);

				if (!bottomText.isEmpty())
				{
					bottomRect = p->boundingRect(lmarg, 0,
												 width - lmarg - itMarg,
												 height(),
												 align | AlignVCenter,
												 bottomText);
				}

				textY1 = (height() - topRect.height() -
						  bottomRect.height() - 2) / 2;
				textY2 = textY1 + topRect.height() + 2;
			}

			if (buddy->idle > 0 &&
				gaim_prefs_get_bool("/gaim/qpe/blist/dim_idle_buddies"))
			{
				p->setPen(cg.dark());
			}

			p->drawText(lmarg, textY1, width - lmarg - itMarg, height(),
						align, topText);

			p->setPen(cg.dark());
			p->drawText(lmarg, textY2, width - lmarg - itMarg, height(),
						align, bottomText);
		}
		else
		{
			QColorGroup _cg(cg);

			if (buddy->idle > 0 &&
				gaim_prefs_get_bool("/gaim/qpe/blist/dim_idle_buddies"))
			{
				_cg.setColor(QColorGroup::Text, cg.dark());
			}

			QListViewItem::paintCell(p, _cg, column, width, align);
		}
	}
	else
	{
		QColorGroup _cg(cg);

		if (buddy->idle > 0 &&
			gaim_prefs_get_bool("/gaim/qpe/blist/dim_idle_buddies"))
		{
			_cg.setColor(QColorGroup::Text, cg.dark());
		}

		QListViewItem::paintCell(p, _cg, column, width, align);
	}
}

void
QQuailBListItem::paintGroupInfo(QPainter *p, const QColorGroup &, int column,
							   int width, int align, int lmarg, int itMarg)
{
	if (column > 0)
		return;

	PurpleGroup *group = (PurpleGroup *)getBlistNode();
	QString groupName, detail;
	QFont f = p->font();

	groupName = group->name;

	if (gaim_prefs_get_bool("/gaim/qpe/blist/show_group_count"))
	{
		groupName += " ";
		detail = QString("(%1/%2)").arg(
			gaim_blist_get_group_online_count(group)).arg(
			gaim_blist_get_group_size(group, FALSE));
	}

	f.setBold(true);
	p->setFont(f);
	p->drawText(lmarg, 0, width - lmarg - itMarg, height(),
				align | AlignVCenter, groupName);

	QRect r = p->boundingRect(lmarg, 0, width - lmarg - itMarg,
							  height(),
							  align | AlignVCenter, groupName);

	f.setBold(false);
	p->setFont(f);
	p->drawText(lmarg + r.right(), 0, width - lmarg - itMarg, height(),
				align | AlignVCenter, detail);
}


/**************************************************************************
 * QQuailBuddyList static utility functions
 **************************************************************************/
QPixmap
QQuailBuddyList::getBuddyStatusIcon(GaimBlistNode *node, QQuailPixmapSize size)
{
	QImage statusImage;
	QImage emblemImage;
	QPixmap statusPixmap;
	const char *protoName = NULL;
	PurplePlugin *prpl = NULL;
	PurplePluginProtocolInfo *prplInfo = NULL;
	PurpleAccount *account;
	char *se = NULL, *sw = NULL, *nw = NULL, *ne = NULL;

	if (node == NULL)
		return QPixmap();

	if (GAIM_BLIST_NODE_IS_BUDDY(node))
		account = ((GaimBuddy *)node)->account;
	else if (GAIM_BLIST_NODE_IS_CHAT(node))
		account = ((PurpleChat *)node)->account;
	else
		return QPixmap();

	prpl = gaim_plugins_find_with_id(gaim_account_get_protocol_id(account));

	if (prpl == NULL)
		return QPixmap();

	prplInfo = GAIM_PLUGIN_PROTOCOL_INFO(prpl);

	if (prplInfo->list_icon != NULL)
	{
		if (GAIM_BLIST_NODE_IS_BUDDY(node))
			protoName = prplInfo->list_icon(account, (GaimBuddy *)node);
		else if (GAIM_BLIST_NODE_IS_CHAT(node))
			protoName = prplInfo->list_icon(account, NULL);
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

			QQuailImageUtils::blendOnLower(
					statusImage.width()  - emblemImage.width(),
					statusImage.height() - emblemImage.height(),
					emblemImage, statusImage);
		}
	}

	/* Grey idle buddies. */
	if (GAIM_BLIST_NODE_IS_BUDDY(node) &&
		((GaimBuddy *)node)->present == GAIM_BUDDY_OFFLINE)
	{
		QQuailImageUtils::greyImage(statusImage);
	}
	else if (GAIM_BLIST_NODE_IS_BUDDY(node) &&
			 ((GaimBuddy *)node)->idle)
	{
		QQuailImageUtils::saturate(statusImage, 0.25);
	}

	statusPixmap.convertFromImage(statusImage);

	return statusPixmap;
}

/**************************************************************************
 * QQuailBuddyList
 **************************************************************************/
QQuailBuddyList::QQuailBuddyList(QWidget *parent, const char *name)
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

QQuailBuddyList::~QQuailBuddyList()
{
	delete saveTimer;
}

void
QQuailBuddyList::setGaimBlist(GaimBuddyList *list)
{
	gaimBlist = list;
}

GaimBuddyList *
QQuailBuddyList::getGaimBlist() const
{
	return gaimBlist;
}

void
QQuailBuddyList::updateNode(GaimBlistNode *node)
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
QQuailBuddyList::reload(bool remove)
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
QQuailBuddyList::getSelectedBuddy() const
{
	QQuailBListItem *item;
	GaimBlistNode *node;
	GaimBuddy *buddy = NULL;

	if ((item = (QQuailBListItem *)selectedItem()) == NULL)
		return NULL;

	if ((node = item->getBlistNode()) == NULL)
		return NULL;

	if (GAIM_BLIST_NODE_IS_BUDDY(node))
		buddy = (GaimBuddy *)node;
	else if (GAIM_BLIST_NODE_IS_CONTACT(node))
		buddy = gaim_contact_get_priority_buddy((PurpleContact *)node);

	return buddy;
}

void
QQuailBuddyList::populateBuddyMenu(GaimBuddy *buddy, QPopupMenu *menu,
								  bool asContact)
{
	PurplePlugin *prpl = NULL;
	PurplePluginProtocolInfo *prplInfo = NULL;
	QQuailAction *a;

	prpl = gaim_plugins_find_with_id(
		gaim_account_get_protocol_id(buddy->account));

	if (prpl != NULL)
		prplInfo = GAIM_PLUGIN_PROTOCOL_INFO(prpl);

	/* Get User Info */
	if (prplInfo != NULL && prplInfo->get_info != NULL)
	{
		a = new QQuailAction(tr("Get Information"),
							QIconSet(Resource::loadPixmap("gaim/16x16/info"),
									 Resource::loadPixmap("gaim/32x32/info")),
							QString::null, 0, this, 0, false, buddy);
		a->addTo(menu);

		connect(a, SIGNAL(activated(void *)),
				this, SLOT(userInfoSlot(void *)));
	}

	/* IM */
	a = new QQuailAction(tr("IM"),
						QIconSet(Resource::loadPixmap("gaim/16x16/send-im"),
								 Resource::loadPixmap("gaim/32x32/send-im")),
						QString::null, 0, this, 0, false, buddy);
	a->addTo(menu);

	connect(a, SIGNAL(activated(void *)),
			this, SLOT(sendImSlot(void *)));

	/* Put in the protocol-specific actions */
	if (prplInfo != NULL && prplInfo->buddy_menu != NULL)
	{
		GList *l;
		PurpleConnection *gc = gaim_account_get_connection(buddy->account);

		for (l = prplInfo->buddy_menu(gc, buddy->name);
			 l != NULL;
			 l = l->next)
		{
			struct proto_buddy_menu *pbm;

			pbm = (struct proto_buddy_menu *)l->data;

			a = new QQuailAction(pbm->label, QString::null, 0, this, 0,
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
	a = new QQuailAction(tr("Alias"),
						QIconSet(Resource::loadPixmap("gaim/16x16/alias"),
								 Resource::loadPixmap("gaim/32x32/alias")),
						QString::null, 0, this, 0, false, buddy);
	a->addTo(menu);

	connect(a, SIGNAL(activated(void *)),
			this, SLOT(aliasBuddySlot(void *)));

	/* Remove */
	a = new QQuailAction(tr("Remove"),
						QIconSet(Resource::loadPixmap("gaim/16x16/remove"),
								 Resource::loadPixmap("gaim/32x32/remove")),
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
		a = new QQuailAction(tr("Expand"),
							QIconSet(Resource::loadPixmap("gaim/16x16/expand"),
									 Resource::loadPixmap("gaim/32x32/expand")),
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
						QQuailBuddyList::getBuddyStatusIcon(bnode),
						buddy2->name, submenu);
				}
			}
		}
	}
}

void
QQuailBuddyList::populateContactMenu(PurpleContact *contact, QPopupMenu *menu)
{
	PurplePlugin *prpl = NULL;
	PurplePluginProtocolInfo *prplInfo = NULL;
	QAction *a;

	prpl = gaim_plugins_find_with_id(gaim_account_get_protocol_id(
			gaim_contact_get_priority_buddy(contact)->account));

	if (prpl != NULL)
		prplInfo = GAIM_PLUGIN_PROTOCOL_INFO(prpl);

	/* Collapse */
	a = new QAction(tr("Collapse"),
					QIconSet(Resource::loadPixmap("gaim/16x16/collapse"),
							 Resource::loadPixmap("gaim/32x32/collapse")),
					QString::null, 0, this, 0);
	a->addTo(menu);

	connect(a, SIGNAL(activated()),
			this, SLOT(collapseContactSlot()));

	/* Remove */
	a = new QAction(tr("Remove"),
					QIconSet(Resource::loadPixmap("gaim/16x16/remove"),
							 Resource::loadPixmap("gaim/32x32/remove")),
					QString::null, 0, this, 0);
	a->addTo(menu);

	connect(a, SIGNAL(activated()),
			this, SLOT(removeContactSlot()));
}

void
QQuailBuddyList::populateChatMenu(PurpleChat *chat, QPopupMenu *menu)
{
	PurplePlugin *prpl = NULL;
	PurplePluginProtocolInfo *prplInfo = NULL;
	QAction *a;

	prpl = gaim_plugins_find_with_id(
		gaim_account_get_protocol_id(chat->account));

	if (prpl != NULL)
		prplInfo = GAIM_PLUGIN_PROTOCOL_INFO(prpl);

	/* Join Chat */
	a = new QAction(tr("Join Chat"),
					QIconSet(Resource::loadPixmap("gaim/16x16/join_chat"),
							 Resource::loadPixmap("gaim/32x32/join_chat")),
					QString::null, 0, this, 0);
	a->addTo(menu);

	connect(a, SIGNAL(activated()),
			this, SLOT(joinChatSlot()));

	/* Auto-Join */
	a = new QAction(tr("Auto-Join"), QString::null, 0, this, 0, true);

	if (gaim_blist_node_get_bool((GaimBlistNode *)chat, "qpe-autojoin"))
		a->setOn(true);

	a->addTo(menu);

	connect(a, SIGNAL(toggled(bool)),
			this, SLOT(autoJoinChatSlot(bool)));

	/* Alias */
	a = new QAction(tr("Alias"),
					QIconSet(Resource::loadPixmap("gaim/16x16/alias"),
							 Resource::loadPixmap("gaim/32x32/alias")),
					QString::null, 0, this, 0);
	a->addTo(menu);

	connect(a, SIGNAL(activated()),
			this, SLOT(aliasChatSlot()));

	/* Remove */
	a = new QAction(tr("Remove Chat"),
					QIconSet(Resource::loadPixmap("gaim/16x16/remove"),
							 Resource::loadPixmap("gaim/32x32/remove")),
					QString::null, 0, this, 0);
	a->addTo(menu);

	connect(a, SIGNAL(activated()),
			this, SLOT(removeChatSlot()));
}

void
QQuailBuddyList::populateGroupMenu(PurpleGroup *, QPopupMenu *menu)
{
	QAction *a;

	/* Add a Buddy */
	a = new QAction(tr("Add a Buddy"),
					QIconSet(Resource::loadPixmap("gaim/16x16/add"),
							 Resource::loadPixmap("gaim/32x32/add")),
					QString::null, 0, this, 0);
	a->addTo(menu);

	connect(a, SIGNAL(activated()),
			this, SLOT(addBuddySlot()));

	/* Add a Chat */
	a = new QAction(tr("Add a Chat"),
					QIconSet(Resource::loadPixmap("gaim/16x16/new-chat"),
							 Resource::loadPixmap("gaim/32x32/new-chat")),
					QString::null, 0, this, 0);
	a->addTo(menu);

	connect(a, SIGNAL(activated()),
			this, SLOT(addChatSlot()));

	/* Delete Group */
	a = new QAction(tr("Remove Group"),
					QIconSet(Resource::loadPixmap("gaim/16x16/remove"),
							 Resource::loadPixmap("gaim/32x32/remove")),
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
QQuailBuddyList::resizeEvent(QResizeEvent *)
{
	if (gaim_prefs_get_bool("/gaim/qpe/blist/show_large_icons"))
		setColumnWidth(1, 0);
	else
		setColumnWidth(1, width() / 4);

	setColumnWidth(0, width() - 20 - columnWidth(1) - columnWidth(2));
}

void
QQuailBuddyList::nodeExpandedSlot(QListViewItem *_item)
{
	QQuailBListItem *item = (QQuailBListItem *)_item;
	GaimBlistNode *node;

	node = item->getBlistNode();

	if (GAIM_BLIST_NODE_IS_GROUP(node))
	{
		gaim_blist_node_set_bool(node, "collapsed", FALSE);

		if (!saveTimer->isActive())
			saveTimer->start(2000, true);
	}
	else if (GAIM_BLIST_NODE_IS_CONTACT(node))
	{

	}
}

void
QQuailBuddyList::nodeCollapsedSlot(QListViewItem *_item)
{
	QQuailBListItem *item = (QQuailBListItem *)_item;
	GaimBlistNode *node;

	node = item->getBlistNode();

	if (GAIM_BLIST_NODE_IS_GROUP(node))
	{
		gaim_blist_node_set_bool(node, "collapsed", TRUE);

		if (!saveTimer->isActive())
			saveTimer->start(2000, true);
	}
	else if (GAIM_BLIST_NODE_IS_CONTACT(node))
	{
		collapseContactSlot(item);
	}
}

void
QQuailBuddyList::collapseContactSlot()
{
	collapseContactSlot((QQuailBListItem *)selectedItem());
}

void
QQuailBuddyList::collapseContactSlot(QQuailBListItem *item)
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
QQuailBuddyList::expandContactSlot()
{
	expandContactSlot((QQuailBListItem *)selectedItem());
}

void
QQuailBuddyList::expandContactSlot(QQuailBListItem *item)
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
QQuailBuddyList::saveBlistSlot()
{
	gaim_blist_save();
}

void
QQuailBuddyList::showContextMenuSlot(QListViewItem *_item,
									const QPoint &point, int)
{
	QQuailBListItem *item = (QQuailBListItem *)_item;
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
		PurpleContact *contact = (PurpleContact *)node;

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
		populateChatMenu((PurpleChat *)node, menu);
	}
	else if (GAIM_BLIST_NODE_IS_GROUP(node))
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
QQuailBuddyList::addBuddySlot()
{
	GaimBlistNode *node;
	QQuailBListItem *item;

	if ((item = (QQuailBListItem *)selectedItem()) == NULL)
		return;

	node = item->getBlistNode();

	if (!GAIM_BLIST_NODE_IS_GROUP(node))
		return;

	emit addBuddy((PurpleGroup *)node);
}

void
QQuailBuddyList::addChatSlot()
{
	GaimBlistNode *node;
	QQuailBListItem *item;

	if ((item = (QQuailBListItem *)selectedItem()) == NULL)
		return;

	node = item->getBlistNode();

	if (!GAIM_BLIST_NODE_IS_GROUP(node))
		return;

	emit addChat((PurpleGroup *)node);
}

void
QQuailBuddyList::removeGroupSlot()
{
	GaimBlistNode *node;
	QQuailBListItem *item;

	if ((item = (QQuailBListItem *)selectedItem()) == NULL)
		return;

	node = item->getBlistNode();

	if (!GAIM_BLIST_NODE_IS_GROUP(node))
		return;

	emit removeGroup((PurpleGroup *)node);
}

static void
renameGroupCb(PurpleGroup *group, const char *newName)
{
	gaim_blist_rename_group(group, newName);
	gaim_blist_save();
}

void
QQuailBuddyList::renameGroupSlot()
{
	GaimBlistNode *node;
	QQuailBListItem *item;
	PurpleGroup *group;

	if ((item = (QQuailBListItem *)selectedItem()) == NULL)
		return;

	node = item->getBlistNode();

	if (!GAIM_BLIST_NODE_IS_GROUP(node))
		return;

	group = (PurpleGroup *)node;

	gaim_request_input(NULL, tr("Rename Group"),
					   tr("Rename Group"),
					   tr("Please enter a new name for the selected group."),
					   group->name, FALSE, FALSE,
					   tr("Rename"), G_CALLBACK(renameGroupCb),
					   tr("Cancel"), NULL, group);
}

void
QQuailBuddyList::userInfoSlot(void *data)
{
	GaimBuddy *buddy = (GaimBuddy *)data;

	serv_get_info(gaim_account_get_connection(buddy->account), buddy->name);
}

void
QQuailBuddyList::sendImSlot(void *data)
{
	emit openIm((GaimBuddy *)data);
}

void
QQuailBuddyList::protoActionSlot(void *data1, void *data2)
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
QQuailBuddyList::aliasBuddySlot(void *data)
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
QQuailBuddyList::removeBuddySlot(void *data)
{
	emit removeBuddy((GaimBuddy *)data);
}

void
QQuailBuddyList::removeContactSlot()
{
	GaimBlistNode *node;
	QQuailBListItem *item;

	if ((item = (QQuailBListItem *)selectedItem()) == NULL)
		return;

	node = item->getBlistNode();

	if (!GAIM_BLIST_NODE_IS_CONTACT(node))
		return;

	emit removeContact((PurpleContact *)node);
}

void
QQuailBuddyList::joinChatSlot()
{
	GaimBlistNode *node;
	QQuailBListItem *item;

	if ((item = (QQuailBListItem *)selectedItem()) == NULL)
		return;

	node = item->getBlistNode();

	if (!GAIM_BLIST_NODE_IS_CHAT(node))
		return;

	emit joinChat((PurpleChat *)node);
}

void
QQuailBuddyList::autoJoinChatSlot(bool on)
{
	GaimBlistNode *node;
	QQuailBListItem *item;

	if ((item = (QQuailBListItem *)selectedItem()) == NULL)
		return;

	node = item->getBlistNode();

	if (!GAIM_BLIST_NODE_IS_CHAT(node))
		return;

	gaim_blist_node_set_bool(node, "qpe-autojoin", on);
	gaim_blist_save();
}

void
QQuailBuddyList::removeChatSlot()
{
	GaimBlistNode *node;
	QQuailBListItem *item;

	if ((item = (QQuailBListItem *)selectedItem()) == NULL)
		return;

	node = item->getBlistNode();

	if (!GAIM_BLIST_NODE_IS_CHAT(node))
		return;

	emit removeChat((PurpleChat *)node);
}

static void
aliasChatCb(PurpleChat *chat, const char *newAlias)
{
	gaim_blist_alias_chat(chat, newAlias);
	gaim_blist_save();
}

void
QQuailBuddyList::aliasChatSlot()
{
	GaimBlistNode *node;
	QQuailBListItem *item;
	PurpleChat *chat;

	if ((item = (QQuailBListItem *)selectedItem()) == NULL)
		return;

	node = item->getBlistNode();

	if (!GAIM_BLIST_NODE_IS_CHAT(node))
		return;

	chat = (PurpleChat *)node;

	gaim_request_input(NULL, tr("Alias Chat"),
					   tr("Please enter an aliased name for this chat."),
					   NULL,
					   chat->alias, false, false,
					   tr("Alias"), G_CALLBACK(aliasChatCb),
					   tr("Cancel"), NULL, chat);
}

void
QQuailBuddyList::addGroup(GaimBlistNode *node)
{
	QQuailBListItem *item = new QQuailBListItem(this, node);
	node->ui_data = item;

	item->setExpandable(true);
}

void
QQuailBuddyList::updateGroup(GaimBlistNode *node)
{
	PurpleGroup *group;
	QQuailBListItem *item;

	g_return_if_fail(GAIM_BLIST_NODE_IS_GROUP(node));

	item = (QQuailBListItem *)node->ui_data;
	group = (PurpleGroup *)node;

	if (gaim_prefs_get_bool("/gaim/qpe/blist/show_empty_groups")    ||
		gaim_prefs_get_bool("/gaim/qpe/blist/show_offline_buddies") ||
		gaim_blist_get_group_online_count(group) > 0)
	{
		if (item == NULL)
		{
			addGroup(node);
			item = (QQuailBListItem *)node->ui_data;
		}

		if (!gaim_blist_node_get_bool(node, "collapsed"))
			item->setOpen(true);
	}
	else
	{
		if (item != NULL)
			delete item;
	}
}

void
QQuailBuddyList::updateContact(GaimBlistNode *node)
{
	PurpleContact *contact;
	GaimBuddy *buddy;
	QQuailBListItem *item;

	g_return_if_fail(GAIM_BLIST_NODE_IS_CONTACT(node));

	updateGroup(node->parent);

	item    = (QQuailBListItem *)node->ui_data;
	contact = (PurpleContact *)node;
	buddy   = gaim_contact_get_priority_buddy(contact);

	if (buddy != NULL &&
		(buddy->present != GAIM_BUDDY_OFFLINE ||
		 (gaim_account_is_connected(buddy->account) &&
		  gaim_prefs_get_bool("/gaim/qpe/blist/show_offline_buddies"))))
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
QQuailBuddyList::updateBuddy(GaimBlistNode *node)
{
	PurpleContact *contact;
	GaimBuddy *buddy;
	QQuailBListItem *item;

	g_return_if_fail(GAIM_BLIST_NODE_IS_BUDDY(node));

	updateContact(node->parent);

	if (node->parent->ui_data == NULL ||
		(node->parent->ui_data != NULL &&
		 !((QQuailBListItem *)node->parent->ui_data)->isExpanded()))
	{
		return;
	}

	buddy   = (GaimBuddy *)node;
	contact = (PurpleContact *)node->parent;
	item    = (QQuailBListItem *)node->ui_data;

	if (buddy->present != GAIM_BUDDY_OFFLINE ||
		(gaim_account_is_connected(buddy->account) &&
		 gaim_prefs_get_bool("/gaim/qpe/blist/show_offline_buddies")))
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
QQuailBuddyList::updateChat(GaimBlistNode *node)
{
	PurpleChat *chat;
	QQuailBListItem *item;

	g_return_if_fail(GAIM_BLIST_NODE_IS_CHAT(node));

	updateGroup(node->parent);

	chat = (PurpleChat *)node;
	item = (QQuailBListItem *)node->ui_data;

	if (gaim_account_is_connected(chat->account))
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
