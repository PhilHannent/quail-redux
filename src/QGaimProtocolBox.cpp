#include "QGaimProtocolBox.h"
#include "QGaimProtocolUtils.h"

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
