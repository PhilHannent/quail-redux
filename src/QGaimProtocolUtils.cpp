/**
 * @file QGaimProtocolUtils.cpp Protocol utility functions
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
#include "QGaimProtocolUtils.h"
#include "base.h"

#include <qpe/resource.h>
#include <qdict.h>
#include <qobject.h>
#include <qpixmap.h>

static QDict<QPixmap> largePixmaps;
static QDict<QPixmap> smallPixmaps;
static QDict<QPixmap> menuPixmaps;

QString
QGaimProtocolUtils::getProtocolName(GaimProtocol protocol)
{
	GaimPlugin *p = gaim_find_prpl(protocol);

	return ((p != NULL && p->info->name != NULL)
			? QObject::tr(p->info->name)
			: QObject::tr("Unknown"));
}

QPixmap
QGaimProtocolUtils::getProtocolIcon(const GaimPlugin *prpl,
									QGaimPixmapSize size)
{
	GaimPluginProtocolInfo *prplInfo = NULL;

	if (prpl == NULL)
		return NULL;

	prplInfo = GAIM_PLUGIN_PROTOCOL_INFO(prpl);

	if (prplInfo->list_icon == NULL)
		return NULL;

	return getProtocolIcon(prplInfo->list_icon(NULL, NULL), size);
}

QPixmap
QGaimProtocolUtils::getProtocolIcon(GaimAccount *account,
									QGaimPixmapSize size)
{
	GaimPluginProtocolInfo *prplInfo = NULL;
	GaimPlugin *prpl;

	if (account == NULL)
		return NULL;

	prpl = gaim_find_prpl(gaim_account_get_protocol(account));

	if (prpl == NULL)
		return NULL;

	prplInfo = GAIM_PLUGIN_PROTOCOL_INFO(prpl);

	if (prplInfo->list_icon == NULL)
		return NULL;

	return getProtocolIcon(prplInfo->list_icon(account, NULL), size);
}

QPixmap
QGaimProtocolUtils::getProtocolIcon(const QString &protoName,
									QGaimPixmapSize size)
{
	QPixmap *pixmap;
	QDict<QPixmap> *pixmaps;

	if (protoName.isEmpty())
		return NULL;

	if (size == QGAIM_PIXMAP_LARGE)
		pixmaps = &largePixmaps;
	else if (size == QGAIM_PIXMAP_SMALL)
		pixmaps = &smallPixmaps;
	else if (size == QGAIM_PIXMAP_MENU)
		pixmaps = &menuPixmaps;

	pixmap = pixmaps->find(protoName);

	if (pixmap == NULL)
	{
		if (size == QGAIM_PIXMAP_LARGE)
		{
			pixmap = new QPixmap(Resource::loadPixmap("gaim/protocols/" +
													  protoName));
		}
		else
		{
			QImage image = Resource::loadImage("gaim/protocols/" + protoName);

			pixmap = new QPixmap();

			if (size == QGAIM_PIXMAP_SMALL)
				pixmap->convertFromImage(image.smoothScale(16, 16));
			else if (size == QGAIM_PIXMAP_MENU)
				pixmap->convertFromImage(image.smoothScale(14, 14));
		}

		pixmaps->insert(protoName, pixmap);
	}

	return *pixmap;
}
