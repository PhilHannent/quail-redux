/**
 * @file QQuailTabBar.cpp Tab bar with colored label support
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
#include "QuailTabBar.h"

#include <libpurple/debug.h>

/**************************************************************************
 * QQuailTabBar
 **************************************************************************/
QQuailTabBar::QQuailTabBar(QWidget *parent, const char *name)
	: QTabBar(parent, name), lastId(0)
{
}

void
QQuailTabBar::setTabColor(int id, const QColor &color)
{
	QTab *t = tab(id);

	if (t != NULL)
	{
		colors[id] = color;

		repaint(t->rect(), false);
	}
}

const QColor &
QQuailTabBar::getTabColor(int id) const
{
	if (colors.contains(id))
		return colors[id];

	return colorGroup().foreground();
}

void
QQuailTabBar::setCurrentIndex(int index)
{
	setCurrentTab(tabList()->at(index));
}

int
QQuailTabBar::getCurrentIndex()
{
	return tabList()->find(tab(currentTab()));
}

int
QQuailTabBar::getLastId() const
{
	return lastId;
}

void
QQuailTabBar::paintLabel(QPainter *p, const QRect &rect,
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
QQuailTabBar::insertTab(QTab *tab, int index)
{
	int id = QTabBar::insertTab(tab, index);

	lastId = id;

	return id;
}
