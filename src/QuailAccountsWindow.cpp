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
#include <QHeaderView>
#include <QLabel>
#include <QLayout>
#include <QListView>
#include <QMenuBar>
#include <QPushButton>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>

QQuailAccountItem::QQuailAccountItem(int index)
    : account(NULL), m_index(index), pulseTimer(0), pulseOrigPixmapName("")
{
}

QQuailAccountItem::~QQuailAccountItem()
{
    //stopPulse();
}

void
QQuailAccountItem::startPulse(QPixmap onlinePixmap, QString pixmapName)
{
    qDebug() << "QQuailAccountItem::startPulse";
	stopPulse();

	pulseGrey = true;
	pulseStep = 0;

	pulseOrigPixmap = new QPixmap(onlinePixmap);
    pulseOrigPixmapName = pixmapName;
    if (pulseTimer == 0) {
        pulseTimer = new QTimer(this);
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
    if (pulseTimer == 0)
        return;
    pulseTimer->stop();
    delete pulseOrigPixmap;
    pulseOrigPixmap = NULL;
}

void
QQuailAccountItem::setAccount(PurpleAccount *account)
{
	this->account = account;
}

PurpleAccount *
QQuailAccountItem::getAccount() const
{
	return account;
}

QString
QQuailAccountItem::key(int, bool) const
{
	QString str;

    str.sprintf("%6d", m_index);

	return str;
}

void
QQuailAccountItem::updatePulse()
{
    qDebug() << "QQuailAccountItem::updatePulse()";
    QPixmap tempPixmap = pulseOrigPixmap->copy();

    if (pulseGrey)
        pulseStep += 0.20;
    else
        pulseStep -= 0.20;

    pulseGrey = (pulseStep <= 0);

    QString pixmapName = pulseOrigPixmapName;
    if (pulseGrey)
        pixmapName += "-grey";
    qDebug() << "QQuailAccountItem::updatePulse().1" << pixmapName;
    setIcon(QQuailImageUtils::saturate(tempPixmap, pulseStep, pixmapName));
}

/**************************************************************************
 * QQuailAccountsWindow libpurple callbacks
 **************************************************************************/
static void
signedOnCb(PurpleConnection *gc, QQuailAccountsWindow *win)
{
    qDebug() << "QQuailAccountsWindow::signedOnCb";
	win->accountSignedOn(purple_connection_get_account(gc));
}

static void
signedOffCb(PurpleConnection *gc, QQuailAccountsWindow *win)
{
    qDebug() << "QQuailAccountsWindow::signedOffCb";
	win->accountSignedOff(purple_connection_get_account(gc));
}

//static void
//account_abled_cb(PurpleAccount *account, gpointer user_data)
//{
//    if (GPOINTER_TO_INT(user_data)) {


//    } else {


//    }
//}

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

//    purple_signal_connect(purple_accounts_get_handle(), "account-disabled",
//                        this, PURPLE_CALLBACK(account_abled_cb), GINT_TO_POINTER(FALSE));
//    purple_signal_connect(purple_accounts_get_handle(), "account-enabled",
//                        this, PURPLE_CALLBACK(account_abled_cb), GINT_TO_POINTER(TRUE));

	loadAccounts();
}

QQuailAccountsWindow::~QQuailAccountsWindow()
{
    delete accounts_widget;
}

void
QQuailAccountsWindow::slotUpdateAccounts()
{
	loadAccounts();
}

void
QQuailAccountsWindow::accountSignedOn(PurpleAccount *account)
{
    qDebug() << "QQuailAccountsWindow::accountSignedOn()";
    QQuailAccountItem *item = (QQuailAccountItem *)accounts_widget->currentItem();

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
        item = (QQuailAccountItem*)accounts_widget->item( item->row(), xProtocol);
		item->stopPulse();
        item->setIcon(QQuailProtocolUtils::getProtocolIcon(account));
	}
    qDebug() << "QQuailAccountsWindow::accountSignedOn().end";
}

void
QQuailAccountsWindow::accountSignedOff(PurpleAccount *account)
{
    qDebug() << "QQuailAccountsWindow::accountSignedOff()";
    QQuailAccountItem *item = (QQuailAccountItem *)accounts_widget->currentItem();

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
        QPixmap protIcon = QQuailProtocolUtils::getProtocolIcon(account);
        QString protIconName = QQuailProtocolUtils::getProtocolIconName(account);
        item = (QQuailAccountItem*)accounts_widget->item( item->row(), xProtocol);
        item->setIcon(QQuailImageUtils::greyPixmap(protIcon, protIconName));
	}
    qDebug() << "QQuailAccountsWindow::accountSignedOff().end";
}

void
QQuailAccountsWindow::buildInterface()
{
    qDebug() << "QQuailAccountsWindow::buildInterface";
    setWindowIcon(QIcon(":/data/images/logo.png"));
	setupToolbar();

	/* Create the accounts view */
    accounts_widget = new QTableWidget(this);
    connect(accounts_widget, SIGNAL(itemSelectionChanged()),
            this, SLOT(slot_account_selected()));
    QStringList horzHeaders;
    horzHeaders << tr("Account") << tr("Network") << tr("");
    accounts_widget->setColumnCount(horzHeaders.size());
    accounts_widget->setHorizontalHeaderLabels( horzHeaders );
    accounts_widget->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    accounts_widget->horizontalHeader()->setHighlightSections(false);
    accounts_widget->verticalHeader()->hide();
    accounts_widget->setSelectionBehavior(QAbstractItemView::SelectRows);
    accounts_widget->setIconSize(QSize(25,25));
    setCentralWidget(accounts_widget);
}

void
QQuailAccountsWindow::setupToolbar()
{
    qDebug() << "QQuailAccountsWindow::setupToolbar";
	QAction *a;

	toolbar = new QToolBar(this);
    //toolbar->setMovable(false);

    /* Buddy List */
    a = new QAction(QIcon(QPixmap(":/data/images/actions/blist.png")),
                    tr("Buddy List"),
                    this);
    toolbar->addAction(a);

    connect(a, SIGNAL(triggered(bool)),
            parentMainWindow, SLOT(showBlistWindow()));

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
    a = new QAction(QIcon(QPixmap(":/data/images/actions/conversations.png")),
                    tr("Conversations"),
                    this);
    toolbar->addAction(a);
    connect(a, SIGNAL(triggered(bool)),
            parentMainWindow, SLOT(showConvWindow()));

    /* Now we're going to construct the toolbar on the right. */
    toolbar->addSeparator();

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

    this->addToolBar(toolbar);
}

void
QQuailAccountsWindow::loadAccounts()
{
    qDebug() << "QQuailAccountsWindow::loadAccounts";
    GList *l;
    int index = 0;
    accounts_widget->setUpdatesEnabled(false);
    accounts_widget->clearContents();
    while (accounts_widget->rowCount() > 0)
        accounts_widget->removeRow(0);

    for (l = purple_accounts_get_all(), index = 0;
		 l != NULL;
		 l = l->next, index++)
	{
		QPixmap protocolIcon;
        QString protocolIconName;
		PurpleAccount *account = (PurpleAccount *)l->data;
		QString protocolId = purple_account_get_protocol_id(account);
        qDebug() << "QQuailAccountsWindow::loadAccounts" << index;
        qDebug() << "QQuailAccountsWindow::loadAccounts" << protocolId;
        qDebug() << "QQuailAccountsWindow::loadAccounts" << account->alias;
        qDebug() << "QQuailAccountsWindow::loadAccounts" << purple_account_get_username(account);

        accounts_widget->insertRow(index);

        QQuailAccountItem *itemUserName = new QQuailAccountItem(index);
        itemUserName->setText(purple_account_get_username(account));
        itemUserName->setAccount(account);

        QQuailAccountItem *itemProtocol = new QQuailAccountItem(index);
        itemProtocol->setText(QQuailProtocolUtils::getProtocolName(protocolId));
        itemProtocol->setAccount(account);
        protocolIcon = QQuailProtocolUtils::getProtocolIcon(account);
        protocolIconName = QQuailProtocolUtils::getProtocolIconName(account);
        if (purple_account_is_connected(account))
            itemProtocol->setIcon(protocolIcon);
        else
            itemProtocol->setIcon(QQuailImageUtils::greyPixmap(protocolIcon,
                                                               protocolIconName));

        QQuailAccountItem *itemEnabled = new QQuailAccountItem(index);
        itemEnabled->setAccount(account);
        if (purple_account_get_enabled(account, UI_ID))
        {
            itemEnabled->setCheckState(Qt::Checked);
        }
        else
        {
            itemEnabled->setCheckState(Qt::Unchecked);
        }


        accounts_widget->setItem(index, xUserName, itemUserName);
        accounts_widget->setItem(index, xProtocol, itemProtocol);
        accounts_widget->setItem(index, xEnabled, itemEnabled);

	}
    accounts_widget->setUpdatesEnabled(true);
}

void
QQuailAccountsWindow::newAccount()
{
    qDebug() << "QQuailAccountsWindow::newAccount";
	QQuailAccountEditor *editor;

    editor = new QQuailAccountEditor(NULL, this, tr("New Account"));
    connect(editor, SIGNAL(signalAccountUpdated()),
            this, SLOT(slotUpdateAccounts()));

    editor->show();
}

void
QQuailAccountsWindow::editAccount()
{
    qDebug() << "QQuailAccountsWindow::editAccount";

    QQuailAccountItem *item = (QQuailAccountItem *)accounts_widget->currentItem();

    QQuailAccountEditor *editor = new QQuailAccountEditor(item->getAccount(),
                                     this,
                                     tr("Edit Account"));
    editor->show();
}

void
QQuailAccountsWindow::deleteAccount()
{
    qDebug() << "QQuailAccountsWindow::deleteAccount";
    QQuailAccountItem *item = (QQuailAccountItem *)accounts_widget->currentItem();

	purple_accounts_remove(item->getAccount());

    accounts_widget->removeRow(item->row());

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

    QQuailAccountItem *item = (QQuailAccountItem *)accounts_widget->currentItem();
    PurpleAccount *account = item->getAccount();

//    item->startPulse(QQuailProtocolUtils::getProtocolIcon(account),
//                     QQuailProtocolUtils::getProtocolIconName(account));

    purple_account_set_enabled(account, UI_ID, TRUE);
    purple_account_connect(account);
    qDebug() << "QQuailAccountsWindow::connectToAccount.end";
}

void
QQuailAccountsWindow::disconnectFromAccount()
{
    qDebug() << "QQuailAccountsWindow::disconnectFromAccount";
    QQuailAccountItem *item = (QQuailAccountItem *)accounts_widget->currentItem();

	purple_account_disconnect(item->getAccount());
}

void
QQuailAccountsWindow::accountsToggled(bool)
{
    qDebug() << "QQuailAccountsWindow::accountsToggled";
    accountsButton->setChecked(true);
}

void
QQuailAccountsWindow::slot_account_selected()
{
    qDebug() << "QQuailAccountsWindow::accountSelected";
    QTableWidgetItem *item = accounts_widget->currentItem();
    if (!item)
        return;

    QQuailAccountItem *accountItem = (QQuailAccountItem *)item;
    PurpleAccount *account = accountItem->getAccount();
    const char *protocolId = purple_account_get_protocol_id(account);

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
    accounts_widget->setColumnWidth(2, (accounts_widget->width() * 0.1) -2);
    accounts_widget->setColumnWidth(1, (accounts_widget->width() * 0.15) -2);
    accounts_widget->setColumnWidth(0, (accounts_widget->width() -
                                 (accounts_widget->columnWidth(1) +
                                  accounts_widget->columnWidth(2)) - 2));
}

QQuailAccountItem *
QQuailAccountsWindow::getItemFromAccount(PurpleAccount *account)
{
    qDebug() << "QQuailAccountsWindow::getItemFromAccount";
    QQuailAccountItem *gitem;

	if (account == NULL)
		return NULL;

    for (int i=0; i<accounts_widget->rowCount(); ++i)
	{
        gitem = (QQuailAccountItem *)accounts_widget->item(i,0);

		if (gitem->getAccount() == account)
			return gitem;
	}

	return NULL;
}

static void
qQuailConnConnectProgress(PurpleConnection *gc, const char *text,
						 size_t step, size_t step_count)
{
    qDebug() << "QQuailAccountsWindow::qQuailConnConnectProgress" << gc->display_name;
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
    qDebug() << "QQuailAccountsWindow::qQuailConnConnected" << gc->display_name;
	QQuailConnectionMeters *meters = qQuailGetMainWindow()->getMeters();
	QQuailConnectionMeter *meter;

	meter = meters->findMeter(gc);

	if (meter != NULL)
		meters->removeMeter(meter);
}

static void
qQuailConnDisconnected(PurpleConnection *gc)
{
    qDebug() << "QQuailAccountsWindow::qQuailConnDisconnected" << gc->display_name;
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
    qDebug() << text;
    Q_UNUSED(gc)
}

static void
qQuailConnectionReportDisconnectReason(PurpleConnection *gc,
                                       PurpleConnectionError reason,
                                       const char *text)
{
    Q_UNUSED(gc)
    qDebug() << "QQuailAccountsWindow::qQuailConnectionReportDisconnectReason";
    qDebug() << reason;
    qDebug() << text;
}

static void
qQuail_connection_network_disconnected(void)
{
    qDebug() << "QQuailAccountsWindow::qQuail_connection_network_disconnected";

}

static void
qQuail_connection_network_connected(void)
{
    qDebug() << "QQuailAccountsWindow::qQuail_connection_network_connected";
}

static PurpleConnectionUiOps connUiOps =
{
    qQuailConnConnectProgress, /*connect_progress*/
    qQuailConnConnected, /* connected */
    qQuailConnDisconnected, /* disconnected */
    qQuailConnNotice, /* notice */
    NULL, /*report_disconnect */
    qQuail_connection_network_connected, /* network_connected*/
    qQuail_connection_network_disconnected, /* network_disconnected*/
    qQuailConnectionReportDisconnectReason, /* report_disconnect_reason*/
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

