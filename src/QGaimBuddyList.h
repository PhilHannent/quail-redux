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

class QGaimBuddyList : public QListView
{
	Q_OBJECT

	public:
		QGaimBuddyList(QWidget *parent = NULL, const char *name = NULL);
		virtual ~QGaimBuddyList();

		void setGaimBlist(struct gaim_buddy_list *list);
		struct gaim_buddy_list *getGaimBlist() const;

		void updateNode(GaimBlistNode *node);

	private:
		void addGroup(GaimBlistNode *node);

	private:
		struct gaim_buddy_list *gaimBlist;

};

#endif /* _QGAIM_BUDDY_LIST_H_ */
