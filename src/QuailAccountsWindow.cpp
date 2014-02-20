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
    : m_account(NULL)
    , m_pulse_timer(0)
    , m_pulse_orig_pixmap_name("")
{
}

quail_account_item::~quail_account_item()
{
    //stopPulse();
}

void
quail_account_item::start_pulse(QPixmap onlinePixmap, QString pixmapName)
{
    qDebug() << "QQuailAccountItem::startPulse";
    stop_pulse();

    m_pulse_grey = true;
    m_pulse_step = 0;

    m_pulse_orig_pixmap = new QPixmap(onlinePixmap);
    m_pulse_orig_pixmap_name = pixmapName;
    if (m_pulse_timer == 0) {
        m_pulse_timer = new QTimer(this);
        m_pulse_timer->setSingleShot(false);

        connect(m_pulse_timer, SIGNAL(timeout()),
                this, SLOT(update_pulse()));
    }
    m_pulse_timer->start(100);
}

void
quail_account_item::stop_pulse()
{
    qDebug() << "QQuailAccountItem::stopPulse";
    if (m_pulse_timer == 0)
        return;
    m_pulse_timer->stop();
    delete m_pulse_orig_pixmap;
    m_pulse_orig_pixmap = NULL;
}

void
quail_account_item::set_account(PurpleAccount *account)
{
    this->m_account = account;
}

PurpleAccount *
quail_account_item::get_account() const
{
    return m_account;
}

void
quail_account_item::update_pulse()
{
    qDebug() << "QQuailAccountItem::updatePulse()";
    QPixmap tempPixmap = m_pulse_orig_pixmap->copy();

    if (m_pulse_grey)
        m_pulse_step += 0.20;
    else
        m_pulse_step -= 0.20;

    m_pulse_grey = (m_pulse_step <= 0);

    QString pixmapName = m_pulse_orig_pixmap_name;
    if (m_pulse_grey)
        pixmapName += "-grey";
    qDebug() << "QQuailAccountItem::updatePulse().1" << pixmapName;
    setIcon(quail_image_utils::saturate(tempPixmap, m_pulse_step, pixmapName));
}

/**************************************************************************
 * QQuailAccountsWindow libpurple callbacks
 **************************************************************************/
static void
signedOnCb(PurpleConnection *gc, quail_accounts_window *win)
{
    qDebug() << "QQuailAccountsWindow::signedOnCb";
    win->account_signed_on(purple_connection_get_account(gc));
}

static void
signedOffCb(PurpleConnection *gc, quail_accounts_window *win)
{
    qDebug() << "QQuailAccountsWindow::signedOffCb";
    win->account_signed_off(purple_connection_get_account(gc));
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
    : QMainWindow(), m_parent(parent)
{
    qDebug() << "QQuailAccountsWindow::QQuailAccountsWindow";
    build_interface();

	purple_signal_connect(purple_connections_get_handle(), "signed-on",
                        this, PURPLE_CALLBACK(signedOnCb), this);
	purple_signal_connect(purple_connections_get_handle(), "signed-off",
                        this, PURPLE_CALLBACK(signedOffCb), this);

//    purple_signal_connect(purple_accounts_get_handle(), "account-disabled",
//                        this, PURPLE_CALLBACK(account_abled_cb), GINT_TO_POINTER(FALSE));
//    purple_signal_connect(purple_accounts_get_handle(), "account-enabled",
//                        this, PURPLE_CALLBACK(account_abled_cb), GINT_TO_POINTER(TRUE));

    connect(this, SIGNAL(signal_update_accounts()), this, SLOT(slot_update_accounts()));
    emit signal_update_accounts();
}

quail_accounts_window::~quail_accounts_window()
{
    delete m_accounts_widget;
}

void
quail_accounts_window::account_signed_on(PurpleAccount *account)
{
    qDebug() << "QQuailAccountsWindow::accountSignedOn()";
    quail_account_item *item = (quail_account_item *)m_accounts_widget->currentItem();

    if (item->get_account() == account)
	{
        m_delete_btn->setEnabled(false);
	}
	else
        item = quail_accounts_window::get_item_from_account(account);

	if (item != NULL)
	{
        item = (quail_account_item*)m_accounts_widget->item( item->row(), xProtocol);
        item->stop_pulse();
        item->setIcon(QQuailProtocolUtils::getProtocolIcon(account));
	}
    qDebug() << "QQuailAccountsWindow::accountSignedOn().end";
}

void
quail_accounts_window::account_signed_off(PurpleAccount *account)
{
    qDebug() << "QQuailAccountsWindow::accountSignedOff()";
    quail_account_item *item = (quail_account_item *)m_accounts_widget->currentItem();

    if (item->get_account() == account)
	{
        m_delete_btn->setEnabled(true);
	}
	else
        item = quail_accounts_window::get_item_from_account(account);

	if (item != NULL)
	{
        QPixmap protIcon = QQuailProtocolUtils::getProtocolIcon(account);
        QString protIconName = QQuailProtocolUtils::getProtocolIconName(account);
        item = (quail_account_item*)m_accounts_widget->item( item->row(), xProtocol);
        item->setIcon(quail_image_utils::greyPixmap(protIcon, protIconName));
	}
    qDebug() << "QQuailAccountsWindow::accountSignedOff().end";
}

void
quail_accounts_window::build_interface()
{
    qDebug() << "QQuailAccountsWindow::buildInterface";
    setWindowIcon(QIcon(":/data/images/logo.png"));
    setup_toolbar();

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
quail_accounts_window::setup_toolbar()
{
    qDebug() << "QQuailAccountsWindow::setupToolbar";
	QAction *a;

    m_toolbar = new QToolBar(this);
    //toolbar->setMovable(false);

    /* Buddy List */
    a = new QAction(QIcon(QPixmap(":/data/images/actions/blist.png")),
                    tr("Buddy List"),
                    this);
    m_toolbar->addAction(a);

    connect(a, SIGNAL(triggered(bool)),
            m_parent, SLOT(showBlistWindow()));

    /* Accounts */
    a = new QAction(QIcon(QPixmap(":/data/images/actions/accounts.png")),
                    tr("Accounts"),
                    this);
    a->setEnabled(true);
    m_toolbar->addAction(a);

    /* Conversations */
    a = new QAction(QIcon(QPixmap(":/data/images/actions/conversations.png")),
                    tr("Conversations"),
                    this);
    m_toolbar->addAction(a);
    connect(a, SIGNAL(triggered(bool)),
            m_parent, SLOT(showConvWindow()));

    /* Now we're going to construct the toolbar on the right. */
    m_toolbar->addSeparator();

	/* New */
    a = new QAction(QIcon(QPixmap(":/data/images/actions/new.png")),
                    tr("New Account"), this);
    m_toolbar->addAction(a);

    connect(a, SIGNAL(triggered(bool)),
            this, SLOT(new_account()));

	/* Edit */
    a = new QAction(QIcon(QPixmap(":/data/images/actions/edit.png")),
                    tr("Edit Account"),
                    this);
    m_edit_btn = a;
    m_toolbar->addAction(a);
    a->setEnabled(false);

    connect(a, SIGNAL(triggered(bool)),
            this, SLOT(edit_account()));

	/* Delete */
    a = new QAction(QIcon(QPixmap(":/data/images/actions/delete.png")),
                    tr("Delete"),
                    this);
    m_delete_btn = a;
    m_toolbar->addAction(a);
    a->setEnabled(false);

    connect(a, SIGNAL(triggered(bool)),
            this, SLOT(delete_account()));

    this->addToolBar(m_toolbar);
}

void
quail_accounts_window::slot_update_accounts()
{
    qDebug() << "quail_accounts_window::slot_update_accounts";
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
        qDebug() << "quail_accounts_window::slot_update_accounts" << index;
        qDebug() << "quail_accounts_window::slot_update_accounts" << protocolId;
        qDebug() << "quail_accounts_window::slot_update_accounts" << account->alias;
        qDebug() << "quail_accounts_window::slot_update_accounts" << purple_account_get_username(account);

        m_accounts_widget->insertRow(index);

        quail_account_item *itemUserName = new quail_account_item();
        itemUserName->setText(purple_account_get_username(account));
        itemUserName->set_account(account);

        quail_account_item *itemProtocol = new quail_account_item();
        itemProtocol->setText(QQuailProtocolUtils::getProtocolName(protocolId));
        itemProtocol->set_account(account);
        protocolIcon = QQuailProtocolUtils::getProtocolIcon(account);
        protocolIconName = QQuailProtocolUtils::getProtocolIconName(account);
        if (purple_account_is_connected(account))
            itemProtocol->setIcon(protocolIcon);
        else
            itemProtocol->setIcon(quail_image_utils::greyPixmap(protocolIcon,
                                                               protocolIconName));

        quail_account_item *itemEnabled = new quail_account_item();
        itemEnabled->set_account(account);
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
quail_accounts_window::new_account()
{
    qDebug() << "quail_accounts_window::new_account";
	quail_account_editor *editor;

    editor = new quail_account_editor(NULL, this, tr("New Account"));
    connect(editor, SIGNAL(signalAccountUpdated()),
            this, SLOT(slot_update_accounts()));

    editor->show();
}

void
quail_accounts_window::edit_account()
{
    qDebug() << "QQuailAccountsWindow::editAccount";

    quail_account_item *item = (quail_account_item *)m_accounts_widget->currentItem();

    quail_account_editor *editor = new quail_account_editor(item->get_account(),
                                     this,
                                     tr("Edit Account"));
    editor->show();
}

void
quail_accounts_window::delete_account()
{
    qDebug() << "QQuailAccountsWindow::deleteAccount";
    quail_account_item *item = (quail_account_item *)m_accounts_widget->currentItem();

    purple_accounts_remove(item->get_account());

    m_accounts_widget->removeRow(item->row());

    m_edit_btn->setEnabled(false);
    m_delete_btn->setEnabled(false);
}

void
quail_accounts_window::slot_account_selected()
{
    qDebug() << "quail_accounts_window::slot_account_selected()";
    QTableWidgetItem *item = m_accounts_widget->currentItem();
    if (!item)
        return;

    m_edit_btn->setEnabled(true);
    m_delete_btn->setEnabled(true);
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

