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

	QTabBar::paintLabel(p, rect, tab, hasFocus);

	if (tab->id == currentTab())
		r.setBottom(r.bottom() - style().defaultFrameWidth());

	p->setPen(colors[tab->id]);
	p->drawText(r, AlignCenter | ShowPrefix, tab->label);
}

int
QGaimTabBar::insertTab(QTab *tab, int index)
{
	int newIndex = index;

	if (index == -1)
		newIndex = tabList()->count();

	colors[newIndex] = black;

	lastId = newIndex;

	return QTabBar::insertTab(tab, index);
}