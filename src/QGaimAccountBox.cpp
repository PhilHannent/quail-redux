/**
 * @file QGaimAccountBox.cpp Account drop-down menu
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
#include "QGaimAccountBox.h"
#include "QGaimProtocolUtils.h"

#include <libgaim/debug.h>

#include <qpixmap.h>

QGaimAccountBox::QGaimAccountBox(bool showAll, QWidget *parent,
								 const char *name)
	: QComboBox(parent, name), showAll(showAll)
{
	if (showAll)
		buildMenu((GaimAccount *)gaim_accounts_get_all()->data);
	else
	{
		GaimConnection *gc;

		gc = (GaimConnection *)gaim_connections_get_all()->data;

		buildMenu(gaim_connection_get_account(gc));
	}
}

QGaimAccountBox::QGaimAccountBox(GaimAccount *account, bool showAll,
								 QWidget *parent, const char *name)
	: QComboBox(parent, name), showAll(showAll)
{
	buildMenu(account);
}

void
QGaimAccountBox::setCurrentAccount(GaimAccount *account)
{
	GaimAccount *tempAccount;
	GList *l, *list;
	int i;

	if (showAll)
		list = gaim_accounts_get_all();
	else
		list = gaim_connections_get_all();

	for (l = list, i = 0; l != NULL; l = l->next, i++)
	{
		if (showAll)
			tempAccount = (GaimAccount *)l->data;
		else
		{
			GaimConnection *gc = (GaimConnection *)l->data;

			tempAccount = gaim_connection_get_account(gc);
		}

		if (tempAccount == account)
		{
			setCurrentItem(i);
			break;
		}
	}
}

GaimAccount *
QGaimAccountBox::getCurrentAccount() const
{
	int index = currentItem();
	GList *l;

	if (showAll)
	{
		l = g_list_nth(gaim_accounts_get_all(), index);

		if (l == NULL)
			return NULL;

		return (GaimAccount *)l->data;
	}
	else
	{
		GaimConnection *gc;

		l = g_list_nth(gaim_connections_get_all(), index);

		if (l == NULL)
			return NULL;

		gc = (GaimConnection *)l->data;

		return gaim_connection_get_account(gc);
	}
}

void
QGaimAccountBox::buildMenu(GaimAccount *account)
{
	GList *l, *list;
	int count;

	if (showAll)
		list = gaim_accounts_get_all();
	else
		list = gaim_connections_get_all();

	for (l = list, count = 0; l != NULL; l = l->next, count++)
	{
		GaimAccount *tempAccount;

		if (showAll)
			tempAccount = (GaimAccount *)l->data;
		else
		{
			GaimConnection *gc = (GaimConnection *)l->data;

			tempAccount = gaim_connection_get_account(gc);
		}

		QPixmap *pixmap = QGaimProtocolUtils::getProtocolIcon(tempAccount);
		QString str;

		str = gaim_account_get_username(tempAccount);

		if (str.length() > 16)
		{
			str.truncate(16);
			str += "...";
		}

		if (pixmap == NULL)
			insertItem(str);
		else
		{
			insertItem(*pixmap, str);
			delete pixmap;
		}

		if (tempAccount == account)
			setCurrentItem(count);
	}
}
