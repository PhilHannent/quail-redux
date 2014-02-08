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

quail_account_item::quail_account_item()
    : account(NULL)
    , pulseTimer(0)
    , pulseOrigPixmapName("")
{
}

quail_account_item::~quail_account_item()
{
    //stopPulse();
}

void
quail_account_item::startPulse(QPixmap onlinePixmap, QString pixmapName)
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
quail_account_item::stopPulse()
{
    qDebug() << "QQuailAccountItem::stopPulse";
    if (pulseTimer == 0)
        return;
    pulseTimer->stop();
    delete pulseOrigPixmap;
    pulseOrigPixmap = NULL;
}

void
quail_account_item::setAccount(PurpleAccount *account)
{
	this->account = account;
}

PurpleAccount *
quail_account_item::get_account() const
{
	return account;
}

void
quail_account_item::updatePulse()
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
signedOnCb(PurpleConnection *gc, quail_accounts_window *win)
{
    qDebug() << "QQuailAccountsWindow::signedOnCb";
	win->accountSignedOn(purple_connection_get_account(gc));
}

static void
signedOffCb(PurpleConnection *gc, quail_accounts_window *win)
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
quail_accounts_window::quail_accounts_window(QMainWindow *parent)
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

quail_accounts_window::~quail_accounts_window()
{
    delete m_accounts_widget;
}

void
quail_accounts_window::slotUpdateAccounts()
{
	loadAccounts();
}

void
quail_accounts_window::accountSignedOn(PurpleAccount *account)
{
    qDebug() << "QQuailAccountsWindow::accountSignedOn()";
    quail_account_item *item = (quail_account_item *)m_accounts_widget->currentItem();

    if (item->get_account() == account)
	{
		connectButton->setEnabled(false);
		disconnectButton->setEnabled(true);
		deleteButton->setEnabled(false);
	}
	else
        item = quail_accounts_window::get_item_from_account(account);

	if (item != NULL)
	{
        item = (quail_account_item*)m_accounts_widget->item( item->row(), xProtocol);
		item->stopPulse();
        item->setIcon(QQuailProtocolUtils::getProtocolIcon(account));
	}
    qDebug() << "QQuailAccountsWindow::accountSignedOn().end";
}

void
quail_accounts_window::accountSignedOff(PurpleAccount *account)
{
    qDebug() << "QQuailAccountsWindow::accountSignedOff()";
    quail_account_item *item = (quail_account_item *)m_accounts_widget->currentItem();

    if (item->get_account() == account)
	{
		connectButton->setEnabled(true);
		disconnectButton->setEnabled(false);
		deleteButton->setEnabled(true);
	}
	else
        item = quail_accounts_window::get_item_from_account(account);

	if (item != NULL)
	{
        QPixmap protIcon = QQuailProtocolUtils::getProtocolIcon(account);
        QString protIconName = QQuailProtocolUtils::getProtocolIconName(account);
        item = (quail_account_item*)m_accounts_widget->item( item->row(), xProtocol);
        item->setIcon(QQuailImageUtils::greyPixmap(protIcon, protIconName));
	}
    qDebug() << "QQuailAccountsWindow::accountSignedOff().end";
}

void
quail_accounts_window::buildInterface()
{
    qDebug() << "QQuailAccountsWindow::buildInterface";
    setWindowIcon(QIcon(":/data/images/logo.png"));
	setupToolbar();

	/* Create the accounts view */
    m_accounts_widget = new QTableWidget(this);
    connect(m_accounts_widget, SIGNAL(itemSelectionChanged()),
            this, SLOT(slot_account_selected()));
    connect(m_accounts_widget,SIGNAL(itemPressed(QTableWidgetItem *)),
            this,SLOT(slot_table_item_pressed(QTableWidgetItem *)));
    connect(m_accounts_widget,SIGNAL(itemClicked(QTableWidgetItem *)),
            this,SLOT(slot_table_item_pressed(QTableWidgetItem *)));

    QStringList horzHeaders;
    horzHeaders << tr("Account") << tr("Network") << tr("");
    m_accounts_widget->setColumnCount(horzHeaders.size());
    m_accounts_widget->setHorizontalHeaderLabels( horzHeaders );
    m_accounts_widget->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    m_accounts_widget->horizontalHeader()->setHighlightSections(false);
    m_accounts_widget->verticalHeader()->hide();
    m_accounts_widget->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_accounts_widget->setIconSize(QSize(25,25));
    setCentralWidget(m_accounts_widget);
}

void
quail_accounts_window::setupToolbar()
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
quail_accounts_window::loadAccounts()
{
    qDebug() << "QQuailAccountsWindow::loadAccounts";
    GList *l;
    int index = 0;
    m_accounts_widget->setUpdatesEnabled(false);
    m_accounts_widget->clearContents();
    while (m_accounts_widget->rowCount() > 0)
        m_accounts_widget->removeRow(0);

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

        m_accounts_widget->insertRow(index);

        quail_account_item *itemUserName = new quail_account_item();
        itemUserName->setText(purple_account_get_username(account));
        itemUserName->setAccount(account);

        quail_account_item *itemProtocol = new quail_account_item();
        itemProtocol->setText(QQuailProtocolUtils::getProtocolName(protocolId));
        itemProtocol->setAccount(account);
        protocolIcon = QQuailProtocolUtils::getProtocolIcon(account);
        protocolIconName = QQuailProtocolUtils::getProtocolIconName(account);
        if (purple_account_is_connected(account))
            itemProtocol->setIcon(protocolIcon);
        else
            itemProtocol->setIcon(QQuailImageUtils::greyPixmap(protocolIcon,
                                                               protocolIconName));

        quail_account_item *itemEnabled = new quail_account_item();
        itemEnabled->setAccount(account);
        if (purple_account_get_enabled(account, UI_ID))
        {
            itemEnabled->setCheckState(Qt::Checked);
        }
        else
        {
            itemEnabled->setCheckState(Qt::Unchecked);
        }


        m_accounts_widget->setItem(index, xUserName, itemUserName);
        m_accounts_widget->setItem(index, xProtocol, itemProtocol);
        m_accounts_widget->setItem(index, xEnabled, itemEnabled);

	}
    m_accounts_widget->setUpdatesEnabled(true);
}

void
quail_accounts_window::newAccount()
{
    qDebug() << "QQuailAccountsWindow::newAccount";
	QQuailAccountEditor *editor;

    editor = new QQuailAccountEditor(NULL, this, tr("New Account"));
    connect(editor, SIGNAL(signalAccountUpdated()),
            this, SLOT(slotUpdateAccounts()));

    editor->show();
}

void
quail_accounts_window::editAccount()
{
    qDebug() << "QQuailAccountsWindow::editAccount";

    quail_account_item *item = (quail_account_item *)m_accounts_widget->currentItem();

    QQuailAccountEditor *editor = new QQuailAccountEditor(item->get_account(),
                                     this,
                                     tr("Edit Account"));
    editor->show();
}

void
quail_accounts_window::deleteAccount()
{
    qDebug() << "QQuailAccountsWindow::deleteAccount";
    quail_account_item *item = (quail_account_item *)m_accounts_widget->currentItem();

    purple_accounts_remove(item->get_account());

    m_accounts_widget->removeRow(item->row());

	connectButton->setEnabled(false);
	disconnectButton->setEnabled(false);
	editButton->setEnabled(false);
	deleteButton->setEnabled(false);
}

void
quail_accounts_window::connectToAccount()
{
    qDebug() << "QQuailAccountsWindow::connectToAccount";
    connectButton->setEnabled(false);

    quail_account_item *item = (quail_account_item *)m_accounts_widget->currentItem();
    PurpleAccount *account = item->get_account();

//    item->startPulse(QQuailProtocolUtils::getProtocolIcon(account),
//                     QQuailProtocolUtils::getProtocolIconName(account));

    purple_account_set_enabled(account, UI_ID, TRUE);
    purple_account_connect(account);
    qDebug() << "QQuailAccountsWindow::connectToAccount.end";
}

void
quail_accounts_window::disconnectFromAccount()
{
    qDebug() << "QQuailAccountsWindow::disconnectFromAccount";
    quail_account_item *item = (quail_account_item *)m_accounts_widget->currentItem();

    purple_account_disconnect(item->get_account());
}

void
quail_accounts_window::accountsToggled(bool)
{
    qDebug() << "QQuailAccountsWindow::accountsToggled";
    accountsButton->setChecked(true);
}

void
quail_accounts_window::slot_account_selected()
{
    qDebug() << "QQuailAccountsWindow::accountSelected";
    QTableWidgetItem *item = m_accounts_widget->currentItem();
    if (!item)
        return;

    quail_account_item *accountItem = (quail_account_item *)item;
    PurpleAccount *account = accountItem->get_account();
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
quail_accounts_window::resizeEvent(QResizeEvent *)
{
    m_accounts_widget->setColumnWidth(2, (m_accounts_widget->width() * 0.1) -2);
    m_accounts_widget->setColumnWidth(1, (m_accounts_widget->width() * 0.15) -2);
    m_accounts_widget->setColumnWidth(0, (m_accounts_widget->width() -
                                 (m_accounts_widget->columnWidth(1) +
                                  m_accounts_widget->columnWidth(2)) - 2));
}

quail_account_item *
quail_accounts_window::get_item_from_account(PurpleAccount *account)
{
    qDebug() << "QQuailAccountsWindow::getItemFromAccount";

	if (account == NULL)
		return NULL;

    for (int i=0; i<m_accounts_widget->rowCount(); ++i)
	{
        quail_account_item *my_item = (quail_account_item *)m_accounts_widget->item(i,0);

        if (my_item->get_account() == account)
            return my_item;
	}

	return NULL;
}

void
quail_accounts_window::slot_table_item_pressed(QTableWidgetItem * item_selected)
{
    qDebug() << "quail_accounts_window::slot_table_item_pressed";
    quail_account_item* account_item = static_cast<quail_account_item*>(
                m_accounts_widget->item(item_selected->row(), xEnabled));

    PurpleAccount* account = account_item->get_account();
    if (account_item->checkState() == Qt::Checked
        && !purple_account_get_enabled(account, UI_ID))
    {
        qDebug() << "quail_accounts_window::slot_table_item_pressed.1";
        purple_account_set_enabled(account, UI_ID, true);
    }
    else if (account_item->checkState() == Qt::Unchecked
             && purple_account_get_enabled(account, UI_ID))
    {
        qDebug() << "quail_accounts_window::slot_table_item_pressed.2";
        purple_account_set_enabled(account, UI_ID, false);
    }
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

