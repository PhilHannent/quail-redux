/**
 * @file QGaimProtocolBox.cpp Protocol drop-down menu
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
#include "QuailProtocolBox.h"
#include "QuailProtocolUtils.h"

#include <libgaim/debug.h>

#include <qpixmap.h>

QGaimProtocolBox::QGaimProtocolBox(QWidget *parent, const char *name)
	: QComboBox(parent, name)
{
	GList *protocols;
	GaimPlugin *prpl;

	protocols = gaim_plugins_get_protocols();

	if (protocols != NULL)
	{
		prpl = (GaimPlugin *)protocols->data;

		buildMenu(prpl->info->id);
	}
}

QGaimProtocolBox::QGaimProtocolBox(QString protocolId, QWidget *parent,
								   const char *name)
	: QComboBox(parent, name)
{
	buildMenu(protocolId);
}

void
QGaimProtocolBox::setCurrentProtocol(QString protocolId)
{
	GaimPlugin *plugin;
	GList *p;
	int i;

	for (p = gaim_plugins_get_protocols(), i = 0;
		 p != NULL;
		 p = p->next, i++)
	{
		plugin = (GaimPlugin *)p->data;

		if (plugin->info->id == protocolId)
		{
			setCurrentItem(i);
			break;
		}
	}
}

void
QGaimProtocolBox::buildMenu(QString protocolId)
{
	GList *p;
	int count;

	for (p = gaim_plugins_get_protocols(), count = 0;
		 p != NULL;
		 p = p->next, count++)
	{
		GaimPlugin *plugin;

		plugin = (GaimPlugin *)p->data;

		insertItem(
			QGaimProtocolUtils::getProtocolIcon(plugin, QGAIM_PIXMAP_MENU),
			plugin->info->name);

		if (protocolId != NULL && protocolId == plugin->info->id)
			setCurrentItem(count);
	}
}
