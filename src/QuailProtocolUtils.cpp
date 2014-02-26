/**
 * @file QQuailProtocolUtils.cpp Protocol utility functions
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
#include "QuailProtocolUtils.h"

#include <QMap>
#include <QObject>
#include <QPixmap>

QString
quail_protocol_utils::getProtocolName(QString protocolId)
{
    PurplePlugin *p = purple_plugins_find_with_id(protocolId.toStdString().c_str());

	return ((p != NULL && p->info->name != NULL)
			? QObject::tr(p->info->name)
			: QObject::tr("Unknown"));
}

QPixmap
quail_protocol_utils::getProtocolIcon(const PurplePlugin *prpl)
{
	PurplePluginProtocolInfo *prplInfo = NULL;

	if (prpl == NULL)
        return QPixmap();

    prplInfo = PURPLE_PLUGIN_PROTOCOL_INFO(prpl);

	if (prplInfo->list_icon == NULL)
        return QPixmap();

    return getProtocolIcon(prplInfo->list_icon(NULL, NULL));
}

QPixmap
quail_protocol_utils::getProtocolIcon(PurpleAccount *account)
{
	PurplePluginProtocolInfo *prplInfo = NULL;
	PurplePlugin *prpl;

	if (account == NULL)
        return QPixmap();

	prpl = purple_plugins_find_with_id(purple_account_get_protocol_id(account));

	if (prpl == NULL)
        return QPixmap();

    prplInfo = PURPLE_PLUGIN_PROTOCOL_INFO(prpl);

	if (prplInfo->list_icon == NULL)
        return QPixmap();

    return getProtocolIcon(prplInfo->list_icon(account, NULL));
}

QString
quail_protocol_utils::getProtocolIconName(PurpleAccount *account)
{
    PurplePluginProtocolInfo *prplInfo = NULL;
    PurplePlugin *prpl;

    if (account == NULL)
        return QString("");

    prpl = purple_plugins_find_with_id(purple_account_get_protocol_id(account));

    if (prpl == NULL)
        return QString("");

    prplInfo = PURPLE_PLUGIN_PROTOCOL_INFO(prpl);

    if (prplInfo->list_icon == NULL)
        return QString("");

    return getProtocolIconName(prplInfo->list_icon(account, NULL));
}


QPixmap
quail_protocol_utils::getProtocolIcon(const QString &protoName)
{
    return QPixmap(getProtocolIconName(protoName));
}

QString
quail_protocol_utils::getProtocolIconName(const QString &protoName)
{
    return QString(":/data/images/protocols/" + protoName + ".svg");
}
