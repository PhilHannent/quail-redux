#include "QGaimAccountsWindow.h"
#include "QGaim.h"
#include "base.h"

#include <libgaim/prpl.h>

#include <qpe/qpetoolbar.h>
#include <qpe/resource.h>
#include <qaction.h>
#include <qlayout.h>
#include <qlistview.h>
#include <qmenubar.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>

/**************************************************************************
 * QGaimAccountListItem
 **************************************************************************/
QGaimAccountListItem::QGaimAccountListItem(QListView *parent, int index)
	: QListViewItem(parent), account(NULL), index(index)
{
}

QGaimAccountListItem::~QGaimAccountListItem()
{
}

void
QGaimAccountListItem::setAccount(GaimAccount *account)
{
	this->account = account;
}

GaimAccount *
QGaimAccountListItem::getAccount() const
{
	return account;
}

QString
QGaimAccountListItem::key(int, bool) const
{
	QString str;

	str.sprintf("%6d", index);

	return str;
}

/**************************************************************************
 * QGaimAccountsWindow
 **************************************************************************/
QGaimAccountsWindow::QGaimAccountsWindow()
	: QMainWindow()
{
	buildInterface();
	loadAccounts();
}

QGaimAccountsWindow::~QGaimAccountsWindow()
{
	delete accountsView;
}

void
QGaimAccountsWindow::buildInterface()
{
	setCaption(tr("Gaim - Accounts"));

	setupMenubar();
	setupToolbar();

	accountsView = new QListView(this, "AccountsView");
	accountsView->addColumn(tr("Screenname"), 160);
	accountsView->addColumn(tr("Protocol"), -1);
	accountsView->setAllColumnsShowFocus(true);

	connect(accountsView, SIGNAL(selectionChanged(QListViewItem *)),
			this, SLOT(accountSelected(QListViewItem *)));

	setCentralWidget(accountsView);
}

void
QGaimAccountsWindow::setupMenubar()
{
	QPixmap *pixmap;
	QAction *a;
	QToolButton *button;

	menubar = new QMenuBar(this);

	/* Build the Account menu */
	accountMenu = new QPopupMenu(this);
	menubar->insertItem(tr("&Account"), accountMenu);

	/* Account -> Add */
	a = new QAction(tr("&New Account..."), QString::null, 0, this, 0);
	a->addTo(accountMenu);

	accountMenu->insertSeparator();

	/* Buddy List */
	pixmap = new QPixmap(DATA_PREFIX "images/blist.png");
	button = new QToolButton(NULL, "blist");
	button->setAutoRaise(true);
	button->setPixmap(*pixmap);
	button->setOn(true);
	menubar->insertItem(button);
	delete pixmap;

	connect(button, SIGNAL(clicked()),
			this, SLOT(showBlist()));

	/* Accounts */
	pixmap = new QPixmap(DATA_PREFIX "images/accounts.png");
	accountsButton = button = new QToolButton(NULL, "accounts");
	button->setAutoRaise(true);
	button->setPixmap(*pixmap);
	button->setToggleButton(true);
	button->setOn(true);
	menubar->insertItem(button);
	delete pixmap;

	connect(button, SIGNAL(toggled(bool)),
			this, SLOT(accountsToggled(bool)));

	/* Conversations */
	pixmap = new QPixmap(DATA_PREFIX "images/edit.png");
	button = new QToolButton(NULL, "accounts");
	button->setAutoRaise(true);
	button->setPixmap(*pixmap);
	menubar->insertItem(button);
	delete pixmap;

	connect(button, SIGNAL(clicked()),
			this, SLOT(showConversations()));
}

void
QGaimAccountsWindow::setupToolbar()
{
	QAction *a;

	setToolBarsMovable(FALSE);

	QPEToolBar *toolbar = new QPEToolBar(this);
	toolbar->setHorizontalStretchable(FALSE);

	addToolBar(toolbar);

	/* Connect */
	a = new QAction(tr("Connect"),
					QIconSet(QPixmap(DATA_PREFIX "images/connect.png")),
					QString::null, 0, this, 0);
	connectButton = a;
	a->addTo(toolbar);

	a->setEnabled(false);
	connect(a, SIGNAL(activated()),
			this, SLOT(connectToAccount()));

	/* Disconnect */
	a = new QAction(tr("Connect"),
					QIconSet(QPixmap(DATA_PREFIX "images/disconnect.png")),
					QString::null, 0, this, 0);
	disconnectButton = a;
	a->addTo(toolbar);

	a->setEnabled(false);
	connect(a, SIGNAL(activated()),
			this, SLOT(disconnectFromAccount()));
}

void
QGaimAccountsWindow::loadAccounts()
{
	GList *l;
	int index;

	for (l = gaim_accounts_get_all(), index = 0;
		 l != NULL;
		 l = l->next, index++)
	{
		QPixmap *pixmap;
		QGaimAccountListItem *item;
		GaimAccount *account = (GaimAccount *)l->data;

		pixmap = getProtocolIcon(account);

		item = new QGaimAccountListItem(accountsView, index);
		item->setText(0, gaim_account_get_username(account));
		item->setText(1, getProtocolName(gaim_account_get_protocol(account)));
		item->setAccount(account);

		if (pixmap != NULL) {
			item->setPixmap(0, *pixmap);
			delete pixmap;
		}
	}
}

QString
QGaimAccountsWindow::getProtocolName(GaimProtocol protocol)
{
	GaimPlugin *p = gaim_find_prpl(protocol);

	return ((p != NULL && p->info->name != NULL)
			? tr(p->info->name)
			: tr("Unknown"));
}

void
QGaimAccountsWindow::connectToAccount()
{
	QGaimAccountListItem *item;
	
	item = (QGaimAccountListItem *)accountsView->selectedItem();

	gaim_account_connect(item->getAccount());
}

void
QGaimAccountsWindow::disconnectFromAccount()
{
	QGaimAccountListItem *item;
	
	item = (QGaimAccountListItem *)accountsView->selectedItem();

	gaim_account_disconnect(item->getAccount());
}

void
QGaimAccountsWindow::showBlist()
{
	qGaimGetHandle()->showBlistWindow();
}

void
QGaimAccountsWindow::accountsToggled(bool)
{
	accountsButton->setOn(true);
}

void
QGaimAccountsWindow::showConversations()
{
}

void
QGaimAccountsWindow::accountSelected(QListViewItem *item)
{
	QGaimAccountListItem *accountItem = (QGaimAccountListItem *)item;
	GaimAccount *account;

	account = accountItem->getAccount();

	connectButton->setEnabled(!gaim_account_is_connected(account));
	disconnectButton->setEnabled(gaim_account_is_connected(account));
}

QPixmap *
QGaimAccountsWindow::getProtocolIcon(GaimAccount *account)
{
	GaimPlugin *prpl;
	GaimPluginProtocolInfo *prpl_info = NULL;
	const char *protoname = NULL;
	QString path;

	prpl = gaim_find_prpl(gaim_account_get_protocol(account));

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
