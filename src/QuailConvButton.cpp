/**
 * @file QQuailConvButton.cpp Conversation menu button
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
#include "QuailConvButton.h"
#include "QuailBuddyList.h"
#include "QuailProtocolUtils.h"
#include "QuailMainWindow.h"

#include <libpurple/debug.h>
#include <libpurple/signals.h>

#include <QDebug>
#include <QMenu>

static void
newConvCb(char *, QQuailConvButton *button)
{
	button->setEnabled(true);
}

static void
delConvCb(char *, QQuailConvButton *button)
{
	if (purple_get_conversations()->next == NULL)
		button->setEnabled(false);
}

QQuailConvButton::QQuailConvButton(QWidget *parent)
    : QToolButton(parent), convs(NULL)
{
    qDebug() << "QQuailConvButton::QQuailConvButton";
    setIcon(QIcon(QPixmap(":/data/images/actions/conversations.png")));

    menu = new QMenu(this);

    setMenu(menu);

	connect(menu, SIGNAL(aboutToShow()),
			this, SLOT(generateMenu()));
    qDebug() << "QQuailConvButton::QQuailConvButton.1";

    connect(menu, SIGNAL(triggered(QAction*)),
            this, SLOT(convActivated(QAction*)));
    qDebug() << "QQuailConvButton::QQuailConvButton.2";

	connect(this, SIGNAL(clicked()),
			this, SLOT(buttonClicked()));
    qDebug() << "QQuailConvButton::QQuailConvButton.3";

	purple_signal_connect(purple_conversations_get_handle(),
						"conversation-created",
                        this, PURPLE_CALLBACK(newConvCb), this);
	purple_signal_connect(purple_conversations_get_handle(),
						"deleting-conversation",
                        this, PURPLE_CALLBACK(delConvCb), this);

	if (purple_get_conversations() == NULL)
		setEnabled(false);
    qDebug() << "QQuailConvButton::QQuailConvButton.end";

}

QQuailConvButton::~QQuailConvButton()
{
	delete menu;

	if (convs != NULL)
		delete convs;

	purple_signal_disconnect(purple_conversations_get_handle(),
						   "conversation-created",
                           this, PURPLE_CALLBACK(newConvCb));
	purple_signal_disconnect(purple_conversations_get_handle(),
						   "deleting-conversation",
                           this, PURPLE_CALLBACK(delConvCb));
}

void
QQuailConvButton::generateMenu()
{
    PurpleConversation *conv;
	GList *l;
	size_t size;
	int i;

	menu->clear();

	if (convs != NULL)
		delete convs;

	if (purple_get_conversations() == NULL)
		return;

	size = g_list_length(purple_get_conversations());

    convs = new PurpleConversation*[size];

	for (l = purple_get_conversations(), i = 0; l != NULL; l = l->next, i++)
	{
		PurpleAccount *account;
		PurpleBuddy *buddy;

        conv = (PurpleConversation *)l->data;
		account = purple_conversation_get_account(conv);

		buddy = purple_find_buddy(account, purple_conversation_get_name(conv));

		if (buddy == NULL)
		{
//            menu->addAction(
//                QQuailProtocolUtils::getProtocolIcon(account),
//				purple_conversation_get_title(conv), i);
		}
		else
		{
//            menu->addAction(
//                QQuailBuddyList::getBuddyStatusIcon((PurpleBlistNode *)buddy),
//				purple_conversation_get_title(conv), i);
		}

		convs[i] = conv;
	}
}

void
QQuailConvButton::convActivated(QAction *action)
{
    PurpleConversation *conv;
    QQuailConvWindow *win;

    conv = convs[action->data().toInt()];

	if (g_list_find(purple_get_conversations(), conv) == NULL)
	{
		/*
		 * The conversation was somehow removed since this menu was
		 * created.
		 */
		return;

	}

//	win = purple_conversation_get_window(conv);

//	purple_conv_window_switch_conversation(win, purple_conversation_get_index(conv));
//	purple_conv_window_raise(win);
}

void
QQuailConvButton::buttonClicked()
{
    QQuailConversation *lastWin = qQuailGetMainWindow()->getLastActiveConvWindow();

//	if (lastWin != NULL)
//		purple_conv_window_raise(lastWin);
}
