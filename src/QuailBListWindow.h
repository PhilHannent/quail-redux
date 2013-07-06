/**
 * @file QQuailBListWindow.h Buddy list window
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
#ifndef _QGAIM_BLIST_WIN_H_
#define _QGAIM_BLIST_WIN_H_

#include <libpurple/blist.h>

#include <qiconset.h>
#include <qmainwindow.h>

#include "QuailAccountsWindow.h"
#include "QuailBuddyList.h"

class QAction;
class QMenuBar;
class QToolBar;
class QToolButton;
class QQuailAccountsWindow;

class QQuailBListWindow : public QMainWindow
{
	Q_OBJECT

	public:
		QQuailBListWindow(QMainWindow *parent);
		~QQuailBListWindow();

		/* Functions our blist UI must access. */
		void setGaimBlist(PurpleBuddyList *list);
		PurpleBuddyList *getGaimBlist() const;

		void accountSignedOn(PurpleAccount *acocunt);
		void accountSignedOff(PurpleAccount *acocunt);

		void updateNode(GaimBlistNode *node);

		void reloadList();

	protected slots:
		void nodeChanged(QListViewItem *item);

		void doubleClickList(QListViewItem *item);

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
		void showPreferencesSlot();

		void showAccountsWindow();
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

		QQuailBuddyList *buddylist;

		QToolButton *addButton;

		QIconSet openChatIconSet;
		QIconSet newChatIconSet;
		QIconSet sendImIconSet;
		QIconSet newImIconSet;

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

		QPopupMenu *addMenu;
		QPopupMenu *settingsMenu;
		QPopupMenu *convsMenu;

		QToolBar *toolbar;
};

GaimBlistUiOps *qQuailGetBlistUiOps();

#endif /* _QGAIM_BLIST_WIN_H_ */
