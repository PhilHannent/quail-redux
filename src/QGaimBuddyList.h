/**
 * @file QGaimBuddyList.h Buddy list widget
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
#ifndef _QGAIM_BUDDY_LIST_H_
#define _QGAIM_BUDDY_LIST_H_

#include <libgaim/account.h>
#include <libgaim/blist.h>

#include <qlistview.h>

#include "QGaimImageUtils.h"

class QGaimBListItem : public QListViewItem
{
	public:
		QGaimBListItem(QListView *parent, GaimBlistNode *node);
		QGaimBListItem(QListViewItem *parent, GaimBlistNode *node);
		~QGaimBListItem();

		GaimBlistNode *getBlistNode() const;

		void updateInfo();

		void paintBranches(QPainter *p, const QColorGroup &cg,
						   int width, int y, int height, GUIStyle s);

		void setExpanded(bool expanded);
		bool isExpanded() const;

	protected:
		void init();

	private:
		GaimBlistNode *node;
		bool expanded;
};

class QGaimBuddyList : public QListView
{
	Q_OBJECT

	public:
		static QPixmap getBuddyStatusIcon(GaimBlistNode *node,
			QGaimPixmapSize size = QGAIM_PIXMAP_SMALL);

	public:
		QGaimBuddyList(QWidget *parent = NULL, const char *name = NULL);
		virtual ~QGaimBuddyList();

		void setGaimBlist(GaimBuddyList *list);
		GaimBuddyList *getGaimBlist() const;

		void updateNode(GaimBlistNode *node);

		void reload(bool remove = false);

	protected slots:
		void nodeExpanded(QListViewItem *item);
		void nodeCollapsed(QListViewItem *item);
		void showContextMenu(QListViewItem *item, const QPoint &point, int col);
		void sendIm() {}
		void showRemoveBuddy() {}
		void showRemoveContact() {}
		void collapseContact();
		void collapseContact(QGaimBListItem *item);
		void expandContact();
		void expandContact(QGaimBListItem *item);
		void saveBlist();

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
