/**
 * @file QGaimBListWindow.h Buddy list window
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
#ifndef _QGAIM_BLIST_WIN_H_
#define _QGAIM_BLIST_WIN_H_

#include <libgaim/blist.h>

#include <qvariant.h>
#include <qwidget.h>
#include <qmainwindow.h>

#include "QGaimAccountsWindow.h"

class QAction;
class QGaimBuddyList;
class QMenuBar;
class QToolBar;
class QToolButton;
class QGaimAccountsWindow;

class QGaimBListWindow : public QMainWindow
{
	Q_OBJECT

	public:
		QGaimBListWindow();
		~QGaimBListWindow();

		/* Functions our blist UI must access. */
		void setGaimBlist(struct gaim_buddy_list *list);
		struct gaim_buddy_list *getGaimBlist() const;

		void accountSignedOn(GaimAccount *acocunt);
		void accountSignedOff(GaimAccount *acocunt);

		void updateNode(GaimBlistNode *node);

	protected slots:
		void nodeChanged(QListViewItem *item);

		void im();
		void chat();

		void showAddBuddy();
		void showAddChat();
		void showAddGroup();
		void showRemoveBuddy();
		void showOfflineBuddies(bool on);

		void showAccountsWindow();
		void blistToggled(bool state);
		void sendIm();
		void openChat();

	private:
		void buildInterface();
		void buildToolBar();

	private:
		QGaimBuddyList *buddylist;

		QToolButton *addButton;

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

struct gaim_blist_ui_ops *qGaimGetBlistUiOps();

#endif /* _QGAIM_BLIST_WIN_H_ */
