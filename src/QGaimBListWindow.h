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
#include <qlistview.h>

#include "QGaimAccountsWindow.h"

class QMenuBar;
class QToolBar;
class QToolButton;
class QGaimAccountsWindow;

class QGaimBListItem : public QListViewItem
{
	public:
		QGaimBListItem(QListView *parent, GaimBlistNode *node);
		QGaimBListItem(QListViewItem *parent, GaimBlistNode *node);
		~QGaimBListItem();

		GaimBlistNode *getBlistNode() const;

		void paintBranches(QPainter *p, const QColorGroup &cg,
						   int width, int y, int height, GUIStyle s);

	protected:
		void init();

	private:
		GaimBlistNode *node;
};

class QGaimBListWindow : public QMainWindow
{
	Q_OBJECT

	public:
		QGaimBListWindow();
		~QGaimBListWindow();

		void setGaimBlist(struct gaim_buddy_list *list);
		struct gaim_buddy_list *getGaimBlist() const;

		void update(GaimBlistNode *node);

	protected slots:
		void im();
		void chat();
		void showAddBuddy();
		void showAccountsWindow();
		void blistToggled(bool state);
		void sendIm();

		void nodeChanged(QListViewItem *item);

	private:
		void buildInterface();
		void buildToolBar();

		void add_group(GaimBlistNode *node);
		QToolButton *newButton(QToolBar *toolbar, const QString image,
							   bool toggle = false, bool on = false);

	private:
		struct gaim_buddy_list *gaimBlist;

		QToolButton *imButton;
		QToolButton *chatButton;
		QToolButton *infoButton;
		QToolButton *awayButton;

		QToolButton *blistButton;

		QPopupMenu *convsMenu;

		QListView *buddylist;
		QToolBar *toolbar;
};

struct gaim_blist_ui_ops *qGaimGetBlistUiOps();

#endif /* _QGAIM_BLIST_WIN_H_ */
