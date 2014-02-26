/**
 * @file QQuailProtocolBox.cpp Protocol drop-down menu
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
#include "QuailProtocolBox.h"
#include "QuailProtocolUtils.h"

#include <libpurple/prpl.h>

quail_protocol_box::quail_protocol_box(QWidget *parent)
    : QComboBox(parent)
{
	GList *protocols;
	PurplePlugin *prpl;

	protocols = purple_plugins_get_protocols();

	if (protocols != NULL)
	{
		prpl = (PurplePlugin *)protocols->data;

		buildMenu(prpl->info->id);
	}
}

quail_protocol_box::quail_protocol_box(QString protocolId, QWidget *parent)
    : QComboBox(parent)
{
	buildMenu(protocolId);
}

void
quail_protocol_box::setCurrentProtocol(QString protocolId)
{
    for (int i = 0; i < this->count(); ++i )
    {
        if (protocolId == this->itemData(i).toString())
        {
            setCurrentIndex(i);
            return;
        }
    }
}

void
quail_protocol_box::buildMenu(QString protocolId)
{
    bool prpl_jabber_found = false;

    for (GList *p = purple_plugins_get_protocols();
		 p != NULL;
         p = p->next)
	{
		PurplePlugin *plugin;

		plugin = (PurplePlugin *)p->data;
        QString plugin_name = plugin->info->name;
        QString plugin_id = plugin->info->id;
        if (!prpl_jabber_found && plugin_name == "XMPP")
        {
            prpl_jabber_found = true;
        }

        addItem(quail_protocol_utils::getProtocolIcon(plugin)
                , plugin_name
                , plugin_id);

        if (protocolId != NULL && protocolId == plugin_id)
            setCurrentIndex(this->count()-1);
	}
    if (prpl_jabber_found)
    {
        addItem(quail_protocol_utils::getProtocolIcon("google-talk"), "Google Talk", "google-talk");
        if (protocolId == "google-talk")
        {
            setCurrentIndex(this->count()-1);
        }
        addItem(quail_protocol_utils::getProtocolIcon("facebook"), "Facebook", "facebook");
        if (protocolId == "facebook")
        {
            setCurrentIndex(this->count()-1);
        }
    }
    this->model()->sort(0);
}
