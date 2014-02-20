/**
 * @file QQuailAccountBox.cpp Account drop-down menu
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
#include "QuailAccountBox.h"
#include "QuailProtocolUtils.h"

#include <libpurple/debug.h>

#include <QPixmap>

quail_account_box::quail_account_box(bool showAll, QWidget *parent)
    : QComboBox(parent), showAll(showAll)
{

	if (showAll)
		buildMenu((PurpleAccount *)purple_accounts_get_all()->data);
	else
	{
		PurpleConnection *gc;

		gc = (PurpleConnection *)purple_connections_get_all()->data;

		buildMenu(purple_connection_get_account(gc));
	}
}

quail_account_box::quail_account_box(PurpleAccount *account, bool showAll,
                                 QWidget *parent)
    : QComboBox(parent), showAll(showAll)
{
	buildMenu(account);
}

void
quail_account_box::setCurrentAccount(PurpleAccount *account)
{
	PurpleAccount *tempAccount;
	GList *l, *list;
	int i;

	if (showAll)
		list = purple_accounts_get_all();
	else
		list = purple_connections_get_all();

	for (l = list, i = 0; l != NULL; l = l->next, i++)
	{
		if (showAll)
			tempAccount = (PurpleAccount *)l->data;
		else
		{
			PurpleConnection *gc = (PurpleConnection *)l->data;

			tempAccount = purple_connection_get_account(gc);
		}

		if (tempAccount == account)
		{
            setCurrentIndex(i);
			break;
		}
	}
}

PurpleAccount *
quail_account_box::getCurrentAccount() const
{
    int index = currentIndex();
	GList *l;

	if (showAll)
	{
		l = g_list_nth(purple_accounts_get_all(), index);

		if (l == NULL)
			return NULL;

		return (PurpleAccount *)l->data;
	}
	else
	{
		PurpleConnection *gc;

		l = g_list_nth(purple_connections_get_all(), index);

		if (l == NULL)
			return NULL;

		gc = (PurpleConnection *)l->data;

		return purple_connection_get_account(gc);
	}
}

void
quail_account_box::buildMenu(PurpleAccount *account)
{
	GList *l, *list;
	int count;

	clear();

	if (showAll)
		list = purple_accounts_get_all();
	else
		list = purple_connections_get_all();

	for (l = list, count = 0; l != NULL; l = l->next, count++)
	{
		PurpleAccount *tempAccount;
		QPixmap pixmap;

		if (showAll)
			tempAccount = (PurpleAccount *)l->data;
		else
		{
			PurpleConnection *gc = (PurpleConnection *)l->data;

			tempAccount = purple_connection_get_account(gc);
		}

		QString str;
		QFontMetrics fm(fontMetrics());

		str = purple_account_get_username(tempAccount);

		pixmap = QQuailProtocolUtils::getProtocolIcon(tempAccount);

		int pw = (pixmap.isNull() ? 10 : pixmap.width() + 10);
		int itemWidth;

		itemWidth = width() - 2 - 16;

		if (fm.width(str) + pw > itemWidth)
		{
			int i = 0;
			QString tempText = "...";

			while (fm.width(tempText + str[i]) + pw < itemWidth)
				tempText += str[i++];

			tempText.remove(0, 3);

			if (tempText.isEmpty())
				tempText = str.left(1);

			str = tempText + "...";
		}

        addItem(QIcon(pixmap), str);

		if (tempAccount == account)
            setCurrentIndex(count);
	}
}

void
quail_account_box::resizeEvent(QResizeEvent *event)
{
	buildMenu(getCurrentAccount());

	QComboBox::resizeEvent(event);
}
