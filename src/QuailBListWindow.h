﻿/**
 * @file QQuailBListWindow.h Buddy list window
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
#ifndef _QUAIL_BLIST_WIN_H_
#define _QUAIL_BLIST_WIN_H_

#include <libpurple/blist.h>

#include <QIcon>
#include <QMainWindow>

class QAction;
class QComboBox;
class QLabel;
class QMenuBar;
class QToolBar;
class QToolButton;
class QTreeWidgetItem;

class quail_accounts_window;
class quail_buddy_list;

class quail_blist_window : public QMainWindow
{
	Q_OBJECT

	public:
		quail_blist_window(QMainWindow *parent);
		~quail_blist_window();

		/* Functions our blist UI must access. */
        void setBlist(PurpleBuddyList *list);
        PurpleBuddyList *getBlist() const;

		void account_signed_on(PurpleAccount *acocunt);
        void account_signed_off(PurpleAccount *acocunt);

		void updateNode(PurpleBlistNode *node);

		void reloadList();

	protected slots:
        void nodeChanged(QTreeWidgetItem *item, QTreeWidgetItem *prevItem);
        void currentNodeChanged(QTreeWidgetItem *item, QTreeWidgetItem *prev);

        void doubleClickList(QTreeWidgetItem *item, int col);

		void showAddBuddy(PurpleGroup *group);
		void showAddBuddy();
		void showAddChat(PurpleGroup *group);
		void showAddChat();
		void showAddGroup();

		void showRemoveBuddy();
		void showConfirmRemoveBuddy(PurpleBuddy *buddy);
		void showConfirmRemoveContact(PurpleContact *contact);
		void showConfirmRemoveChat(PurpleChat *chat);
		void showConfirmRemoveGroup(PurpleGroup *group);

		void showOfflineBuddies(bool on);
		void blistToggled(bool state);
		void openImSlot(PurpleBuddy *buddy);
		void openImSlot();
		void openChatSlot(PurpleChat *chat);
		void openChatSlot();

	private:
		void buildInterface();
		void buildToolBar();

	private:
		QMainWindow *parentMainWindow;

        quail_buddy_list *m_buddy_tree;

		QToolButton *addButton;

        QIcon openChatIconSet;
        QIcon newChatIconSet;
        QIcon sendImIconSet;
        QIcon newImIconSet;

		QAction *imButton;
		QAction *chatButton;
		QAction *infoButton;
		QAction *awayButton;

		QAction *addBuddyButton;
		QAction *addChatButton;
		QAction *addGroupButton;
		QAction *removeButton;

		QAction *showOfflineButton;

		QAction *blistButton;
        QAction *metersButton;

        QMenu *addMenu;
        QMenu *settingsMenu;
        QMenu *convsMenu;

        QToolBar *toolbar;
};

PurpleBlistUiOps *quail_get_blist_ui_ops();

#endif /* _QUAIL_BLIST_WIN_H_ */
