/**
 * @file QGaimAccountsWindow.cpp Accounts window
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
#include "QGaimAccountsWindow.h"
#include "QGaimAccountEditor.h"
#include "QGaimConnectionMeter.h"
#include "QGaimConvButton.h"
#include "QGaimImageUtils.h"
#include "QGaimProtocolUtils.h"
#include "QGaimMainWindow.h"
#include "base.h"

#include <libgaim/prpl.h>
#include <libgaim/signals.h>

#include <qpe/resource.h>
#include <qaction.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlistview.h>
#include <qmenubar.h>
#include <qpushbutton.h>
#include <qtimer.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>

/**************************************************************************
 * QGaimAccountListItem
 **************************************************************************/
QGaimAccountListItem::QGaimAccountListItem(QListView *parent, int index)
	: QListViewItem(parent), account(NULL), index(index), pulseTimer(NULL)
{
}

QGaimAccountListItem::~QGaimAccountListItem()
{
	stopPulse();
}

void
QGaimAccountListItem::startPulse(QPixmap onlinePixmap)
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
QGaimAccountListItem::stopPulse()
{
	if (pulseTimer == NULL)
		return;

	delete pulseTimer;
	delete pulseOrigPixmap;

	pulseTimer      = NULL;
	pulseOrigPixmap = NULL;
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

void
QGaimAccountListItem::updatePulse()
{
	QPixmap tempPixmap(*pulseOrigPixmap);

	setPixmap(0, QGaimImageUtils::saturate(tempPixmap, pulseStep));

	if (pulseGrey)
		pulseStep += 0.20;
	else
		pulseStep -= 0.20;

	pulseGrey = (pulseStep <= 0);
}

/**************************************************************************
 * QGaimAccountsWindow gaim callbacks
 **************************************************************************/
static void
signedOnCb(GaimConnection *gc, QGaimAccountsWindow *win)
{
	win->accountSignedOn(gaim_connection_get_account(gc));
}

static void
signedOffCb(GaimConnection *gc, QGaimAccountsWindow *win)
{
	win->accountSignedOff(gaim_connection_get_account(gc));
}

/**************************************************************************
 * QGaimAccountsWindow
 **************************************************************************/
QGaimAccountsWindow::QGaimAccountsWindow(QMainWindow *parent)
	: QMainWindow(), parentMainWindow(parent)
{
	connect(parent, SIGNAL(pixmapSizeChanged(bool)),
			this, SLOT(setUsesBigPixmaps(bool)));
	setUsesBigPixmaps(parent->usesBigPixmaps());

	buildInterface();

	gaim_signal_connect(gaim_connections_get_handle(), "signed-on",
						this, GAIM_CALLBACK(signedOnCb), this);
	gaim_signal_connect(gaim_connections_get_handle(), "signed-off",
						this, GAIM_CALLBACK(signedOffCb), this);

	loadAccounts();
}

QGaimAccountsWindow::~QGaimAccountsWindow()
{
	delete accountsView;
}

void
QGaimAccountsWindow::updateAccounts()
{
	loadAccounts();
}

void
QGaimAccountsWindow::accountSignedOn(GaimAccount *account)
{
	QGaimAccountListItem *item;

	item = (QGaimAccountListItem *)accountsView->selectedItem();

	if (item->getAccount() == account)
	{
		connectButton->setEnabled(false);
		disconnectButton->setEnabled(true);
		deleteButton->setEnabled(false);
	}
	else
		item = QGaimAccountsWindow::getItemFromAccount(account);

	if (item != NULL)
	{
		item->stopPulse();
		item->setPixmap(0, QGaimProtocolUtils::getProtocolIcon(account));
	}
}

void
QGaimAccountsWindow::accountSignedOff(GaimAccount *account)
{
	QGaimAccountListItem *item;

	item = (QGaimAccountListItem *)accountsView->selectedItem();

	if (item->getAccount() == account)
	{
		connectButton->setEnabled(true);
		disconnectButton->setEnabled(false);
		deleteButton->setEnabled(true);
	}
	else
		item = QGaimAccountsWindow::getItemFromAccount(account);

	if (item != NULL)
	{
		QPixmap protocolIcon = QGaimProtocolUtils::getProtocolIcon(account);

		item->setPixmap(0, QGaimImageUtils::greyPixmap(protocolIcon));
	}
}

void
QGaimAccountsWindow::buildInterface()
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
		QPixmap protocolIcon;
		QGaimAccountListItem *item;
		GaimAccount *account = (GaimAccount *)l->data;
		QString protocolId = gaim_account_get_protocol_id(account);

		protocolIcon = QGaimProtocolUtils::getProtocolIcon(account);

		item = new QGaimAccountListItem(accountsView, index);
		item->setText(0, gaim_account_get_username(account));
		item->setText(1, QGaimProtocolUtils::getProtocolName(protocolId));
		item->setAccount(account);

		if (gaim_account_is_connected(account))
			item->setPixmap(0, protocolIcon);
		else
			item->setPixmap(0, QGaimImageUtils::greyPixmap(protocolIcon));
	}
}

void
QGaimAccountsWindow::newAccount()
{
	QGaimAccountEditor *editor;

	editor = new QGaimAccountEditor(NULL, this, "", true);
	editor->setAccountsWindow(this);

	editor->showMaximized();
}

void
QGaimAccountsWindow::editAccount()
{
	QGaimAccountEditor *editor;
	QGaimAccountListItem *item;

	item = (QGaimAccountListItem *)accountsView->selectedItem();

	editor = new QGaimAccountEditor(item->getAccount(), this, "", true);
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

	item->startPulse(QGaimProtocolUtils::getProtocolIcon(item->getAccount()));

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
	qGaimGetMainWindow()->showBlistWindow();
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
	const char *protocolId;

	account    = accountItem->getAccount();
	protocolId = gaim_account_get_protocol_id(account);

	if (gaim_plugins_find_with_id(protocolId) == NULL)
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

void
QGaimAccountsWindow::resizeEvent(QResizeEvent *)
{
	accountsView->setColumnWidth(1, (accountsView->width()) / 4);
	accountsView->setColumnWidth(0, accountsView->width() - 20 -
								 accountsView->columnWidth(1));
}

QGaimAccountListItem *
QGaimAccountsWindow::getItemFromAccount(GaimAccount *account)
{
	QListViewItem *item;
	QGaimAccountListItem *gitem;

	if (account == NULL)
		return NULL;

	for (item = accountsView->firstChild();
		 item != NULL;
		 item = item->nextSibling())
	{
		gitem = (QGaimAccountListItem *)item;

		if (gitem->getAccount() == account)
			return gitem;
	}

	return NULL;
}

static void
qGaimConnConnectProgress(GaimConnection *gc, const char *text,
						 size_t step, size_t step_count)
{
	QGaimConnectionMeters *meters;
	QGaimConnectionMeter *meter;

	meters = qGaimGetMainWindow()->getMeters();
	meter = meters->findMeter(gc);

	if (meter == NULL)
		meter = meters->addConnection(gc);

	meter->update(QString(text), step, step_count);
}

static void
qGaimConnConnected(GaimConnection *gc)
{
	QGaimConnectionMeters *meters = qGaimGetMainWindow()->getMeters();
	QGaimConnectionMeter *meter;

	meter = meters->findMeter(gc);

	if (meter != NULL)
		meters->removeMeter(meter);
}

static void
qGaimConnDisconnected(GaimConnection *gc)
{
	QGaimConnectionMeters *meters = qGaimGetMainWindow()->getMeters();
	QGaimConnectionMeter *meter;

	meter = meters->findMeter(gc);

	if (meter != NULL)
		meters->removeMeter(meter);
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
	qGaimConnNotice,
	NULL
};

GaimConnectionUiOps *
qGaimGetConnectionUiOps()
{
	return &connUiOps;
}
