#include "QGaimAccountEditor.h"
#include "QGaimProtocolBox.h"
#include "QGaimProtocolUtils.h"
#include "base.h"

#include <qcombobox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qtabwidget.h>
#include <qvbox.h>

QGaimAccountEditor::QGaimAccountEditor(GaimAccount *account)
	: QMainWindow(), account(account), plugin(NULL), prplInfo(NULL)
{
	GaimProtocol protocol;

	if (account == NULL)
	{
		protocolId = "prpl-oscar";
		protocol   = GAIM_PROTO_OSCAR;
	}
	else
	{
		protocolId = gaim_account_get_protocol_id(account);
		protocol   = gaim_account_get_protocol(account);
	}

	if ((plugin = gaim_find_prpl(protocol)) != NULL)
		prplInfo = GAIM_PLUGIN_PROTOCOL_INFO(plugin);

	buildInterface();
}

QGaimAccountEditor::~QGaimAccountEditor()
{
}

void
QGaimAccountEditor::buildInterface()
{
	if (account == NULL)
		setCaption(tr("Add Account"));
	else
		setCaption(tr("Edit Account"));

	tabs = new QTabWidget(this);
	tabs->setMargin(6);

	setCentralWidget(tabs);

	tabs->addTab(buildAccountTab(), "Account");

	if (plugin != NULL)
	{
		QPixmap *pixmap = QGaimProtocolUtils::getProtocolIcon(plugin);

		if (pixmap == NULL)
			tabs->addTab(buildProtocolTab(), plugin->info->name);
		else
		{
			tabs->addTab(buildProtocolTab(), QIconSet(*pixmap),
						 plugin->info->name);

			delete pixmap;
		}
	}

	tabs->addTab(buildProxyTab(), "Proxy");
}

QWidget *
QGaimAccountEditor::buildAccountTab()
{
	QVBox *vbox;
	QGroupBox *gbox;
	QLabel *label;
	QWidget *spacer;

	/* Create the main vbox. */
	vbox = new QVBox(this);

	/* Create the Login Options group box. */
//	gbox = new QGroupBox(2, Qt::Horizontal, tr("Login Options"), vbox);
	gbox = new QGroupBox(2, Qt::Horizontal, vbox);

	/* Protocol */
	label = new QLabel(tr("Protocol:"), gbox);

	protocolList = new QGaimProtocolBox(gbox, "protocol combo");

	/* Add a spacer. */
	spacer = new QLabel("", vbox);
	vbox->setStretchFactor(spacer, 1);

	return vbox;
}

QWidget *
QGaimAccountEditor::buildProtocolTab()
{
	return new QLabel("Protocol", this);
}

QWidget *
QGaimAccountEditor::buildProxyTab()
{
	return new QLabel("Proxy", this);
}

