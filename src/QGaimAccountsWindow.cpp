/**
 * @file QGaimAccountsWindow.cpp Accounts window
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
#include "QGaimAccountsWindow.h"
#include "QGaimAccountEditor.h"
#include "QGaimConnectionMeter.h"
#include "QGaimConvButton.h"
#include "QGaimProtocolUtils.h"
#include "QGaim.h"
#include "base.h"

#include <libgaim/prpl.h>

#include <qpe/resource.h>
#include <qaction.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlistview.h>
#include <qmenubar.h>
#include <qpushbutton.h>
#include <qtoolbar.h>
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

QGaimConnectionMeters *
QGaimAccountsWindow::getMeters() const
{
	return meters;
}

void
QGaimAccountsWindow::updateAccounts()
{
	loadAccounts();
}

void
QGaimAccountsWindow::buildInterface()
{
	QVBox *vbox;

	setCaption(tr("Gaim - Accounts"));

	setupToolbar();

	/* Create the main vbox */
	vbox = new QVBox(this);

	/* Create the accounts view */
	accountsView = new QListView(vbox, "AccountsView");
	accountsView->addColumn(tr("Screenname"), 160);
	accountsView->addColumn(tr("Protocol"), -1);
	accountsView->setAllColumnsShowFocus(true);

	connect(accountsView, SIGNAL(selectionChanged(QListViewItem *)),
			this, SLOT(accountSelected(QListViewItem *)));

	/* Create the connection meters box. */
	meters = new QGaimConnectionMeters(vbox);

	setCentralWidget(vbox);
}


void
QGaimAccountsWindow::setupToolbar()
{
	QAction *a;
	QLabel *label;
	QToolButton *button;

	setToolBarsMovable(false);

	toolbar = new QToolBar(this);
	toolbar->setHorizontalStretchable(true);

	/* New */
	a = new QAction(tr("New Account"),
					QIconSet(Resource::loadPixmap("gaim/new.png")),
					QString::null, 0, this, 0);
	a->addTo(toolbar);

	connect(a, SIGNAL(activated()),
			this, SLOT(newAccount()));

	/* Edit */
	a = new QAction(tr("Edit Account"),
					QIconSet(Resource::loadPixmap("gaim/edit.png")),
					QString::null, 0, this, 0);
	editButton = a;
	a->addTo(toolbar);
	a->setEnabled(false);

	connect(a, SIGNAL(activated()),
			this, SLOT(editAccount()));

	/* Delete */
	a = new QAction(tr("Delete"),
					QIconSet(Resource::loadPixmap("gaim/delete.png")),
					QString::null, 0, this, 0);
	deleteButton = a;
	a->addTo(toolbar);
	a->setEnabled(false);

	connect(a, SIGNAL(activated()),
			this, SLOT(deleteAccount()));

	/* Separator */
	toolbar->addSeparator();

	/* Connect */
	a = new QAction(tr("Connect"),
					QIconSet(Resource::loadPixmap("gaim/connect.png")),
					QString::null, 0, this, 0);
	connectButton = a;
	a->addTo(toolbar);

	a->setEnabled(false);
	connect(a, SIGNAL(activated()),
			this, SLOT(connectToAccount()));

	/* Disconnect */
	a = new QAction(tr("Connect"),
					QIconSet(Resource::loadPixmap("gaim/disconnect.png")),
					QString::null, 0, this, 0);
	disconnectButton = a;
	a->addTo(toolbar);

	a->setEnabled(false);
	connect(a, SIGNAL(activated()),
			this, SLOT(disconnectFromAccount()));


	/* Add some whitespace. */
	label = new QLabel(toolbar);
	label->setText("");
	toolbar->setStretchableWidget(label);

	/* Now we're going to construct the toolbar on the right. */
	toolbar->addSeparator();

	/* Buddy List */
	button = new QToolButton(toolbar, "blist");
	button->setAutoRaise(true);
	button->setPixmap(Resource::loadPixmap("gaim/blist.png"));
	button->setOn(true);

	connect(button, SIGNAL(clicked()),
			this, SLOT(showBlist()));

	/* Accounts */
	accountsButton = button = new QToolButton(toolbar, "accounts");
	button->setAutoRaise(true);
	button->setPixmap(Resource::loadPixmap("gaim/accounts.png"));
	button->setToggleButton(true);
	button->setOn(true);

	connect(button, SIGNAL(toggled(bool)),
			this, SLOT(accountsToggled(bool)));

	/* Conversations */
	button = new QGaimConvButton(toolbar, "conversations");
}

void
QGaimAccountsWindow::loadAccounts()
{
	GList *l;
	int index;

	accountsView->clear();

	for (l = gaim_accounts_get_all(), index = 0;
		 l != NULL;
		 l = l->next, index++)
	{
		QPixmap *pixmap;
		QGaimAccountListItem *item;
		GaimAccount *account = (GaimAccount *)l->data;
		GaimProtocol protocol = gaim_account_get_protocol(account);

		pixmap = QGaimProtocolUtils::getProtocolIcon(account);

		item = new QGaimAccountListItem(accountsView, index);
		item->setText(0, gaim_account_get_username(account));
		item->setText(1, QGaimProtocolUtils::getProtocolName(protocol));
		item->setAccount(account);

		if (pixmap != NULL) {
			item->setPixmap(0, *pixmap);
			delete pixmap;
		}
	}
}

void
QGaimAccountsWindow::newAccount()
{
	QGaimAccountEditor *editor;

	editor = new QGaimAccountEditor(NULL, this);
	editor->setAccountsWindow(this);

	editor->showMaximized();
}

void
QGaimAccountsWindow::editAccount()
{
	QGaimAccountEditor *editor;
	QGaimAccountListItem *item;

	item = (QGaimAccountListItem *)accountsView->selectedItem();

	editor = new QGaimAccountEditor(item->getAccount(), this);
	editor->setAccountsWindow(this);

	editor->showMaximized();
}

void
QGaimAccountsWindow::deleteAccount()
{
	QGaimAccountListItem *item;

	item = (QGaimAccountListItem *)accountsView->selectedItem();

	gaim_accounts_remove(item->getAccount());

	delete item;

	connectButton->setEnabled(false);
	disconnectButton->setEnabled(false);
	editButton->setEnabled(false);
	deleteButton->setEnabled(false);
}

void
QGaimAccountsWindow::connectToAccount()
{
	QGaimAccountListItem *item;

	connectButton->setEnabled(false);

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
QGaimAccountsWindow::accountSelected(QListViewItem *item)
{
	QGaimAccountListItem *accountItem = (QGaimAccountListItem *)item;
	GaimAccount *account;

	account = accountItem->getAccount();

	if (gaim_find_prpl(gaim_account_get_protocol(account)) == NULL)
	{
		connectButton->setEnabled(false);
		disconnectButton->setEnabled(false);
	}
	else
	{
		connectButton->setEnabled(!gaim_account_is_connected(account));
		disconnectButton->setEnabled(gaim_account_is_connected(account));
	}

	editButton->setEnabled(true);
	deleteButton->setEnabled(true);
}

static void
qGaimConnConnectProgress(GaimConnection *gc, const char *text,
						 size_t step, size_t step_count)
{
	QGaim *gaim = qGaimGetHandle();
	QGaimAccountsWindow *accountsWin;
	QGaimConnectionMeters *meters;
	QGaimConnectionMeter *meter;

	accountsWin = gaim->getAccountsWindow();

	if (accountsWin == NULL)
	{
		qGaimGetHandle()->showAccountsWindow();
		accountsWin = gaim->getAccountsWindow();
	}

	meters = accountsWin->getMeters();
	meter = meters->findMeter(gc);

	if (meter == NULL)
		meter = meters->addConnection(gc);

	meter->update(QString(text), step, step_count);
}

static void
qGaimConnConnected(GaimConnection *gc)
{
	QGaim *gaim = qGaimGetHandle();
	QGaimAccountsWindow *accountsWin = gaim->getAccountsWindow();
	QGaimConnectionMeters *meters = accountsWin->getMeters();
	QGaimConnectionMeter *meter;

	meter = meters->findMeter(gc);

	if (meter != NULL)
		meters->removeMeter(meter);
}

static void
qGaimConnDisconnected(GaimConnection *gc, const char *reason)
{
	QGaim *gaim = qGaimGetHandle();
	QGaimAccountsWindow *accountsWin = gaim->getAccountsWindow();
	QGaimConnectionMeters *meters = accountsWin->getMeters();
	QGaimConnectionMeter *meter;

	meter = meters->findMeter(gc);

	if (meter != NULL)
		meters->removeMeter(meter);

	/* XXX */
	reason = NULL;
}

static void
qGaimConnNotice(GaimConnection *gc, const char *text)
{
	/* XXX */
	gc = NULL;
	text = NULL;
}

static GaimConnectionUiOps connUiOps =
{
	qGaimConnConnectProgress,
	qGaimConnConnected,
	qGaimConnDisconnected,
	qGaimConnNotice
};

GaimConnectionUiOps *
qGaimGetConnectionUiOps()
{
	return &connUiOps;
}
