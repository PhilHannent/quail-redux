/**
 * @file QQuailTabBar.h Tab bar with colored label support
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
#ifndef _QGAIM_TAB_BAR_H_
#define _QGAIM_TAB_BAR_H_

#include <QColor>
#include <QList>
#include <QMap>
#include <QTabBar>

class QQuailTabBar : public QTabBar
{
	Q_OBJECT

	public:
		QQuailTabBar(QWidget *parent = NULL, const char *name = NULL);

		void setTabColor(int id, const QColor &color);
		const QColor &getTabColor(int id) const;

		void setCurrentIndex(int index);
		int getCurrentIndex();

		int getLastId() const;

        virtual int insertTab(QWidget *tab, int index = -1);

	protected:
		virtual void paintLabel(QPainter *painter, const QRect &rect,
                                QWidget *tab, bool hasFocus) const;

	protected:
		QMap<int, QColor> colors;
		int lastId;
};

#endif /* _QGAIM_TAB_BAR_H_ */
