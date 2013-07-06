/**
 * @file QQuailAccountsWindow.cpp Accounts window
 *
 * @Copyright (C) 2003-2004 Christian Hammond.
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
#include "QuailAccountsWindow.h"
#include "QuailAccountEditor.h"
#include "QuailConnectionMeter.h"
#include "QuailConvButton.h"
#include "QuailImageUtils.h"
#include "QuailProtocolUtils.h"
#include "QuailMainWindow.h"
#include "base.h"

#include <libpurple/prpl.h>
#include <libpurple/signals.h>

//TODO: Add back the resource file
//#include <qpe/resource.h>
#include <QAction>
#include <QLabel>
#include <QLayout>
#include <QListView>
#include <QMenuBar>
#include <QPushButton>
#include <QTimer>
#include <QToolbar>
#include <QToolButton>

/**************************************************************************
 * QQuailAccountListItem
 **************************************************************************/
QQuailAccountListItem::QQuailAccountListItem(QListView *parent, int index)
	: QListViewItem(parent), account(NULL), index(index), pulseTimer(NULL)
{
}

QQuailAccountListItem::~QQuailAccountListItem()
{
	stopPulse();
}

void
QQuailAccountListItem::startPulse(QPixmap onlinePixmap)
{
	stopPulse();

	pulseGrey = true;
	pulseStep = 0;

	pulseOrigPixmap = new QPixmap(onlinePixmap);
	pulseTimer = new QTimer(this);

	connect(pulseTimer, SIGNAL(timeout()),
			this, SLOT(updatePulse()));

	pulseTimer->start(100, false);
}

void
QQuailAccountListItem::stopPulse()
{
	if (pulseTimer == NULL)
		return;

	delete pulseTimer;
	delete pulseOrigPixmap;

	pulseTimer      = NULL;
	pulseOrigPixmap = NULL;
}

void
QQuailAccountListItem::setAccount(PurpleAccount *account)
{
	this->account = account;
}

PurpleAccount *
QQuailAccountListItem::getAccount() const
{
	return account;
}

QString
QQuailAccountListItem::key(int, bool) const
{
	QString str;

	str.sprintf("%6d", index);

	return str;
}

void
QQuailAccountListItem::updatePulse()
{
	QPixmap tempPixmap(*pulseOrigPixmap);

	setPixmap(0, QQuailImageUtils::saturate(tempPixmap, pulseStep));

	if (pulseGrey)
		pulseStep += 0.20;
	else
		pulseStep -= 0.20;

	pulseGrey = (pulseStep <= 0);
}

/**************************************************************************
 * QQuailAccountsWindow gaim callbacks
 **************************************************************************/
static void
signedOnCb(PurpleConnection *gc, QQuailAccountsWindow *win)
{
	win->accountSignedOn(purple_connection_get_account(gc));
}

static void
signedOffCb(PurpleConnection *gc, QQuailAccountsWindow *win)
{
	win->accountSignedOff(purple_connection_get_account(gc));
}

/**************************************************************************
 * QQuailAccountsWindow
 **************************************************************************/
QQuailAccountsWindow::QQuailAccountsWindow(QMainWindow *parent)
	: QMainWindow(), parentMainWindow(parent)
{
	connect(parent, SIGNAL(pixmapSizeChanged(bool)),
			this, SLOT(setUsesBigPixmaps(bool)));
	setUsesBigPixmaps(parent->usesBigPixmaps());

	buildInterface();

	purple_signal_connect(purple_connections_get_handle(), "signed-on",
						this, GAIM_CALLBACK(signedOnCb), this);
	purple_signal_connect(purple_connections_get_handle(), "signed-off",
						this, GAIM_CALLBACK(signedOffCb), this);

	loadAccounts();
}

QQuailAccountsWindow::~QQuailAccountsWindow()
{
	delete accountsView;
}

void
QQuailAccountsWindow::updateAccounts()
{
	loadAccounts();
}

void
QQuailAccountsWindow::accountSignedOn(PurpleAccount *account)
{
	QQuailAccountListItem *item;

	item = (QQuailAccountListItem *)accountsView->selectedItem();

	if (item->getAccount() == account)
	{
		connectButton->setEnabled(false);
		disconnectButton->setEnabled(true);
		deleteButton->setEnabled(false);
	}
	else
		item = QQuailAccountsWindow::getItemFromAccount(account);

	if (item != NULL)
	{
		item->stopPulse();
		item->setPixmap(0, QQuailProtocolUtils::getProtocolIcon(account));
	}
}

void
QQuailAccountsWindow::accountSignedOff(PurpleAccount *account)
{
	QQuailAccountListItem *item;

	item = (QQuailAccountListItem *)accountsView->selectedItem();

	if (item->getAccount() == account)
	{
		connectButton->setEnabled(true);
		disconnectButton->setEnabled(false);
		deleteButton->setEnabled(true);
	}
	else
		item = QQuailAccountsWindow::getItemFromAccount(account);

	if (item != NULL)
	{
		QPixmap protocolIcon = QQuailProtocolUtils::getProtocolIcon(account);

		item->setPixmap(0, QQuailImageUtils::greyPixmap(protocolIcon));
	}
}

void
QQuailAccountsWindow::buildInterface()
{
	setupToolbar();

	/* Create the accounts view */
	accountsView = new QListView(this, "AccountsView");
	accountsView->addColumn(tr("Screenname"));
	accountsView->addColumn(tr("Protocol"));
	accountsView->setAllColumnsShowFocus(true);
	accountsView->setColumnWidthMode(0, QListView::Manual);
	accountsView->setColumnWidthMode(1, QListView::Manual);

	connect(accountsView, SIGNAL(selectionChanged(QListViewItem *)),
			this, SLOT(accountSelected(QListViewItem *)));

	setCentralWidget(accountsView);
}

void
QQuailAccountsWindow::setupToolbar()
{
	QAction *a;
	QLabel *label;
	QToolButton *button;

	setToolBarsMovable(false);

	toolbar = new QToolBar(this);
	toolbar->setHorizontalStretchable(true);

	/* New */
	a = new QAction(tr("New Account"),
					QIconSet(Resource::loadPixmap("gaim/16x16/new"),
							 Resource::loadPixmap("gaim/32x32/new")),
					QString::null, 0, this, 0);
	a->addTo(toolbar);

	connect(a, SIGNAL(activated()),
			this, SLOT(newAccount()));

	/* Edit */
	a = new QAction(tr("Edit Account"),
					QIconSet(Resource::loadPixmap("gaim/16x16/edit"),
							 Resource::loadPixmap("gaim/32x32/edit")),
					QString::null, 0, this, 0);
	editButton = a;
	a->addTo(toolbar);
	a->setEnabled(false);

	connect(a, SIGNAL(activated()),
			this, SLOT(editAccount()));

	/* Delete */
	a = new QAction(tr("Delete"),
					QIconSet(Resource::loadPixmap("gaim/16x16/delete"),
							 Resource::loadPixmap("gaim/32x32/delete")),
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
					QIconSet(Resource::loadPixmap("gaim/16x16/connect"),
							 Resource::loadPixmap("gaim/32x32/connect")),
					QString::null, 0, this, 0);
	connectButton = a;
	a->addTo(toolbar);

	a->setEnabled(false);
	connect(a, SIGNAL(activated()),
			this, SLOT(connectToAccount()));

	/* Disconnect */
	a = new QAction(tr("Connect"),
					QIconSet(Resource::loadPixmap("gaim/16x16/disconnect"),
							 Resource::loadPixmap("gaim/32x32/disconnect")),
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
	a = new QAction(tr("Buddy List"),
					QIconSet(Resource::loadPixmap("gaim/16x16/blist"),
							 Resource::loadPixmap("gaim/32x32/blist")),
					QString::null, 0, this, 0);
	a->addTo(toolbar);

	connect(a, SIGNAL(activated()),
			this, SLOT(showBlist()));

	/* Accounts */
	a = new QAction(tr("Accounts"),
					QIconSet(Resource::loadPixmap("gaim/16x16/accounts"),
							 Resource::loadPixmap("gaim/32x32/accounts")),
					QString::null, 0, this, 0, true);
	a->setOn(true);
	accountsButton = a;
	a->addTo(toolbar);

	connect(a, SIGNAL(toggled(bool)),
			this, SLOT(accountsToggled(bool)));

	/* Conversations */
	button = new QQuailConvButton(toolbar, "conversations");
}

void
QQuailAccountsWindow::loadAccounts()
{
	GList *l;
	int index;

	accountsView->clear();

	for (l = purple_accounts_get_all(), index = 0;
		 l != NULL;
		 l = l->next, index++)
	{
		QPixmap protocolIcon;
		QQuailAccountListItem *item;
		PurpleAccount *account = (PurpleAccount *)l->data;
		QString protocolId = purple_account_get_protocol_id(account);

		protocolIcon = QQuailProtocolUtils::getProtocolIcon(account);

		item = new QQuailAccountListItem(accountsView, index);
		item->setText(0, purple_account_get_username(account));
		item->setText(1, QQuailProtocolUtils::getProtocolName(protocolId));
		item->setAccount(account);

		if (purple_account_is_connected(account))
			item->setPixmap(0, protocolIcon);
		else
			item->setPixmap(0, QQuailImageUtils::greyPixmap(protocolIcon));
	}
}

void
QQuailAccountsWindow::newAccount()
{
	QQuailAccountEditor *editor;

	editor = new QQuailAccountEditor(NULL, this, "", true);
	editor->setAccountsWindow(this);

	editor->showMaximized();
}

void
QQuailAccountsWindow::editAccount()
{
	QQuailAccountEditor *editor;
	QQuailAccountListItem *item;

	item = (QQuailAccountListItem *)accountsView->selectedItem();

	editor = new QQuailAccountEditor(item->getAccount(), this, "", true);
	editor->setAccountsWindow(this);

	editor->showMaximized();
}

void
QQuailAccountsWindow::deleteAccount()
{
	QQuailAccountListItem *item;

	item = (QQuailAccountListItem *)accountsView->selectedItem();

	purple_accounts_remove(item->getAccount());

	delete item;

	connectButton->setEnabled(false);
	disconnectButton->setEnabled(false);
	editButton->setEnabled(false);
	deleteButton->setEnabled(false);
}

void
QQuailAccountsWindow::connectToAccount()
{
	QQuailAccountListItem *item;

	connectButton->setEnabled(false);

	item = (QQuailAccountListItem *)accountsView->selectedItem();

	item->startPulse(QQuailProtocolUtils::getProtocolIcon(item->getAccount()));

	purple_account_connect(item->getAccount());
}

void
QQuailAccountsWindow::disconnectFromAccount()
{
	QQuailAccountListItem *item;

	item = (QQuailAccountListItem *)accountsView->selectedItem();

	purple_account_disconnect(item->getAccount());
}

void
QQuailAccountsWindow::showBlist()
{
	qQuailGetMainWindow()->showBlistWindow();
}

void
QQuailAccountsWindow::accountsToggled(bool)
{
	accountsButton->setOn(true);
}

void
QQuailAccountsWindow::accountSelected(QListViewItem *item)
{
	QQuailAccountListItem *accountItem = (QQuailAccountListItem *)item;
	PurpleAccount *account;
	const char *protocolId;

	account    = accountItem->getAccount();
	protocolId = purple_account_get_protocol_id(account);

	if (purple_plugins_find_with_id(protocolId) == NULL)
	{
		connectButton->setEnabled(false);
		disconnectButton->setEnabled(false);
	}
	else
	{
		connectButton->setEnabled(!purple_account_is_connected(account));
		disconnectButton->setEnabled(purple_account_is_connected(account));
	}

	editButton->setEnabled(true);
	deleteButton->setEnabled(true);
}

void
QQuailAccountsWindow::resizeEvent(QResizeEvent *)
{
	accountsView->setColumnWidth(1, (accountsView->width()) / 4);
	accountsView->setColumnWidth(0, accountsView->width() - 20 -
								 accountsView->columnWidth(1));
}

QQuailAccountListItem *
QQuailAccountsWindow::getItemFromAccount(PurpleAccount *account)
{
	QListViewItem *item;
	QQuailAccountListItem *gitem;

	if (account == NULL)
		return NULL;

	for (item = accountsView->firstChild();
		 item != NULL;
		 item = item->nextSibling())
	{
		gitem = (QQuailAccountListItem *)item;

		if (gitem->getAccount() == account)
			return gitem;
	}

	return NULL;
}

static void
qQuailConnConnectProgress(PurpleConnection *gc, const char *text,
						 size_t step, size_t step_count)
{
	QQuailConnectionMeters *meters;
	QQuailConnectionMeter *meter;

	meters = qQuailGetMainWindow()->getMeters();
	meter = meters->findMeter(gc);

	if (meter == NULL)
		meter = meters->addConnection(gc);

	meter->update(QString(text), step, step_count);
}

static void
qQuailConnConnected(PurpleConnection *gc)
{
	QQuailConnectionMeters *meters = qQuailGetMainWindow()->getMeters();
	QQuailConnectionMeter *meter;

	meter = meters->findMeter(gc);

	if (meter != NULL)
		meters->removeMeter(meter);
}

static void
qQuailConnDisconnected(PurpleConnection *gc)
{
	QQuailConnectionMeters *meters = qQuailGetMainWindow()->getMeters();
	QQuailConnectionMeter *meter;

	meter = meters->findMeter(gc);

	if (meter != NULL)
		meters->removeMeter(meter);
}

static void
qQuailConnNotice(PurpleConnection *gc, const char *text)
{
	/* XXX */
	gc = NULL;
	text = NULL;
}

static PurpleConnectionUiOps connUiOps =
{
	qQuailConnConnectProgress,
	qQuailConnConnected,
	qQuailConnDisconnected,
	qQuailConnNotice,
	NULL
};

PurpleConnectionUiOps *
qQuailGetConnectionUiOps()
{
	return &connUiOps;
}
