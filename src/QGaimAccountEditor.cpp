#include "QGaimAccountEditor.h"
#include "base.h"

#include <libgaim/prpl.h>

#include <qcombobox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qvbox.h>

QGaimAccountEditor::QGaimAccountEditor(GaimAccount *account)
	: QMainWindow(), account(account)
{
	buildInterface();
}

QGaimAccountEditor::~QGaimAccountEditor()
{
}

void
QGaimAccountEditor::buildInterface()
{
	QVBox *vbox;
	QGroupBox *gbox;
	QLabel *label;

	if (account == NULL)
		setCaption(tr("Add Account"));
	else
		setCaption(tr("Edit Account"));

	/* Create the main vbox. */
	vbox = new QVBox(this);

	/* Create the Login Options group box. */
	gbox = new QGroupBox(2, Qt::Horizontal, tr("Login Options"), vbox);

	/* Protocol */
	label = new QLabel(tr("Protocol:"), gbox);

	protocolList = new QComboBox(gbox, "protocol combo");

	/* Fill the protocol drop-down. */
	GList *p;

	for (p = gaim_plugins_get_protocols(); p != NULL; p = p->next)
	{
		GaimPlugin *plugin;
		GaimPluginProtocolInfo *prpl_info;

		plugin = (GaimPlugin *)p->data;
		prpl_info = GAIM_PLUGIN_PROTOCOL_INFO(plugin);

		QPixmap *pixmap = getProtocolIcon(plugin);

		if (pixmap == NULL)
			protocolList->insertItem(plugin->info->name);
		else
		{
			protocolList->insertItem(*pixmap, plugin->info->name);
			delete pixmap;
		}
	}

	setCentralWidget(vbox);
}

QPixmap *
QGaimAccountEditor::getProtocolIcon(GaimPlugin *prpl) const
{
	GaimPluginProtocolInfo *prpl_info = NULL;
	const char *protoname = NULL;
	QString path;

	if (prpl != NULL)
	{
		prpl_info = GAIM_PLUGIN_PROTOCOL_INFO(prpl);

		if (prpl_info->list_icon != NULL)
			protoname = prpl_info->list_icon(account, NULL);
	}

	if (protoname == NULL)
		return NULL;

	path  = DATA_PREFIX "images/protocols/small/";
	path += protoname;
	path += ".png";

	QPixmap *pixmap = new QPixmap();

	if (!pixmap->load(path))
	{
		delete pixmap;
		return NULL;
	}

	return pixmap;
}
