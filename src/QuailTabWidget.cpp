/**
 * @file QGaimTabWidget.cpp Tab widget with colored label support.
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

QGaimTabWidget::QGaimTabWidget(QWidget *parent, const char *name)
	: QTabWidget(parent, name)
{
	setTabBar(new QGaimTabBar(this, "tab control"));
}

void
QGaimTabWidget::setCurrentIndex(int index)
{
	((QGaimTabBar *)tabBar())->setCurrentIndex(index);
}

int
QGaimTabWidget::getCurrentIndex() const
{
	return ((QGaimTabBar *)tabBar())->getCurrentIndex();
}

void
QGaimTabWidget::setTabColor(int index, const QColor &color)
{
	((QGaimTabBar *)tabBar())->setTabColor(index, color);
}

int
QGaimTabWidget::getLastId() const
{
	return ((QGaimTabBar *)tabBar())->getLastId();
}
