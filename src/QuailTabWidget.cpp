/**
 * @file QQuailTabWidget.cpp Tab widget with colored label support.
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
#include "QuailTabWidget.h"
#include "QuailTabBar.h"

QQuailTabWidget::QQuailTabWidget(QWidget *parent, const char *name)
    : QTabWidget(parent)
{
    setTabBar(new QQuailTabBar(this));
}

void
QQuailTabWidget::setCurrentIndex(int index)
{
	((QQuailTabBar *)tabBar())->setCurrentIndex(index);
}

int
QQuailTabWidget::getCurrentIndex() const
{
	return ((QQuailTabBar *)tabBar())->getCurrentIndex();
}

void
QQuailTabWidget::setTabColor(int index, const QColor &color)
{
	((QQuailTabBar *)tabBar())->setTabColor(index, color);
}

int
QQuailTabWidget::getLastId() const
{
	return ((QQuailTabBar *)tabBar())->getLastId();
}
