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
QGaimAccountListItem::QGaimAccountListItem(QListView *parent)
	: QListViewItem(parent), account(NULL)
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
	a = new QAction(tr("Connect"), Resource::loadPixmap("enter"),
					QString::null, 0, this, 0);
	a->addTo(toolbar);

	connect(a, SIGNAL(activated()),
			this, SLOT(connectToAccount()));
}

void
QGaimAccountsWindow::loadAccounts()
{
	for (GList *l = gaim_accounts_get_all(); l != NULL; l = l->next)
	{
		QGaimAccountListItem *item;
		GaimAccount *account = (GaimAccount *)l->data;

		item = new QGaimAccountListItem(accountsView);
		item->setText(0, gaim_account_get_username(account));
		item->setText(1, getProtocolName(gaim_account_get_protocol(account)));
		item->setAccount(account);
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
