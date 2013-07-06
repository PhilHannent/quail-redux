/**
 * @file QQuailBuddyList.h Buddy list widget
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
#ifndef _QGAIM_BUDDY_LIST_H_
#define _QGAIM_BUDDY_LIST_H_

#include <libpurple/account.h>
#include <libpurple/blist.h>

#include <qlistview.h>

#include "QuailImageUtils.h"

class QQuailBListItem : public QListViewItem
{
	public:
		QQuailBListItem(QListView *parent, GaimBlistNode *node);
		QQuailBListItem(QListViewItem *parent, GaimBlistNode *node);
		~QQuailBListItem();

		GaimBlistNode *getBlistNode() const;

		void updateInfo();

		void paintCell(QPainter *p, const QColorGroup &cg, int column,
					   int width, int align);

		void paintBranches(QPainter *p, const QColorGroup &cg,
						   int width, int y, int height, GUIStyle s);

		void setExpanded(bool expanded);
		bool isExpanded() const;

	protected:
		void init();

		void paintBuddyInfo(QPainter *p, const QColorGroup &cg, int column,
							int width, int align, int lmarg, int itMarg);
		void paintGroupInfo(QPainter *p, const QColorGroup &cg, int column,
							int width, int align, int lmarg, int itMarg);

	private:
		GaimBlistNode *node;
		bool expanded;
		bool dirty;
		QString topText, bottomText;
		int textY1, textY2;
};

class QQuailBuddyList : public QListView
{
	Q_OBJECT

	public:
		static QPixmap getBuddyStatusIcon(GaimBlistNode *node,
			QQuailPixmapSize size = QGAIM_PIXMAP_SMALL);

	public:
		QQuailBuddyList(QWidget *parent = NULL, const char *name = NULL);
		virtual ~QQuailBuddyList();

		void setGaimBlist(GaimBuddyList *list);
		GaimBuddyList *getGaimBlist() const;

		void updateNode(GaimBlistNode *node);

		void reload(bool remove = false);

		GaimBuddy *getSelectedBuddy() const;

	signals:
		void openIm(GaimBuddy *buddy);
		void addBuddy(PurpleGroup *group);
		void addChat(PurpleGroup *group);
		void joinChat(GaimChat *chat);
		void removeBuddy(GaimBuddy *buddy);
		void removeContact(GaimContact *contact);
		void removeChat(GaimChat *chat);
		void removeGroup(PurpleGroup *group);

	protected:
		void populateBuddyMenu(GaimBuddy *buddy, QPopupMenu *menu,
							   bool asContact);
		void populateContactMenu(GaimContact *contact, QPopupMenu *menu);
		void populateChatMenu(GaimChat *chat, QPopupMenu *menu);
		void populateGroupMenu(PurpleGroup *group, QPopupMenu *menu);

	protected slots:
		void resizeEvent(QResizeEvent *event);

		void nodeExpandedSlot(QListViewItem *item);
		void nodeCollapsedSlot(QListViewItem *item);
		void collapseContactSlot();
		void collapseContactSlot(QQuailBListItem *item);
		void expandContactSlot();
		void expandContactSlot(QQuailBListItem *item);
		void saveBlistSlot();

		void showContextMenuSlot(QListViewItem *item, const QPoint &point,
								 int col);

		/* Group menu slots */
		void addBuddySlot();
		void addChatSlot();
		void removeGroupSlot();
		void renameGroupSlot();

		/* Buddy menu slots */
		void userInfoSlot(void *data);
		void sendImSlot(void *data);
		void protoActionSlot(void *data1, void *data2);
		void aliasBuddySlot(void *data);
		void removeBuddySlot(void *data);

		/* Contact menu slots */
		void removeContactSlot();

		/* Chat menu slots */
		void joinChatSlot();
		void autoJoinChatSlot(bool on);
		void removeChatSlot();
		void aliasChatSlot();

	private:
		void addGroup(GaimBlistNode *node);

		void updateGroup(GaimBlistNode *node);
		void updateContact(GaimBlistNode *node);
		void updateBuddy(GaimBlistNode *node);
		void updateChat(GaimBlistNode *node);

	private:
		GaimBuddyList *gaimBlist;

		QTimer *saveTimer;
};

#endif /* _QGAIM_BUDDY_LIST_H_ */
