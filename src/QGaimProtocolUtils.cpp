#include "QGaimProtocolUtils.h"
#include "base.h"

#include <qobject.h>
#include <qpixmap.h>

QString
QGaimProtocolUtils::getProtocolName(GaimProtocol protocol)
{
	GaimPlugin *p = gaim_find_prpl(protocol);

	return ((p != NULL && p->info->name != NULL)
			? QObject::tr(p->info->name)
			: QObject::tr("Unknown"));
}

QPixmap *
QGaimProtocolUtils::getProtocolIcon(const GaimPlugin *prpl)
{
	GaimPluginProtocolInfo *prplInfo = NULL;

	if (prpl == NULL)
		return NULL;

	prplInfo = GAIM_PLUGIN_PROTOCOL_INFO(prpl);

	if (prplInfo->list_icon == NULL)
		return NULL;

	return getProtocolIcon(prplInfo->list_icon(NULL, NULL));
}

QPixmap *
QGaimProtocolUtils::getProtocolIcon(GaimAccount *account)
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

	return getProtocolIcon(prplInfo->list_icon(account, NULL));
}

QPixmap *
QGaimProtocolUtils::getProtocolIcon(const QString &protoName)
{
	QString path;
	QPixmap *pixmap;

	if (protoName.isEmpty())
		return NULL;

	path  = DATA_PREFIX "images/protocols/small/";
	path += protoName;
	path += ".png";

	pixmap = new QPixmap();

	if (!pixmap->load(path))
	{
		delete pixmap;
		return NULL;
	}

	return pixmap;
}
