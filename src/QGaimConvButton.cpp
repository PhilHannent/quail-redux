/**
 * @file QGaimConvButton.cpp Conversation menu button
 *
 * @Copyright (C) 2003 Christian Hammond.
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
#include "QGaimConvButton.h"
#include "QGaimProtocolUtils.h"
#include "QGaim.h"
#include "base.h"

#include <libgaim/debug.h>
#include <libgaim/signals.h>

#include <qpe/resource.h>
#include <qpopupmenu.h>

static void
newConvCb(char *, QGaimConvButton *button)
{
	button->setEnabled(true);
}

static void
delConvCb(char *, QGaimConvButton *button)
{
	if (gaim_get_conversations()->next == NULL)
		button->setEnabled(false);
}

QGaimConvButton::QGaimConvButton(QWidget *parent, const char *name)
	: QToolButton(parent, name), convs(NULL)
{
	setAutoRaise(true);
	setPixmap(Resource::loadPixmap("gaim/conversations"));

	menu = new QPopupMenu();

	setPopup(menu);

	connect(menu, SIGNAL(aboutToShow()),
			this, SLOT(generateMenu()));
	connect(menu, SIGNAL(activated(int)),
			this, SLOT(convActivated(int)));

	connect(this, SIGNAL(clicked()),
			this, SLOT(buttonClicked()));

	gaim_signal_connect(gaim_conversations_get_handle(),
						"conversation-created",
						this, GAIM_CALLBACK(newConvCb), this);
	gaim_signal_connect(gaim_conversations_get_handle(),
						"deleting-conversation",
						this, GAIM_CALLBACK(delConvCb), this);

	if (gaim_get_conversations() == NULL)
		setEnabled(false);
}

QGaimConvButton::~QGaimConvButton()
{
	delete menu;

	if (convs != NULL)
		delete convs;

	gaim_signal_connect(gaim_conversations_get_handle(),
						"conversation-created",
						this, GAIM_CALLBACK(newConvCb), this);
	gaim_signal_connect(gaim_conversations_get_handle(),
						"deleting-conversation",
						this, GAIM_CALLBACK(delConvCb), this);
}

void
QGaimConvButton::generateMenu()
{
	GaimConversation *conv;
	GList *l;
	size_t size;
	int i;

	menu->clear();

	if (convs != NULL)
		delete convs;

	if (gaim_get_conversations() == NULL)
		return;

	size = g_list_length(gaim_get_conversations());

	convs = new GaimConversation*[size];

	for (l = gaim_get_conversations(), i = 0; l != NULL; l = l->next, i++)
	{
		GaimAccount *account;

		conv = (GaimConversation *)l->data;
		account = gaim_conversation_get_account(conv);

		menu->insertItem(QGaimProtocolUtils::getProtocolIcon(account),
						 gaim_conversation_get_title(conv), i);

		convs[i] = conv;
	}
}

void
QGaimConvButton::convActivated(int id)
{
	GaimConversation *conv;
	GaimWindow *win;

	conv = convs[id];

	if (g_list_find(gaim_get_conversations(), conv) == NULL)
	{
		/*
		 * The conversation was somehow removed since this menu was
		 * created.
		 */
		return;

	}

	win = gaim_conversation_get_window(conv);

	gaim_window_switch_conversation(win, gaim_conversation_get_index(conv));
	gaim_window_raise(win);
}

void
QGaimConvButton::buttonClicked()
{
	GaimWindow *lastWin = qGaimGetHandle()->getLastActiveConvWindow();

	if (lastWin != NULL)
		gaim_window_raise(lastWin);
}
