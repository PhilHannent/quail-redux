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
//#include <libpurple/multi.h>
#include <libpurple/prefs.h>
#include <libpurple/request.h>
#include <libpurple/server.h>

//#include <qpe/qpeapplication.h>
//#include <qpe/resource.h>

#include <QAction>
//#include <qheader.h>
#include <QMenu>
#include <QTimer>


/**************************************************************************
 * QQuailBListItem
 **************************************************************************/
QQuailBListItem::QQuailBListItem(QListWidget *parent, PurpleBlistNode *node)
    : QListWidgetItem(parent), node(node), expanded(false), dirty(true)
{
	init();
}

//QQuailBListItem::QQuailBListItem(QListWidgetItem *parent, PurpleBlistNode *node)
//    : QListWidgetItem(parent), node(node), expanded(false), dirty(true)
//{
//	init();
//}

QQuailBListItem::~QQuailBListItem()
{
	if (node != NULL)
		node->ui_data = NULL;
}

PurpleBlistNode *
QQuailBListItem::getBlistNode() const
{
	return node;
}

void
QQuailBListItem::updateInfo()
{
//	QQuailPixmapSize pixmapSize;

//    if (purple_prefs_get_bool("/quail/blist/show_large_icons"))
//		pixmapSize = QGAIM_PIXMAP_LARGE;
//	else
//		pixmapSize = QGAIM_PIXMAP_SMALL;

	dirty = true;

    if (PURPLE_BLIST_NODE_IS_CONTACT(node))
	{
		PurpleContact *contact = (PurpleContact *)node;
		PurpleBuddy *buddy = purple_contact_get_priority_buddy(contact);
        PurplePresence *presence = purple_buddy_get_presence(buddy);
        char *tmp;

		if (buddy == NULL)
			return;

		if (isExpanded())
		{
            setIcon(QPixmap(":/quail.png"));
		}
		else
		{
            QString text("");

//			if (buddy->evil > 0)
//				text += QString("%1%").arg(buddy->evil);

            if (purple_prefs_get_bool("/quail/blist/show_idle_times"))
			{
                int idle_secs = 0;
                /* Idle */
                if (purple_presence_is_idle(presence))
                {
                    idle_secs = purple_presence_get_idle_time(presence);
                    if (idle_secs > 0)
                    {
                        tmp = purple_str_seconds_to_string(time(NULL) - idle_secs);
                        //purple_notify_user_info_add_pair(user_info, tr("Idle"), tmp);

                        g_free(tmp);
                        time_t t;
                        int ihrs, imin;
                        QString idle;

                        time(&t);

                        ihrs = (t - idle_secs) / 3600;
                        imin = ((t - idle_secs) / 60) % 60;

                        if (ihrs > 0)
                            idle = QString("(%1:%2)").arg(ihrs).arg(imin, 2);
                        else
                            idle = QString("(%1)").arg(imin);

                        if (text != "")
                            text += "  ";

                        text += idle;

                    }
                }

			}

            setPixmap(QQuailBuddyList::getBuddyStatusIcon((PurpleBlistNode *)buddy));
            //setText(1, text);
		}

        //setText(0, purple_get_buddy_alias(buddy));
	}
    else if (PURPLE_BLIST_NODE_IS_BUDDY(node))
	{
		PurpleBuddy *buddy = (PurpleBuddy *)node;
        PurplePresence *presence = purple_buddy_get_presence(buddy);
		QString text = "";

        //if (buddy->evil > 0)
            //text += QString("%1%").arg(buddy->evil);

        if (purple_prefs_get_bool("/quail/blist/show_idle_times"))
		{
            int idle_secs = 0;
            /* Idle */
            if (purple_presence_is_idle(presence))
            {
                idle_secs = purple_presence_get_idle_time(presence);
                if (idle_secs > 0)
                {
                    //tmp = purple_str_seconds_to_string(time(NULL) - idle_secs);
                    //purple_notify_user_info_add_pair(user_info, tr("Idle"), tmp);

                    //g_free(tmp);
                    time_t t;
                    int ihrs, imin;
                    QString idle;

                    time(&t);

                    ihrs = (t - idle_secs) / 3600;
                    imin = ((t - idle_secs) / 60) % 60;

                    if (ihrs > 0)
                        idle = QString("(%1:%2)").arg(ihrs).arg(imin, 2);
                    else
                        idle = QString("(%1)").arg(imin);

                    if (text != "")
                        text += "  ";

                    text += idle;

                }
            }
        }

        setIcon(QQuailBuddyList::getBuddyStatusIcon(node));
        //setText(0, purple_get_buddy_alias(buddy));
        //setText(1, text);
	}
    else if (PURPLE_BLIST_NODE_IS_CHAT(node))
	{
		PurpleChat *chat = (PurpleChat *)node;

        setIcon(QQuailProtocolUtils::getProtocolIcon(chat->account));
        //setText(0, purple_chat_get_display_name(chat));
	}
}

void
QQuailBListItem::paintCell(QPainter *p, const QPalette &cg, int column,
						  int width, int align)
{
//    QListWidget *lv = this->listWidget();
//	const QPixmap *icon = pixmap(column);

//	p->fillRect(0, 0, width, height(), cg.base());

//	int itMarg = (lv == NULL ? 1 : lv->itemMargin());
//	int lmarg = itMarg;

//	if (isSelected() && (column == 0 || lv->allColumnsShowFocus()))
//	{
//		p->fillRect(0, 0, width, height(), cg.highlight());
//		p->setPen(cg.highlightedText());
//	}
//	else
//		p->setPen(cg.text());

//	if (icon)
//	{
//		p->drawPixmap(lmarg, (height() - icon->height()) / 2, *icon);
//		lmarg += icon->width() + itMarg;
//	}

//    if (PURPLE_BLIST_NODE_IS_BUDDY(node) || PURPLE_BLIST_NODE_IS_CONTACT(node))
//		paintBuddyInfo(p, cg, column, width, align, lmarg, itMarg);
//    else if (PURPLE_BLIST_NODE_IS_GROUP(node))
//		paintGroupInfo(p, cg, column, width, align, lmarg, itMarg);
//	else
//        QListWidgetItem::paintCell(p, cg, column, width, align);

	dirty = false;
}

void
QQuailBListItem::paintBranches(QPainter *p, const QPalette &cg,
                              int width, int x, int height)
{
//    if (PURPLE_BLIST_NODE_IS_CHAT(node) || PURPLE_BLIST_NODE_IS_BUDDY(node))
//	{
//		p->fillRect(0, 0, width, height, QBrush(cg.base()));
//	}
//	else
//        return QListWidgetItem::paintBranches(p, cg, width, x, height);
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
QQuailBListItem::paintBuddyInfo(QPainter *p, const QPalette &cg, int column,
							   int width, int align, int lmarg, int itMarg)
{
	PurpleBuddy *buddy;
	PurpleContact *contact = NULL;

    if (PURPLE_BLIST_NODE_IS_BUDDY(node))
		buddy = (PurpleBuddy *)node;
	else
	{
		contact = (PurpleContact *)node;
		buddy = purple_contact_get_priority_buddy(contact);
	}

    if (purple_prefs_get_bool("/quail/blist/show_large_icons"))
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

				prpl = purple_plugins_find_with_id(
					purple_account_get_protocol_id(buddy->account));

				if (prpl != NULL)
                    prplInfo = PURPLE_PLUGIN_PROTOCOL_INFO(prpl);

				if (prpl != NULL && prplInfo->status_text != NULL &&
					purple_account_get_connection(buddy->account) != NULL)
				{
					char *tmp = prplInfo->status_text(buddy);

					if (tmp != NULL)
					{
						g_strdelimit(tmp, "\n", ' ');

						statusText = QString(tmp) + " ";

						g_free(tmp);
					}
				}

                if (!isExpanded() &&
                    purple_prefs_get_bool("/quail/blist/show_idle_times"))
				{
					time_t t;
					int ihrs, imin;

					time(&t);
					ihrs = (t - buddy->idle) / 3600;
					imin = ((t - buddy->idle) / 60) % 60;

					if (ihrs)
					{
                        idleTime = tr("Idle (%1h%2m) ").args( ihrs, imin);
					}
					else
                        idleTime = tr("Idle (%1m) ").arg(imin);
				}

//				if (!isExpanded() && buddy->evil > 0 &&
//                    purple_prefs_get_bool("/quail/blist/show_warning_levels"))
//				{
//                    warning = tr("Warned (%1%) ").arg(buddy->evil);
//				}

                if (!PURPLE_BUDDY_IS_ONLINE(buddy) && statusText.isEmpty())
					statusText = QObject::tr("Offline ");

				if (contact != NULL && !isExpanded() && contact->alias != NULL)
					topText = contact->alias;
				else
                    topText = purple_contact_get_alias(contact);

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
                purple_prefs_get_bool("/quail/blist/dim_idle_buddies"))
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
            QPalette _cg(cg);

			if (buddy->idle > 0 &&
                purple_prefs_get_bool("/quail/blist/dim_idle_buddies"))
			{
                _cg.setColor(QPalette::Text, cg.dark());
			}

            QListWidgetItem::paintCell(p, _cg, column, width, align);
		}
	}
	else
	{
        QPalette _cg(cg);

		if (buddy->idle > 0 &&
            purple_prefs_get_bool("/quail/blist/dim_idle_buddies"))
		{
            _cg.setColor(QPalette::Text, cg.dark());
		}

        QListWidgetItem::paintCell(p, _cg, column, width, align);
	}
}

void
QQuailBListItem::paintGroupInfo(QPainter *p, const QPalette &, int column,
							   int width, int align, int lmarg, int itMarg)
{
	if (column > 0)
		return;

	PurpleGroup *group = (PurpleGroup *)getBlistNode();
	QString groupName, detail;
	QFont f = p->font();

	groupName = group->name;

    if (purple_prefs_get_bool("/quail/blist/show_group_count"))
	{
		groupName += " ";
		detail = QString("(%1/%2)").arg(
			purple_blist_get_group_online_count(group)).arg(
			purple_blist_get_group_size(group, FALSE));
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
QQuailBuddyList::getBuddyStatusIcon(PurpleBlistNode *node)
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

    if (PURPLE_BLIST_NODE_IS_BUDDY(node))
		account = ((PurpleBuddy *)node)->account;
    else if (PURPLE_BLIST_NODE_IS_CHAT(node))
		account = ((PurpleChat *)node)->account;
	else
		return QPixmap();

	prpl = purple_plugins_find_with_id(purple_account_get_protocol_id(account));

	if (prpl == NULL)
		return QPixmap();

    prplInfo = PURPLE_PLUGIN_PROTOCOL_INFO(prpl);

	if (prplInfo->list_icon != NULL)
	{
        if (PURPLE_BLIST_NODE_IS_BUDDY(node))
			protoName = prplInfo->list_icon(account, (PurpleBuddy *)node);
        else if (PURPLE_BLIST_NODE_IS_CHAT(node))
			protoName = prplInfo->list_icon(account, NULL);
	}

    if (PURPLE_BLIST_NODE_IS_BUDDY(node) &&
		((PurpleBuddy *)node)->present != GAIM_BUDDY_SIGNING_OFF &&
		prplInfo->list_emblems != NULL)
	{
		prplInfo->list_emblems((PurpleBuddy *)node, &se, &sw, &nw, &ne);
	}

	if (se == NULL)
	{
		if      (sw != NULL) se = sw;
		else if (ne != NULL) se = ne;
		else if (nw != NULL) se = nw;
	}

	sw = nw = ne = NULL; /* So that only the se icon will composite. */

    if (PURPLE_BLIST_NODE_IS_BUDDY(node) &&
		((PurpleBuddy *)node)->present == GAIM_BUDDY_SIGNING_ON)
	{
        statusImage = QPixmap(":/data/images/status/login.png");
	}
    else if (PURPLE_BLIST_NODE_IS_BUDDY(node) &&
			 ((PurpleBuddy *)node)->present == GAIM_BUDDY_SIGNING_OFF)
	{
        statusImage = QPixmap(":/data/images/status/logout.png");
	}
	else
	{
        statusImage = QPixmap(":/data/images/protocols/" +
                              QString(protoName) + ".png");
	}

	if (statusImage.isNull())
		return QPixmap();

	if (se != NULL)
	{
        emblemImage = QPixmap(":/data/images/status/" + QString(se) + ".png");

		if (!emblemImage.isNull())
		{
			QQuailImageUtils::blendOnLower(
					statusImage.width()  - emblemImage.width(),
					statusImage.height() - emblemImage.height(),
					emblemImage, statusImage);
		}
	}

	/* Grey idle buddies. */
    if (PURPLE_BLIST_NODE_IS_BUDDY(node) &&
		((PurpleBuddy *)node)->present == GAIM_BUDDY_OFFLINE)
	{
		QQuailImageUtils::greyImage(statusImage);
	}
    else if (PURPLE_BLIST_NODE_IS_BUDDY(node) &&
			 ((PurpleBuddy *)node)->idle)
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
    : QListWidget(parent, name)
{
	addColumn(tr("Buddy"), -1);
	addColumn(tr("Idle"), 50);

	setColumnAlignment(1, AlignRight);

	setAllColumnsShowFocus(TRUE);
	setRootIsDecorated(true);

	header()->hide();

    connect(this, SIGNAL(expanded(QListWidgetItem *)),
            this, SLOT(nodeExpandedSlot(QListWidgetItem *)));
    connect(this, SIGNAL(collapsed(QListWidgetItem *)),
            this, SLOT(nodeCollapsedSlot(QListWidgetItem *)));
    connect(this, SIGNAL(rightButtonPressed(QListWidgetItem *,
											const QPoint &, int)),
            this, SLOT(showContextMenuSlot(QListWidgetItem *,
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
QQuailBuddyList::setBlist(PurpleBuddyList *list)
{
    quailBlist = list;
}

PurpleBuddyList *
QQuailBuddyList::getBlist() const
{
    return quailBlist;
}

void
QQuailBuddyList::updateNode(PurpleBlistNode *node)
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
QQuailBuddyList::reload(bool remove)
{
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
QQuailBuddyList::getSelectedBuddy() const
{
	QQuailBListItem *item;
	PurpleBlistNode *node;
	PurpleBuddy *buddy = NULL;

	if ((item = (QQuailBListItem *)selectedItem()) == NULL)
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
QQuailBuddyList::populateBuddyMenu(PurpleBuddy *buddy, QMenu *menu,
								  bool asContact)
{
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
        a = new QQuailAction(QIcon(QPixmap(":/data/images/actions/info.png")),
                             tr("Get Information"),
                             this);
        a->setData(QVariant(buddy));
        menu->addAction(a);

		connect(a, SIGNAL(activated(void *)),
				this, SLOT(userInfoSlot(void *)));
	}

	/* IM */
    a = new QQuailAction(QIcon(QPixmap(":/data/images/actions/send-im.png")),
                         tr("IM"),
                         this);
    a->setData(buddy);
    menu->addAction(a);

	connect(a, SIGNAL(activated(void *)),
			this, SLOT(sendImSlot(void *)));

	/* Put in the protocol-specific actions */
	if (prplInfo != NULL && prplInfo->buddy_menu != NULL)
	{
		GList *l;
		PurpleConnection *gc = purple_account_get_connection(buddy->account);

		for (l = prplInfo->buddy_menu(gc, buddy->name);
			 l != NULL;
			 l = l->next)
		{
			struct proto_buddy_menu *pbm;

			pbm = (struct proto_buddy_menu *)l->data;

            a = new QQuailAction(pbm->label, QString::null, 0, this, 0,
                                false, buddy);
            a->setUserData2(pbm);
            menu->addAction(a);

			connect(a, SIGNAL(activated(void *, void *)),
					this, SLOT(protoActionSlot(void *, void *)));
		}
	}

	/* Separator */
    menu->addSeparator();

	/* Alias */
	a = new QQuailAction(tr("Alias"),
                        QIcon(QPixmap(":/data/images/actions/alias.png")),
						QString::null, 0, this, 0, false, buddy);
    menu->addAction(a);

	connect(a, SIGNAL(activated(void *)),
			this, SLOT(aliasBuddySlot(void *)));

	/* Remove */
	a = new QQuailAction(tr("Remove"),
                        QIcon(QPixmap(":/data/images/actions/remove.png")),
						QString::null, 0, this, 0, false, buddy);
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
							QString::null, 0, this, 0);
        menu->addAction(a);

		connect(a, SIGNAL(activated()),
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

				if (!showOffline && GAIM_BUDDY_IS_ONLINE(buddy2))
				{
                    QMenu *subMenu = new QMenu(this);

                    populateBuddyMenu(buddy2, subMenu, false);
                    subMenu->setIcon(QQuailBuddyList::getBuddyStatusIcon(bnode));
                    subMenu->setTitle(buddy2->name);
                    menu->addMenu(subMenu);
				}
			}
		}
	}
}

void
QQuailBuddyList::populateContactMenu(PurpleContact *contact, QMenu *menu)
{
	PurplePlugin *prpl = NULL;
	PurplePluginProtocolInfo *prplInfo = NULL;
	QAction *a;

	prpl = purple_plugins_find_with_id(purple_account_get_protocol_id(
			purple_contact_get_priority_buddy(contact)->account));

	if (prpl != NULL)
        prplInfo = PURPLE_PLUGIN_PROTOCOL_INFO(prpl);

	/* Collapse */
	a = new QAction(tr("Collapse"),
                    QIcon(QPixmap(":/data/images/actions/collapse.png")),
					QString::null, 0, this, 0);
    menu->addAction(a);

	connect(a, SIGNAL(activated()),
			this, SLOT(collapseContactSlot()));

	/* Remove */
	a = new QAction(tr("Remove"),
                    QIcon(QPixmap(":/data/images/actions/remove.png")),
					QString::null, 0, this, 0);
    menu->addAction(a);

	connect(a, SIGNAL(activated()),
			this, SLOT(removeContactSlot()));
}

void
QQuailBuddyList::populateChatMenu(PurpleChat *chat, QMenu *menu)
{
	PurplePlugin *prpl = NULL;
	PurplePluginProtocolInfo *prplInfo = NULL;
	QAction *a;

	prpl = purple_plugins_find_with_id(
		purple_account_get_protocol_id(chat->account));

	if (prpl != NULL)
        prplInfo = PURPLE_PLUGIN_PROTOCOL_INFO(prpl);

	/* Join Chat */
	a = new QAction(tr("Join Chat"),
                    QIcon(QPixmap(":/data/images/actions/join_chat.png")),
					QString::null, 0, this, 0);
    menu->addAction(a);

	connect(a, SIGNAL(activated()),
			this, SLOT(joinChatSlot()));

	/* Auto-Join */
	a = new QAction(tr("Auto-Join"), QString::null, 0, this, 0, true);

	if (purple_blist_node_get_bool((PurpleBlistNode *)chat, "qpe-autojoin"))
		a->setOn(true);

    menu->addAction(a);

	connect(a, SIGNAL(toggled(bool)),
			this, SLOT(autoJoinChatSlot(bool)));

	/* Alias */
	a = new QAction(tr("Alias"),
                    QIcon(QPixmap(":/data/images/actions/alias.png")),
					QString::null, 0, this, 0);
    menu->addAction(a);

	connect(a, SIGNAL(activated()),
			this, SLOT(aliasChatSlot()));

	/* Remove */
	a = new QAction(tr("Remove Chat"),
                    QIcon(QPixmap(":/data/images/actions/remove.png")),
					QString::null, 0, this, 0);
    menu->addAction(a);

	connect(a, SIGNAL(activated()),
			this, SLOT(removeChatSlot()));
}

void
QQuailBuddyList::populateGroupMenu(PurpleGroup *, QMenu *menu)
{
	QAction *a;

	/* Add a Buddy */
	a = new QAction(tr("Add a Buddy"),
                    QIcon(QPixmap(":/data/images/actions/add.png")),
					QString::null, 0, this, 0);
    menu->addAction(a);

	connect(a, SIGNAL(activated()),
			this, SLOT(addBuddySlot()));

	/* Add a Chat */
	a = new QAction(tr("Add a Chat"),
                    QIcon(QPixmap(":/data/images/actions/new-chat.png")),
					QString::null, 0, this, 0);
    menu->addAction(a);

	connect(a, SIGNAL(activated()),
			this, SLOT(addChatSlot()));

	/* Delete Group */
	a = new QAction(tr("Remove Group"),
                    QIcon(QPixmap(":/data/images/actions/remove.png")),
					QString::null, 0, this, 0);
    menu->addAction(a);

	connect(a, SIGNAL(activated()),
			this, SLOT(removeGroupSlot()));

	/* Rename */
	a = new QAction(tr("Rename"), QString::null, 0, this, 0);
    menu->addAction(a);

	connect(a, SIGNAL(activated()),
			this, SLOT(renameGroupSlot()));
}

void
QQuailBuddyList::resizeEvent(QResizeEvent *)
{
    if (purple_prefs_get_bool("/quail/blist/show_large_icons"))
		setColumnWidth(1, 0);
	else
		setColumnWidth(1, width() / 4);

	setColumnWidth(0, width() - 20 - columnWidth(1) - columnWidth(2));
}

void
QQuailBuddyList::nodeExpandedSlot(QListWidgetItem *_item)
{
	QQuailBListItem *item = (QQuailBListItem *)_item;
	PurpleBlistNode *node;

	node = item->getBlistNode();

    if (PURPLE_BLIST_NODE_IS_GROUP(node))
	{
		purple_blist_node_set_bool(node, "collapsed", FALSE);

		if (!saveTimer->isActive())
			saveTimer->start(2000, true);
	}
    else if (PURPLE_BLIST_NODE_IS_CONTACT(node))
	{

	}
}

void
QQuailBuddyList::nodeCollapsedSlot(QListWidgetItem *_item)
{
	QQuailBListItem *item = (QQuailBListItem *)_item;
	PurpleBlistNode *node;

	node = item->getBlistNode();

    if (PURPLE_BLIST_NODE_IS_GROUP(node))
	{
		purple_blist_node_set_bool(node, "collapsed", TRUE);

		if (!saveTimer->isActive())
			saveTimer->start(2000, true);
	}
    else if (PURPLE_BLIST_NODE_IS_CONTACT(node))
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
	PurpleBlistNode *node;

	g_return_if_fail(item != NULL);

	node = item->getBlistNode();

    g_return_if_fail(PURPLE_BLIST_NODE_IS_CONTACT(node));

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
	PurpleBlistNode *node, *child;

	g_return_if_fail(item != NULL);

	node = item->getBlistNode();

    g_return_if_fail(PURPLE_BLIST_NODE_IS_CONTACT(node));

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
	purple_blist_save();
}

void
QQuailBuddyList::showContextMenuSlot(QListWidgetItem *_item,
									const QPoint &point, int)
{
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
QQuailBuddyList::addBuddySlot()
{
	PurpleBlistNode *node;
	QQuailBListItem *item;

	if ((item = (QQuailBListItem *)selectedItem()) == NULL)
		return;

	node = item->getBlistNode();

    if (!PURPLE_BLIST_NODE_IS_GROUP(node))
		return;

	emit addBuddy((PurpleGroup *)node);
}

void
QQuailBuddyList::addChatSlot()
{
	PurpleBlistNode *node;
	QQuailBListItem *item;

	if ((item = (QQuailBListItem *)selectedItem()) == NULL)
		return;

	node = item->getBlistNode();

    if (!PURPLE_BLIST_NODE_IS_GROUP(node))
		return;

	emit addChat((PurpleGroup *)node);
}

void
QQuailBuddyList::removeGroupSlot()
{
	PurpleBlistNode *node;
	QQuailBListItem *item;

	if ((item = (QQuailBListItem *)selectedItem()) == NULL)
		return;

	node = item->getBlistNode();

    if (!PURPLE_BLIST_NODE_IS_GROUP(node))
		return;

	emit removeGroup((PurpleGroup *)node);
}

static void
renameGroupCb(PurpleGroup *group, const char *newName)
{
	purple_blist_rename_group(group, newName);
	purple_blist_save();
}

void
QQuailBuddyList::renameGroupSlot()
{
	PurpleBlistNode *node;
	QQuailBListItem *item;
	PurpleGroup *group;

	if ((item = (QQuailBListItem *)selectedItem()) == NULL)
		return;

	node = item->getBlistNode();

    if (!PURPLE_BLIST_NODE_IS_GROUP(node))
		return;

	group = (PurpleGroup *)node;

	purple_request_input(NULL, tr("Rename Group"),
					   tr("Rename Group"),
					   tr("Please enter a new name for the selected group."),
					   group->name, FALSE, FALSE,
					   tr("Rename"), G_CALLBACK(renameGroupCb),
					   tr("Cancel"), NULL, group);
}

void
QQuailBuddyList::userInfoSlot(void *data)
{
	PurpleBuddy *buddy = (PurpleBuddy *)data;

	serv_get_info(purple_account_get_connection(buddy->account), buddy->name);
}

void
QQuailBuddyList::sendImSlot(void *data)
{
	emit openIm((PurpleBuddy *)data);
}

void
QQuailBuddyList::protoActionSlot(void *data1, void *data2)
{
	PurpleBuddy *buddy = (PurpleBuddy *)data1;
	struct proto_buddy_menu *pbm = (struct proto_buddy_menu *)data2;

	if (pbm->callback != NULL)
		pbm->callback(pbm->gc, buddy->name);
}

static void
aliasBuddyCb(PurpleBuddy *buddy, const char *newAlias)
{
	purple_blist_alias_buddy(buddy, newAlias);
	purple_blist_save();
}

void
QQuailBuddyList::aliasBuddySlot(void *data)
{
	PurpleBuddy *buddy = (PurpleBuddy *)data;

	purple_request_input(NULL, tr("Alias Buddy"),
			tr("Please enter an aliased name for %1.").arg(buddy->name),
			NULL,
			buddy->alias, false, false,
			tr("Alias"), G_CALLBACK(aliasBuddyCb),
			tr("Cancel"), NULL, buddy);
}

void
QQuailBuddyList::removeBuddySlot(void *data)
{
	emit removeBuddy((PurpleBuddy *)data);
}

void
QQuailBuddyList::removeContactSlot()
{
	PurpleBlistNode *node;
	QQuailBListItem *item;

	if ((item = (QQuailBListItem *)selectedItem()) == NULL)
		return;

	node = item->getBlistNode();

    if (!PURPLE_BLIST_NODE_IS_CONTACT(node))
		return;

	emit removeContact((PurpleContact *)node);
}

void
QQuailBuddyList::joinChatSlot()
{
	PurpleBlistNode *node;
	QQuailBListItem *item;

	if ((item = (QQuailBListItem *)selectedItem()) == NULL)
		return;

	node = item->getBlistNode();

    if (!PURPLE_BLIST_NODE_IS_CHAT(node))
		return;

	emit joinChat((PurpleChat *)node);
}

void
QQuailBuddyList::autoJoinChatSlot(bool on)
{
	PurpleBlistNode *node;
	QQuailBListItem *item;

	if ((item = (QQuailBListItem *)selectedItem()) == NULL)
		return;

	node = item->getBlistNode();

    if (!PURPLE_BLIST_NODE_IS_CHAT(node))
		return;

	purple_blist_node_set_bool(node, "qpe-autojoin", on);
	purple_blist_save();
}

void
QQuailBuddyList::removeChatSlot()
{
	PurpleBlistNode *node;
	QQuailBListItem *item;

	if ((item = (QQuailBListItem *)selectedItem()) == NULL)
		return;

	node = item->getBlistNode();

    if (!PURPLE_BLIST_NODE_IS_CHAT(node))
		return;

	emit removeChat((PurpleChat *)node);
}

static void
aliasChatCb(PurpleChat *chat, const char *newAlias)
{
	purple_blist_alias_chat(chat, newAlias);
	purple_blist_save();
}

void
QQuailBuddyList::aliasChatSlot()
{
	PurpleBlistNode *node;
	QQuailBListItem *item;
	PurpleChat *chat;

	if ((item = (QQuailBListItem *)selectedItem()) == NULL)
		return;

	node = item->getBlistNode();

    if (!PURPLE_BLIST_NODE_IS_CHAT(node))
		return;

	chat = (PurpleChat *)node;

	purple_request_input(NULL, tr("Alias Chat"),
					   tr("Please enter an aliased name for this chat."),
					   NULL,
					   chat->alias, false, false,
					   tr("Alias"), G_CALLBACK(aliasChatCb),
					   tr("Cancel"), NULL, chat);
}

void
QQuailBuddyList::addGroup(PurpleBlistNode *node)
{
	QQuailBListItem *item = new QQuailBListItem(this, node);
	node->ui_data = item;

	item->setExpandable(true);
}

void
QQuailBuddyList::updateGroup(PurpleBlistNode *node)
{
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
			item->setOpen(true);
	}
	else
	{
		if (item != NULL)
			delete item;
	}
}

void
QQuailBuddyList::updateContact(PurpleBlistNode *node)
{
	PurpleContact *contact;
	PurpleBuddy *buddy;
	QQuailBListItem *item;

    g_return_if_fail(PURPLE_BLIST_NODE_IS_CONTACT(node));

	updateGroup(node->parent);

	item    = (QQuailBListItem *)node->ui_data;
	contact = (PurpleContact *)node;
	buddy   = purple_contact_get_priority_buddy(contact);

	if (buddy != NULL &&
		(buddy->present != GAIM_BUDDY_OFFLINE ||
		 (purple_account_is_connected(buddy->account) &&
          purple_prefs_get_bool("/quail/blist/show_offline_buddies"))))
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
QQuailBuddyList::updateBuddy(PurpleBlistNode *node)
{
	PurpleContact *contact;
	PurpleBuddy *buddy;
	QQuailBListItem *item;

    g_return_if_fail(PURPLE_BLIST_NODE_IS_BUDDY(node));

	updateContact(node->parent);

	if (node->parent->ui_data == NULL ||
		(node->parent->ui_data != NULL &&
		 !((QQuailBListItem *)node->parent->ui_data)->isExpanded()))
	{
		return;
	}

	buddy   = (PurpleBuddy *)node;
	contact = (PurpleContact *)node->parent;
	item    = (QQuailBListItem *)node->ui_data;

	if (buddy->present != GAIM_BUDDY_OFFLINE ||
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
QQuailBuddyList::updateChat(PurpleBlistNode *node)
{
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
