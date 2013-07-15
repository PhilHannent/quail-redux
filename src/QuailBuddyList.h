/**
 * @file QQuailBuddyList.h Buddy list widget
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
#ifndef _QUAIL_BUDDY_LIST_H_
#define _QUAIL_BUDDY_LIST_H_

#include <libpurple/account.h>
#include <libpurple/blist.h>

#include <QTreeWidget>

#include "QuailImageUtils.h"

class QQuailBListItem : public QTreeWidgetItem
{
	public:
        QQuailBListItem(QTreeWidget *parent, PurpleBlistNode *node);
        QQuailBListItem(QTreeWidgetItem *parent, PurpleBlistNode *node);
		~QQuailBListItem();

		PurpleBlistNode *getBlistNode() const;

		void updateInfo();
		void setExpanded(bool expanded);
		bool isExpanded() const;

	protected:
		void init();

        void paintBuddyInfo(QPainter *p, const QPalette &cg, int column,
							int width, int align, int lmarg, int itMarg);
        void paintGroupInfo(QPainter *p, const QPalette &cg, int column,
							int width, int align, int lmarg, int itMarg);

	private:
		PurpleBlistNode *node;
		bool expanded;
		bool dirty;
		QString topText, bottomText;
		int textY1, textY2;
};

class QQuailBuddyList : public QTreeWidget
{
	Q_OBJECT

	public:
        static QPixmap getBuddyStatusIcon(PurpleBlistNode *node);

	public:
		QQuailBuddyList(QWidget *parent = NULL, const char *name = NULL);
		virtual ~QQuailBuddyList();

        void setBlist(PurpleBuddyList *list);
        PurpleBuddyList *getBlist() const;

		void updateNode(PurpleBlistNode *node);

		void reload(bool remove = false);

		PurpleBuddy *getSelectedBuddy() const;

	signals:
		void openIm(PurpleBuddy *buddy);
		void addBuddy(PurpleGroup *group);
		void addChat(PurpleGroup *group);
		void joinChat(PurpleChat *chat);
		void removeBuddy(PurpleBuddy *buddy);
		void removeContact(PurpleContact *contact);
		void removeChat(PurpleChat *chat);
		void removeGroup(PurpleGroup *group);

	protected:
        void populateBuddyMenu(PurpleBuddy *buddy, QMenu *menu,
							   bool asContact);
        void populateContactMenu(PurpleContact *contact, QMenu *menu);
        void populateChatMenu(PurpleChat *chat, QMenu *menu);
        void populateGroupMenu(PurpleGroup *group, QMenu *menu);

	protected slots:
		void resizeEvent(QResizeEvent *event);

        void nodeExpandedSlot(QTreeWidgetItem *item);
        void nodeCollapsedSlot(QTreeWidgetItem *item);
		void collapseContactSlot();
		void collapseContactSlot(QQuailBListItem *item);
		void expandContactSlot();
		void expandContactSlot(QQuailBListItem *item);
		void saveBlistSlot();

        void showContextMenuSlot(QTreeWidgetItem *item, const QPoint &point,
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
		void addGroup(PurpleBlistNode *node);

		void updateGroup(PurpleBlistNode *node);
		void updateContact(PurpleBlistNode *node);
		void updateBuddy(PurpleBlistNode *node);
		void updateChat(PurpleBlistNode *node);

	private:
        PurpleBuddyList *quailBlist;

		QTimer *saveTimer;
};

#endif /* _QUAIL_BUDDY_LIST_H_ */
