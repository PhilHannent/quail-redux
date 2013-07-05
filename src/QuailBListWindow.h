/**
 * @file QGaimBListWindow.h Buddy list window
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
class QGaimAccountsWindow;

class QGaimBListWindow : public QMainWindow
{
	Q_OBJECT

	public:
		QGaimBListWindow(QMainWindow *parent);
		~QGaimBListWindow();

		/* Functions our blist UI must access. */
		void setGaimBlist(GaimBuddyList *list);
		GaimBuddyList *getGaimBlist() const;

		void accountSignedOn(GaimAccount *acocunt);
		void accountSignedOff(GaimAccount *acocunt);

		void updateNode(GaimBlistNode *node);

		void reloadList();

	protected slots:
		void nodeChanged(QListViewItem *item);

		void doubleClickList(QListViewItem *item);

		void showAddBuddy(GaimGroup *group);
		void showAddBuddy();
		void showAddChat(GaimGroup *group);
		void showAddChat();
		void showAddGroup();

		void showRemoveBuddy();
		void showConfirmRemoveBuddy(GaimBuddy *buddy);
		void showConfirmRemoveContact(GaimContact *contact);
		void showConfirmRemoveChat(GaimChat *chat);
		void showConfirmRemoveGroup(GaimGroup *group);

		void showOfflineBuddies(bool on);
		void showPreferencesSlot();

		void showAccountsWindow();
		void blistToggled(bool state);
		void openImSlot(GaimBuddy *buddy);
		void openImSlot();
		void openChatSlot(GaimChat *chat);
		void openChatSlot();

	private:
		void buildInterface();
		void buildToolBar();

	private:
		QMainWindow *parentMainWindow;

		QGaimBuddyList *buddylist;

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

GaimBlistUiOps *qGaimGetBlistUiOps();

#endif /* _QGAIM_BLIST_WIN_H_ */
