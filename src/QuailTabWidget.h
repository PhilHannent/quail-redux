﻿/**
 * @file QQuailTabWidget.h Tab widget with colored label support.
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
#ifndef _QUAIL_TAB_WIDGET_H_
#define _QUAIL_TAB_WIDGET_H_

#include <QTabWidget>

class quail_tab_widget : public QTabWidget
{
	Q_OBJECT

	public:
        quail_tab_widget(QWidget *parent = NULL);

		void setCurrentIndex(int index);
		int getCurrentIndex() const;

		void setTabColor(int index, const QColor &color);

		int getLastId() const;

    public slots:
        void slotSendMessage();
        void slotSendEnabled(bool bEnabled);
        void slotToggleUserList();

    signals:
        void signalSendEnabled(bool bEnabled);
};

#endif /* _QUAIL_TAB_WIDGET_H_ */
