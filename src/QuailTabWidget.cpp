/**
 * @file QQuailTabWidget.cpp Tab widget with colored label support.
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

#include "libpurple/conversation.h"

#include "QuailTabWidget.h"
#include "QuailTabBar.h"
#include "QuailConvWindow.h"

#include <QDebug>

QQuailTabWidget::QQuailTabWidget(QWidget *parent)
    : QTabWidget(parent)
{
    setTabBar(new QQuailTabBar(this));
    connect(this, SIGNAL(currentChanged(int)),
            parent, SLOT(tabChanged(int)));

    connect(parent, SIGNAL(signalSendMessage()),
            this, SLOT(slotSendMessage()));

    connect(this, SIGNAL(signalSendEnabled(bool)),
            parent, SLOT(slotSendEnabled(bool)));

    connect(parent, SIGNAL(signalToggleUserList()),
            this, SLOT(slotToggleUserList()));
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

void
QQuailTabWidget::slotSendEnabled(bool bEnabled)
{
    qDebug() << "QQuailTabWidget::slotSendEnabled(bool bEnabled)";
    emit signalSendEnabled(bEnabled);
}

void
QQuailTabWidget::slotSendMessage()
{
    qDebug() << "QQuailTabWidget::slotSendMessage()";
    QQuailConversation* qconv = (QQuailConversation*)this->currentWidget();
    qconv->send();
}

void
QQuailTabWidget::slotToggleUserList()
{
    qDebug() << "QQuailTabWidget::slotToggleUserList()";
    QQuailConversation *qconv = (QQuailConversation*)this->currentWidget();
    PurpleConversation *conv = qconv->getConversation();
    /* Toogle the user list */
    if (purple_conversation_get_type(conv) == PURPLE_CONV_TYPE_CHAT)
    {
        QQuailConvChat *chat = (QQuailConvChat*)qconv;
        chat->setShowUserList(!chat->getShowUserList());
    }
}
