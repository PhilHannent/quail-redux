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

#include <libpurple/debug.h>

#include <QDebug>
#include <QPixmap>

QQuailProtocolBox::QQuailProtocolBox(QWidget *parent)
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

QQuailProtocolBox::QQuailProtocolBox(QString protocolId, QWidget *parent)
    : QComboBox(parent)
{
	buildMenu(protocolId);
}

void
QQuailProtocolBox::setCurrentProtocol(QString protocolId)
{
    qDebug() << "QQuailProtocolBox::setCurrentProtocol" << protocolId;
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
QQuailProtocolBox::buildMenu(QString protocolId)
{
    qDebug() << "QQuailProtocolBox::buildMenu:protocolId:" << protocolId;
	GList *p;
	int count;
    bool prpl_jabber_found = false;

	for (p = purple_plugins_get_protocols(), count = 0;
		 p != NULL;
		 p = p->next, count++)
	{
		PurplePlugin *plugin;

		plugin = (PurplePlugin *)p->data;
        QString plugin_name = plugin->info->name;
        QString plugin_id = plugin->info->id;
        if (!prpl_jabber_found && plugin_name == "XMPP")
        {
            prpl_jabber_found = true;
        }

        qDebug() << "QQuailProtocolBox::buildMenu:plugin_name:" << plugin_name;
        qDebug() << "QQuailProtocolBox::buildMenu:plugin_id:" << plugin_id;
        addItem(QQuailProtocolUtils::getProtocolIcon(plugin)
                , plugin_name
                , plugin_id);

        if (protocolId != NULL && protocolId == plugin_id)
            setCurrentIndex(count);
	}
    if (prpl_jabber_found)
    {
        addItem(QQuailProtocolUtils::getProtocolIcon("google-talk"), "Google Talk", "google-talk");
        ++count;
        if (protocolId == "google-talk")
        {
            qDebug() << "QQuailProtocolBox::buildMenu:googleTalk set:" << protocolId;
            setCurrentIndex(count);
        }
        addItem(QQuailProtocolUtils::getProtocolIcon("facebook"), "Facebook", "facebook");
        ++count;
        if (protocolId == "facebook")
        {
            qDebug() << "QQuailProtocolBox::buildMenu:facebook set:" << protocolId;
            setCurrentIndex(count);
        }
    }
    this->model()->sort(0);
}
