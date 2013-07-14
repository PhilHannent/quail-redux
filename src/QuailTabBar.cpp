/**
 * @file QQuailTabBar.cpp Tab bar with colored label support
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
#include "QuailTabBar.h"

#include <libpurple/debug.h>

/**************************************************************************
 * QQuailTabBar
 **************************************************************************/
QQuailTabBar::QQuailTabBar(QWidget *parent)
    : QTabBar(parent), lastId(0)
{
}

void
QQuailTabBar::setTabColor(int id, const QColor &color)
{
//    QWidget *t = this->tabAt(id);

//	if (t != NULL)
//	{
//		colors[id] = color;

//		repaint(t->rect(), false);
//	}
}

const QColor &
QQuailTabBar::getTabColor(int id) const
{
//	if (colors.contains(id))
//		return colors[id];

//	return colorGroup().foreground();
}

void
QQuailTabBar::setCurrentIndex(int index)
{
    //setCurrentIndex(tabList()->at(index));
}

int
QQuailTabBar::getCurrentIndex()
{
    //return tabList()->find(tab(currentIndex()));
}

int
QQuailTabBar::getLastId() const
{
	return lastId;
}

void
QQuailTabBar::paintLabel(QPainter *p, const QRect &rect,
                        QWidget *tab, bool hasFocus) const
{
	QRect r = rect;

//	if (tab->iconSet())
//	{
//        QIcon::Mode mode = ((tab->isEnabled() && isEnabled())
//                               ? QIcon::Normal : QIcon::Disabled);

//        if (mode == QIcon::Normal && hasFocus)
//            mode = QIcon::Active;

//        QPixmap pixmap = tab->iconSet()->pixmap(QIcon::Small, mode);

//		int pixw = pixmap.width();
//		int pixh = pixmap.height();

//		r.setLeft(r.left() + pixw + 2);

//		p->drawPixmap(rect.left() + 2,
//					  rect.center().y() - pixh / 2,
//					  pixmap);
//	}

//	p->setPen(colors[tab->id]);
//	p->drawText(r, AlignCenter | ShowPrefix, tab->label);
}

int
QQuailTabBar::insertTab(QWidget *tab, int index)
{
//    int id = this->addTab(tab, index);

//	lastId = id;

//	return id;
}
