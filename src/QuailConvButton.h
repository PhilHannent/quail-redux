﻿/**
 * @file QQuailConvButton.h Conversation menu button
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
#ifndef _QUAIL_CONV_BUTTON_H_
#define _QUAIL_CONV_BUTTON_H_

#include <QVariant>
#include <QWidget>
#include <QToolButton>

#include <libpurple/conversation.h>

class QMenu;

class quail_conv_button : public QToolButton
{
	Q_OBJECT

	public:
        quail_conv_button(QWidget *parent = NULL);
		~quail_conv_button();

	protected slots:
		void generateMenu();
        void convActivated(QAction *id);
		void buttonClicked();

    signals:
        void signalShowConvWindow();

	private:
        QMenu *menu;
        PurpleConversation **convs;
};

#endif /* _QUAIL_CONV_BUTTON_H_ */
