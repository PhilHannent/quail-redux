/**
 * @file QGaimTabBar.cpp Tab bar with colored label support
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
#include "QGaimTabBar.h"

#include <libgaim/debug.h>

/**************************************************************************
 * QGaimTabBar
 **************************************************************************/
QGaimTabBar::QGaimTabBar(QWidget *parent, const char *name)
	: QTabBar(parent, name), lastId(0)
{
}

void
QGaimTabBar::setTabColor(int id, const QColor &color)
{
	QTab *t = tab(id);

	if (t != NULL)
	{
		colors[id] = color;

		repaint(t->rect(), false);
	}
}

const QColor &
QGaimTabBar::getTabColor(int id) const
{
	if (colors.contains(id))
		return colors[id];

	return colorGroup().foreground();
}

void
QGaimTabBar::setCurrentIndex(int index)
{
	setCurrentTab(tabList()->at(index));
}

int
QGaimTabBar::getCurrentIndex()
{
	return tabList()->find(tab(currentTab()));
}

int
QGaimTabBar::getLastId() const
{
	return lastId;
}

void
QGaimTabBar::paintLabel(QPainter *p, const QRect &rect,
						QTab *tab, bool hasFocus) const
{
	QRect r = rect;

	if (tab->iconSet())
	{
		QIconSet::Mode mode = ((tab->isEnabled() && isEnabled())
							   ? QIconSet::Normal : QIconSet::Disabled);

		if (mode == QIconSet::Normal && hasFocus)
			mode = QIconSet::Active;

		QPixmap pixmap = tab->iconSet()->pixmap(QIconSet::Small, mode);

		int pixw = pixmap.width();
		int pixh = pixmap.height();

		r.setLeft(r.left() + pixw + 2);

		p->drawPixmap(rect.left() + 2,
					  rect.center().y() - pixh / 2,
					  pixmap);
	}

	p->setPen(colors[tab->id]);
	p->drawText(r, AlignCenter | ShowPrefix, tab->label);
}

int
QGaimTabBar::insertTab(QTab *tab, int index)
{
	int id = QTabBar::insertTab(tab, index);

	lastId = id;

	return id;
}
