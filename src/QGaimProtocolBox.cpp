/**
 * @file QGaimProtocolBox.cpp Protocol drop-down menu
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
#include "QGaimProtocolBox.h"
#include "QGaimProtocolUtils.h"

#include <libgaim/debug.h>

#include <qpixmap.h>

QGaimProtocolBox::QGaimProtocolBox(QWidget *parent, const char *name)
	: QComboBox(parent, name)
{
	buildMenu(GAIM_PROTO_OSCAR);
}

QGaimProtocolBox::QGaimProtocolBox(GaimProtocol protocol, QWidget *parent,
								   const char *name)
	: QComboBox(parent, name)
{
	buildMenu(protocol);
}

void
QGaimProtocolBox::setCurrentProtocol(GaimProtocol protocol)
{
	GaimPlugin *plugin;
	GaimPluginProtocolInfo *prpl_info;
	GList *p;
	int i;

	for (p = gaim_plugins_get_protocols(), i = 0;
		 p != NULL;
		 p = p->next, i++)
	{
		plugin = (GaimPlugin *)p->data;
		prpl_info = GAIM_PLUGIN_PROTOCOL_INFO(plugin);

		if (prpl_info->protocol == protocol)
		{
			setCurrentItem(i);
			break;
		}
	}
}

void
QGaimProtocolBox::buildMenu(GaimProtocol protocol)
{
	GList *p;
	int count;

	for (p = gaim_plugins_get_protocols(), count = 0;
		 p != NULL;
		 p = p->next, count++)
	{
		GaimPlugin *plugin;
		GaimPluginProtocolInfo *prpl_info;

		plugin = (GaimPlugin *)p->data;
		prpl_info = GAIM_PLUGIN_PROTOCOL_INFO(plugin);

		QPixmap *pixmap = QGaimProtocolUtils::getProtocolIcon(plugin);

		if (pixmap == NULL)
			insertItem(plugin->info->name);
		else
		{
			insertItem(*pixmap, plugin->info->name);
			delete pixmap;
		}

		if (protocol == prpl_info->protocol)
			setCurrentItem(count);
	}
}
