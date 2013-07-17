/**
 * @file QQuailAccountsWindow.cpp Accounts window
 *
 * @Copyright (C) 2003-2004 Christian Hammond.
 * @Copyright (C) 2013      Phil Hannent.
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

#include <libpurple/prpl.h>
#include <libpurple/signals.h>

#include <QAction>
#include <QCheckBox>
#include <QDebug>
#include <QLabel>
#include <QLayout>
#include <QListView>
#include <QMenuBar>
#include <QPushButton>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>

QQuailAccountItem::QQuailAccountItem(int index)
    : account(NULL), index(index), pulseTimer(0)
{
    qDebug() << "QQuailAccountItem::QQuailAccountItem";
}

QQuailAccountItem::~QQuailAccountItem()
{
	stopPulse();
}

void
QQuailAccountItem::startPulse(QPixmap onlinePixmap)
{
    qDebug() << "QQuailAccountItem::startPulse";
	stopPulse();

	pulseGrey = true;
	pulseStep = 0;

	pulseOrigPixmap = new QPixmap(onlinePixmap);
    if (pulseTimer == 0) {
        pulseTimer = new QTimer();
        pulseTimer->setSingleShot(false);

        connect(pulseTimer, SIGNAL(timeout()),
                this, SLOT(updatePulse()));
    }
    pulseTimer->start(100);
}

void
QQuailAccountItem::stopPulse()
{
    qDebug() << "QQuailAccountItem::stopPulse";
	if (pulseTimer == NULL)
		return;

	delete pulseTimer;
	delete pulseOrigPixmap;

	pulseTimer      = NULL;
	pulseOrigPixmap = NULL;
}

void
QQuailAccountItem::setAccount(PurpleAccount *account)
{
    qDebug() << "QQuailAccountItem::setAccount";
	this->account = account;
}

PurpleAccount *
QQuailAccountItem::getAccount() const
{
    qDebug() << "QQuailAccountItem::getAccount";
	return account;
}

QString
QQuailAccountItem::key(int, bool) const
{
	QString str;

	str.sprintf("%6d", index);

	return str;
}

void
QQuailAccountItem::updatePulse()
{
	QPixmap tempPixmap(*pulseOrigPixmap);

    setIcon(QQuailImageUtils::saturate(tempPixmap, pulseStep));

	if (pulseGrey)
		pulseStep += 0.20;
	else
		pulseStep -= 0.20;

	pulseGrey = (pulseStep <= 0);
}

/**************************************************************************
 * QQuailAccountsWindow libpurple callbacks
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
    qDebug() << "QQuailAccountsWindow::QQuailAccountsWindow";
	buildInterface();

	purple_signal_connect(purple_connections_get_handle(), "signed-on",
                        this, PURPLE_CALLBACK(signedOnCb), this);
	purple_signal_connect(purple_connections_get_handle(), "signed-off",
                        this, PURPLE_CALLBACK(signedOffCb), this);

	loadAccounts();
}

QQuailAccountsWindow::~QQuailAccountsWindow()
{
    delete accountsWidget;
}

void
QQuailAccountsWindow::updateAccounts()
{
	loadAccounts();
}

void
QQuailAccountsWindow::accountSignedOn(PurpleAccount *account)
{
    QQuailAccountItem *item = (QQuailAccountItem *)accountsWidget->currentItem();

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
        item->setIcon(QQuailProtocolUtils::getProtocolIcon(account));
	}
}

void
QQuailAccountsWindow::accountSignedOff(PurpleAccount *account)
{
    QQuailAccountItem *item = (QQuailAccountItem *)accountsWidget->currentItem();

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

        item->setIcon(QQuailImageUtils::greyPixmap(protocolIcon));
	}
}

void
QQuailAccountsWindow::buildInterface()
{
    qDebug() << "QQuailAccountsWindow::buildInterface";
	setupToolbar();

	/* Create the accounts view */
    accountsWidget = new QTableWidget(this);
    QStringList horzHeaders;
    horzHeaders << tr("Username") << tr("Network");
//    horzHeaders << tr("Network")
//                << tr("Username")
//                << tr("Enabled")
//                << tr("Status");
    accountsWidget->setColumnCount(horzHeaders.size());
    accountsWidget->setHorizontalHeaderLabels( horzHeaders );
    accountsWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(accountsWidget, SIGNAL(itemSelectionChanged()),
            this, SLOT(accountSelected()));

    setCentralWidget(accountsWidget);
}

void
QQuailAccountsWindow::setupToolbar()
{
    qDebug() << "QQuailAccountsWindow::setupToolbar";
	QAction *a;
    QToolButton *button;

	toolbar = new QToolBar(this);
    toolbar->setMovable(false);

	/* New */
    a = new QAction(QIcon(QPixmap(":/data/images/actions/new.png")),
                    tr("New Account"), this);
    toolbar->addAction(a);

    connect(a, SIGNAL(triggered(bool)),
			this, SLOT(newAccount()));

	/* Edit */
    a = new QAction(QIcon(QPixmap(":/data/images/actions/edit.png")),
                    tr("Edit Account"),
                    this);
	editButton = a;
    toolbar->addAction(a);
    a->setEnabled(false);

    connect(a, SIGNAL(triggered(bool)),
			this, SLOT(editAccount()));

	/* Delete */
    a = new QAction(QIcon(QPixmap(":/data/images/actions/delete.png")),
                    tr("Delete"),
                    this);
	deleteButton = a;
    toolbar->addAction(a);
    a->setEnabled(false);

    connect(a, SIGNAL(triggered(bool)),
			this, SLOT(deleteAccount()));

	/* Separator */
	toolbar->addSeparator();

	/* Connect */
    a = new QAction(QIcon(QPixmap(":/data/images/actions/connect.png")),
                    tr("Connect"),
                    this);
	connectButton = a;
    toolbar->addAction(a);

	a->setEnabled(false);
    connect(a, SIGNAL(triggered(bool)),
			this, SLOT(connectToAccount()));

	/* Disconnect */
    a = new QAction(QIcon(QPixmap(":/data/images/actions/disconnect.png")),
                    tr("Connect"),
                    this);
	disconnectButton = a;
    toolbar->addAction(a);

	a->setEnabled(false);
    connect(a, SIGNAL(triggered(bool)),
			this, SLOT(disconnectFromAccount()));

	/* Now we're going to construct the toolbar on the right. */
	toolbar->addSeparator();

	/* Buddy List */
    a = new QAction(QIcon(QPixmap(":/data/images/actions/blist.png")),
                    tr("Buddy List"),
                    this);
    toolbar->addAction(a);

    connect(a, SIGNAL(triggered(bool)),
			this, SLOT(showBlist()));

	/* Accounts */
    a = new QAction(QIcon(QPixmap(":/data/images/actions/accounts.png")),
                    tr("Accounts"),
                    this);
    a->setEnabled(true);
	accountsButton = a;
    toolbar->addAction(a);

	connect(a, SIGNAL(toggled(bool)),
			this, SLOT(accountsToggled(bool)));

	/* Conversations */
    button = new QQuailConvButton(toolbar);
    toolbar->addWidget(button);
    this->addToolBar(toolbar);
}

void
QQuailAccountsWindow::loadAccounts()
{
    qDebug() << "QQuailAccountsWindow::loadAccounts";
	GList *l;
	int index;

    //accountsWidget->clear();

	for (l = purple_accounts_get_all(), index = 0;
		 l != NULL;
		 l = l->next, index++)
	{
		QPixmap protocolIcon;
		PurpleAccount *account = (PurpleAccount *)l->data;
		QString protocolId = purple_account_get_protocol_id(account);
        qDebug() << "QQuailAccountsWindow::loadAccounts" << protocolId;
        qDebug() << "QQuailAccountsWindow::loadAccounts" << account->alias;
        qDebug() << "QQuailAccountsWindow::loadAccounts" << purple_account_get_username(account);

        accountsWidget->insertRow(index);

        QQuailAccountItem *itemUserName = new QQuailAccountItem(index);
        itemUserName->setText(purple_account_get_username(account));
        itemUserName->setAccount(account);

        QQuailAccountItem *itemProtocol = new QQuailAccountItem(index);
        itemProtocol->setText(QQuailProtocolUtils::getProtocolName(protocolId));
        itemProtocol->setAccount(account);
        //itemProtocol->setSizeHint();
        protocolIcon = QQuailProtocolUtils::getProtocolIcon(account);
        if (purple_account_is_connected(account))
            itemProtocol->setIcon(protocolIcon);
        else
            itemProtocol->setIcon(QQuailImageUtils::greyPixmap(protocolIcon));

//        QQuailAccountCheckBox *itemEnabled = new QQuailAccountCheckBox(index);
//        itemEnabled->setAccount(account);

//        QQuailAccountItem *itemStatus = new QQuailAccountItem(index);
//        itemStatus->setAccount(account);
//        itemStatus->setText(tr("Unknown"));

        accountsWidget->setItem(index, 0, itemUserName);
        accountsWidget->setItem(index, 1, itemProtocol);
//        accountsWidget->setCellWidget(index, 2, itemEnabled);
//        accountsWidget->setItem(index, 3, itemStatus);


	}
    accountsWidget->resizeColumnsToContents();
}

void
QQuailAccountsWindow::newAccount()
{
    qDebug() << "QQuailAccountsWindow::newAccount";
	QQuailAccountEditor *editor;

    editor = new QQuailAccountEditor(NULL, this, tr("New Account"));
	editor->setAccountsWindow(this);

    editor->show();
}

void
QQuailAccountsWindow::editAccount()
{
    qDebug() << "QQuailAccountsWindow::editAccount";
	QQuailAccountEditor *editor;
    QQuailAccountItem *item;

    item = (QQuailAccountItem *)accountsWidget->currentItem();

    editor = new QQuailAccountEditor(item->getAccount(), this, tr("Edit Account"));
	editor->setAccountsWindow(this);

    editor->show();
}

void
QQuailAccountsWindow::deleteAccount()
{
    qDebug() << "QQuailAccountsWindow::deleteAccount";
    QQuailAccountItem *item;

    item = (QQuailAccountItem *)accountsWidget->currentItem();

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
    qDebug() << "QQuailAccountsWindow::connectToAccount";
    connectButton->setEnabled(false);

    QQuailAccountItem *item = (QQuailAccountItem *)accountsWidget->currentItem();

	item->startPulse(QQuailProtocolUtils::getProtocolIcon(item->getAccount()));

	purple_account_connect(item->getAccount());
}

void
QQuailAccountsWindow::disconnectFromAccount()
{
    qDebug() << "QQuailAccountsWindow::disconnectFromAccount";
    QQuailAccountItem *item = (QQuailAccountItem *)accountsWidget->currentItem();

	purple_account_disconnect(item->getAccount());
}

void
QQuailAccountsWindow::showBlist()
{
    qDebug() << "QQuailAccountsWindow::showBlist";
	qQuailGetMainWindow()->showBlistWindow();
}

void
QQuailAccountsWindow::accountsToggled(bool)
{
    qDebug() << "QQuailAccountsWindow::accountsToggled";
    accountsButton->setChecked(true);
}

void
QQuailAccountsWindow::accountSelected()
{
    qDebug() << "QQuailAccountsWindow::accountSelected";
    QTableWidgetItem *item = accountsWidget->currentItem();
    QQuailAccountItem *accountItem = (QQuailAccountItem *)item;
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
    accountsWidget->setColumnWidth(1, ((accountsWidget->width()) / 4) -2);
    accountsWidget->setColumnWidth(0, (accountsWidget->width() - 20 -
                                 accountsWidget->columnWidth(1)) - 2);
}

QQuailAccountItem *
QQuailAccountsWindow::getItemFromAccount(PurpleAccount *account)
{
    qDebug() << "QQuailAccountsWindow::getItemFromAccount";
    QQuailAccountItem *gitem;

	if (account == NULL)
		return NULL;

    for (int i=0; i<accountsWidget->rowCount(); ++i)
	{
        gitem = (QQuailAccountItem *)accountsWidget->item(i,0);

		if (gitem->getAccount() == account)
			return gitem;
	}

	return NULL;
}

static void
qQuailConnConnectProgress(PurpleConnection *gc, const char *text,
						 size_t step, size_t step_count)
{
    qDebug() << "QQuailAccountsWindow::qQuailConnConnectProgress";
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
    qDebug() << "QQuailAccountsWindow::qQuailConnConnected";
	QQuailConnectionMeters *meters = qQuailGetMainWindow()->getMeters();
	QQuailConnectionMeter *meter;

	meter = meters->findMeter(gc);

	if (meter != NULL)
		meters->removeMeter(meter);
}

static void
qQuailConnDisconnected(PurpleConnection *gc)
{
    qDebug() << "QQuailAccountsWindow::qQuailConnDisconnected";
	QQuailConnectionMeters *meters = qQuailGetMainWindow()->getMeters();
	QQuailConnectionMeter *meter;

	meter = meters->findMeter(gc);

	if (meter != NULL)
		meters->removeMeter(meter);
}

static void
qQuailConnNotice(PurpleConnection *gc, const char *text)
{
    qDebug() << "QQuailAccountsWindow::qQuailConnNotice";
    Q_UNUSED(gc)
    Q_UNUSED(text)
	/* XXX */
	gc = NULL;
	text = NULL;
}

static PurpleConnectionUiOps connUiOps =
{
    qQuailConnConnectProgress, /*connect_progress*/
    qQuailConnConnected, /* connected */
    qQuailConnDisconnected, /* disconnected */
    qQuailConnNotice, /* notice */
    NULL, /*report_disconnect */
    NULL, /* network_connected*/
    NULL, /* network_disconnected*/
    NULL, /* report_disconnect_reason*/
    NULL,
    NULL,
    NULL
};

PurpleConnectionUiOps *
qQuailGetConnectionUiOps()
{
    qDebug() << "QQuailAccountsWindow::qQuailGetConnectionUiOps";
	return &connUiOps;
}

