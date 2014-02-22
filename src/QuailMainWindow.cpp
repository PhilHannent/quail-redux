/**
 * @file QQuailMainWindow.cpp Main gaim window
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
#include "QuailMainWindow.h"

#include <QAction>
#include <QApplication>
#include <QComboBox>
#include <QCloseEvent>
#include <QDebug>
#include <QDesktopWidget>
#include <QDir>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QSettings>
#include <QStackedWidget>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QThread>
#include <QToolBar>
#include <QVariant>
#include <QVBoxLayout>

#include <libpurple/prefs.h>
#include <libpurple/conversation.h>
#include <libpurple/core.h>
#include <libpurple/proxy.h>
#include <libpurple/sound.h>
#include <libpurple/pounce.h>
#include <libpurple/plugin.h>

#include "version.h"
#include "global.h"
#include "QuailAccountsWindow.h"
#include "QuailBListWindow.h"
#include "QuailConnectionMeter.h"
#include "QuailConvWindow.h"
#include "QuailDebugWindow.h"
#include "QuailEventLoop.h"
#include "QuailNotify.h"
#include "QuailPrefsDialog.h"
#include "QuailRequest.h"
#include "QuailStatusSelector.h"

static quail_main_window *main_win = 0;
quail_event_loop *event_loop = 0;

/**************************************************************************
 * Core stuff
 **************************************************************************/
static void
qQuailPrefsInit(void)
{
    qDebug() << "QQuailMainWindow::qQuailPrefsInit";
    purple_prefs_add_none("/quail");
    purple_prefs_add_none("/quail/blist");
    purple_prefs_add_bool("/quail/blist/show_offline_buddies", false);
    purple_prefs_add_bool("/quail/blist/show_empty_groups",    false);
    purple_prefs_add_bool("/quail/blist/show_idle_times",      true);
    purple_prefs_add_bool("/quail/blist/show_warning_levels",  true);
    purple_prefs_add_bool("/quail/blist/show_group_count",     true);
    purple_prefs_add_bool("/quail/blist/show_large_icons",
                        (QApplication::desktop()->screenGeometry().width() >= 600));
    purple_prefs_add_bool("/quail/blist/dim_idle_buddies",     true);
    //purple_prefs_add_none("/quail/blist/geometary", 0);
    //purple_prefs_add_none("/quail/blist/state", 0);
	qQuailNotifyInit();

}

static void
qQuailCoreDebugInit(void)
{
	purple_debug_set_ui_ops(qQuailGetDebugUiOps());
}

static void
qQuailCoreUiInit(void)
{
    purple_blist_set_ui_ops(quail_get_blist_ui_ops());
	purple_connections_set_ui_ops(qQuailGetConnectionUiOps());
    purple_conversations_set_ui_ops(qQuailGetConvWindowUiOps());
	purple_notify_set_ui_ops(qQuailGetNotifyUiOps());
    purple_request_set_ui_ops(qQuailGetRequestUiOps());
}

static void
qQuailCoreQuit(void)
{
	exit(0);
}

static PurpleCoreUiOps coreOps =
{
	qQuailPrefsInit,
	qQuailCoreDebugInit,
	qQuailCoreUiInit,
    qQuailCoreQuit,
    NULL, /* get_ui_info */
    NULL,
    NULL,
    NULL
};

static PurpleCoreUiOps *
qQuailGetCoreUiOps()
{
	return &coreOps;
}


/**************************************************************************
 * QQuailMainWindow
 **************************************************************************/
quail_main_window::quail_main_window(QWidget *parent)
    : QMainWindow(parent),
      m_accounts_window(0),
      m_blist_window(0),
      m_conv_window(0),
      lastConvWin(0),
      m_pref_window(0),
      nextConvWinId(0),
      m_language("en")
{
    qDebug() << "QQuailMainWindow";
    main_win = this;
    event_loop = new quail_event_loop();
    setWindowIcon(QIcon(":/data/images/logo.png"));
    QString configPath(QDir::home().path() + "/.quail");
    purple_util_set_user_dir(configPath.toStdString().c_str());
    createActions();
	buildInterface();
    createTrayIcon();
    qDebug() << "QQuailMainWindow::QQuailMainWindow:about to initCore";
	initCore();
    qDebug() << "QQuailMainWindow::QQuailMainWindow:initCore.complete";
	showBlistWindow();
    qDebug() << "QQuailMainWindow::QQuailMainWindow:showBlistWindow()";
	purple_set_blist(purple_blist_new());
	purple_blist_load();

    retranslateUi(this);
    purple_accounts_restore_current_statuses();
    slotReadSettings();
}

quail_main_window::~quail_main_window()
{
	purple_core_quit();
}

void
quail_main_window::buildInterface()
{
    widgetStack = new QStackedWidget(this);

	/* Create the connection meters box. */
    meters = new QQuailConnectionMeters(this);
    widgetStack->addWidget(meters);
//    vbox->addWidget(meters);

    setCentralWidget(widgetStack);
}

void
quail_main_window::createActions()
{
    actShowBuddyList = new QAction(this);
    actShowBuddyList->setIcon(QIcon(":/data/images/actions/blist.png"));
    connect(actShowBuddyList, SIGNAL(triggered()),
            this, SLOT(showBlistWindow()));

    actShowAccounts = new QAction(this);
    actShowAccounts->setIcon(QIcon(":/data/images/actions/accounts.png"));
    connect(actShowAccounts, SIGNAL(triggered()),
            this, SLOT(showAccountsWindow()));

    actMetersAccounts = new QAction(this);
    actMetersAccounts->setText("Show Meters");
    connect(actMetersAccounts, SIGNAL(triggered()),
            this, SLOT(slot_show_meters()));

    actMinimize = new QAction(this);
    connect(actMinimize, SIGNAL(triggered()),
            this, SLOT(hide()));
    actQuit = new QAction(this);
    connect(actQuit, SIGNAL(triggered()),
            this, SLOT(slot_quit()));
}

QMenu*
quail_main_window::createStatusMenu()
{
    /* Fetch a list of possible states */
    QMenu* returnedMenu = new QMenu(this);
    returnedMenu->setTitle(tr("Change Status"));
    QActionGroup* agStates = new QActionGroup(this);
    connect(agStates, SIGNAL(triggered(QAction*)), this, SLOT(slotStateChanges(QAction*)));
    QAction* actStatus;

    actStatus = new QAction(this);
    actStatus->setText(tr("Available"));
    actStatus->setIcon(QIcon(":/data/images/status/available.png"));
    actStatus->setData(PURPLE_STATUS_AVAILABLE);
    agStates->addAction(actStatus);
    returnedMenu->addAction(actStatus);

    actStatus = new QAction(this);
    actStatus->setText(tr("Away"));
    actStatus->setIcon(QIcon(":/data/images/status/away.png"));
    actStatus->setData(PURPLE_STATUS_AWAY);
    agStates->addAction(actStatus);
    returnedMenu->addAction(actStatus);

    actStatus = new QAction(this);
    actStatus->setText(tr("Do not disturb"));
    actStatus->setIcon(QIcon(":/data/images/status/busy.png"));
    actStatus->setData(PURPLE_STATUS_UNAVAILABLE);
    agStates->addAction(actStatus);
    returnedMenu->addAction(actStatus);

    actStatus = new QAction(this);
    actStatus->setText(tr("Invisible"));
    actStatus->setIcon(QIcon(":/data/images/status/invisible.png"));
    actStatus->setData(PURPLE_STATUS_INVISIBLE);
    agStates->addAction(actStatus);
    returnedMenu->addAction(actStatus);

    actStatus = new QAction(this);
    actStatus->setText(tr("Offline"));
    actStatus->setIcon(QIcon(":/data/images/status/offline.png"));
    actStatus->setData(PURPLE_STATUS_OFFLINE);
    agStates->addAction(actStatus);
    returnedMenu->addAction(actStatus);

    //returnedMenu->addSeparator();
    //TODO: Add popular statuses

    returnedMenu->addSeparator();

    actStatus = new QAction(this);
    actStatus->setText(tr("New..."));
    //actStatus->setIcon(QIcon(":/data/images/status/offline.png"));
    actStatus->setData(QUAIL_ACTION_STATUS_NEW);
    agStates->addAction(actStatus);
    returnedMenu->addAction(actStatus);

    actStatus = new QAction(this);
    actStatus->setText(tr("Saved..."));
    //actStatus->setIcon(QIcon(":/data/images/status/offline.png"));
    actStatus->setData(QUAIL_ACTION_STATUS_SAVED);
    agStates->addAction(actStatus);
    returnedMenu->addAction(actStatus);

    return returnedMenu;
}

void
quail_main_window::slotStateChanges(QAction* act)
{
    qDebug() << "QQuailMainWindow::slotStateChanges()";
    switch (act->data().toInt())
    {
    case QUAIL_ACTION_STATUS_NEW:
        break;
    case QUAIL_ACTION_STATUS_SAVED:
        break;
    case PURPLE_STATUS_OFFLINE:
        break;
    case PURPLE_STATUS_INVISIBLE:
        break;
    case PURPLE_STATUS_UNAVAILABLE:
        break;
    case PURPLE_STATUS_AWAY:
        break;
    case PURPLE_STATUS_AVAILABLE:
        //    purple_account_set_status(status_box->account, id,
        //                            TRUE, "message", message, NULL);
        break;
    default:
        /* Must be a custom status */
        break;
    }


}

void
quail_main_window::retranslateUi(QWidget * /*currentForm*/)
{
    qDebug() << "QQuailMainWindow::retranslateUi";
    actShowBuddyList->setText(tr("Show Buddy List"));
    actShowAccounts->setText(tr("Accounts"));
    actMinimize->setText(tr("Hide"));
    actQuit->setText(tr("Quit"));
}

void
quail_main_window::createTrayIcon()
{
    m_tray_icon_menu = new QMenu(this);
    m_tray_icon_menu->addAction(actShowBuddyList);
    m_tray_icon_menu->addAction(actShowAccounts);
    m_tray_icon_menu->addAction(actMinimize);
    m_tray_icon_menu->addSeparator();
    m_status_menu = createStatusMenu();
    m_tray_icon_menu->addMenu(m_status_menu);
    m_tray_icon_menu->addSeparator();
    m_tray_icon_menu->addAction(actQuit);

    m_tray_icon = new QSystemTrayIcon(this);
    m_tray_icon->setContextMenu(m_tray_icon_menu);
    m_tray_icon->setIcon(QIcon(":/data/images/logo.png"));
    m_tray_icon->show();
    connect(m_tray_icon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(slot_activate_tray(QSystemTrayIcon::ActivationReason)));
}

void quail_main_window::slot_activate_tray(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::DoubleClick)
    {
        if (this->isVisible())
            this->hide();
        else
            this->show();
    }
}

void
quail_main_window::initCore()
{
    qDebug() << "QQuailMainWindow::initCore()";
    char *path;

	purple_core_set_ui_ops(qQuailGetCoreUiOps());
    qDebug() << "QQuailMainWindow::initCore().1";
	purple_eventloop_set_ui_ops(qQuailGetEventLoopUiOps());
    qDebug() << "QQuailMainWindow::initCore().2";
    if (!purple_core_init("quail")) {
        qDebug() << tr("Initialization of the Quail core failed.\n"
                  "Please report this!\n");
	}
    purple_debug_set_enabled(true);
    qDebug() << "QQuailMainWindow::initCore().3";
    path = g_build_filename(purple_user_dir(), "plugins", NULL);
    qDebug() << "QQuailMainWindow::initCore().4";
    purple_plugins_add_search_path(path);
    qDebug() << "QQuailMainWindow::initCore().5";
    purple_plugins_probe(NULL);
    qDebug() << "QQuailMainWindow::initCore().6";
	purple_prefs_load();
    qDebug() << "QQuailMainWindow::initCore().7";
    //purple_accounts_load();
	purple_pounces_load();
    qDebug() << "QQuailMainWindow::initCore().end";
}

void
quail_main_window::closeEvent(QCloseEvent *event)
{
    qDebug() << "QQuailMainWindow::closeEvent()";
    QWidget *visibleWidget = widgetStack->currentWidget();

    if (visibleWidget == m_accounts_window || visibleWidget == m_blist_window)
    {
        slotSaveSettings();
        hide();
    }
    else
	{
		/* This had better be a conversation window... */
        //TODO: Fix me
        qWarning() << "QQuailMainWindow::closeEvent()<<<<<<FIX ME";
        //QQuailConvWindow *qwin = (QQuailConvWindow *)visibleWidget;
        //purple_conv_window_destroy(qwin->getConvWindow());
	}
    event->ignore();
}

void quail_main_window::switchLanguage()
{
    //qDebug() << "MainWindow::switchLanguage()";
    qApp->removeTranslator(&appTranslator);
    qApp->removeTranslator(&qtTranslator);
    emit signalLanguageCode(m_language);
    if (m_language != "en") {
        /* After reading the settings the translator should know the language */
        if (QFile::exists(QApplication::applicationDirPath() + QDir::separator()
                          + "translations" + QDir::separator() + "quail_" + m_language + ".qm"))
        {
            appTranslator.load("quail_" + m_language,
                QApplication::applicationDirPath() + QDir::separator()
                        + "translations" + QDir::separator());
            qtTranslator.load("qt_" + m_language, QApplication::applicationDirPath()
                + QDir::separator() + "translations" + QDir::separator());
        }
        qApp->installTranslator(&appTranslator);
        qApp->installTranslator(&qtTranslator);
    }
    retranslateUi(this);
}

void quail_main_window::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LocaleChange) {
        switchLanguage();
    }
}

void
quail_main_window::addConversationWindow(PurpleConversation *conv)
{
    qDebug() << "QQuailMainWindow::addConversationWindow()";
//    PurpleConversationType conv_type = purple_conversation_get_type(conv);
//    //QQuailConvWindow *qwin;
//    QQuailConversation *win;

//    if (conv_type == PURPLE_CONV_TYPE_IM) {
//        win = new QQuailConvIm(conv, this);
//    } else if (conv_type == PURPLE_CONV_TYPE_CHAT) {
//        win = new QQuailConvChat(conv, this);
//    }

//    conv->ui_data = win;
    if (m_conv_window == 0)
    {
        m_conv_window = new quail_conv_window(this);
        getWidgetStack()->addWidget(m_conv_window);
        //qwin = new QQuailConvWindow(win, this);
        //qwin->setId(nextConvWinId++);
    }
    m_conv_window->addConversation(conv);
    getWidgetStack()->setCurrentWidget(m_conv_window);
    qDebug() << "QQuailMainWindow::addConversationWindow().end";
}

void
quail_main_window::removeConversationWindow(quail_conversation */*win*/)
{
    qDebug() << "QQuailMainWindow::removeConversationWindow()";
//    GList *l;
//    QQuailConversation *newWin = NULL;

//    l = g_list_find(purple_get_windows(), win->getConvWindow());

//	getWidgetStack()->removeWidget(win);

//	if (l != NULL)
//	{
//		if (l->next != NULL)
//            newWin = (QQuailConversation *)l->next->data;
//		else if (l->prev != NULL)
//            newWin = (QQuailConversation *)l->prev->data;

//		if (newWin != NULL)
//		{
//            QQuailConversation *qwin = (QQuailConversation *)newWin->ui_data;

//            //getWidgetStack()->setCurrentWidget(qwin);

//			return;
//		}
//	}

	showBlistWindow();
}

/* Ideally I want to save these with using the purple pref functions
 * However they only cover basic types. Need to look at how to put
 * these settings into a purple string */
void
quail_main_window::slotSaveSettings()
{
    QSettings appSettings(APP_NAME, APP_MAJOR_VERSION);
    appSettings.setValue("geometry", saveGeometry());
    appSettings.setValue("state", saveState());
}

void
quail_main_window::slotReadSettings()
{
    QSettings appSettings(APP_NAME, APP_MAJOR_VERSION);
    restoreGeometry(appSettings.value("geometry", saveGeometry()).toByteArray());
    restoreState(appSettings.value("state", saveState()).toByteArray());
}

quail_blist_window *
quail_main_window::getBlistWindow() const
{
    return m_blist_window;
}

quail_accounts_window *
quail_main_window::getAccountsWindow() const
{
    return m_accounts_window;
}

void
quail_main_window::setLastActiveConvWindow(quail_conversation *win)
{
    qDebug() << "QQuailMainWindow::setLastActiveConvWindow()";
    lastConvWin = win;
}

quail_conversation *
quail_main_window::getLastActiveConvWindow() const
{
	return lastConvWin;
}

QStackedWidget *
quail_main_window::getWidgetStack() const
{
	return widgetStack;
}

QQuailConnectionMeters *
quail_main_window::getMeters() const
{
	return meters;
}

void
quail_main_window::showBlistWindow()
{
    if (!m_blist_window)
	{
        m_blist_window = new quail_blist_window(this);
        widgetStack->addWidget(m_blist_window);
	}

    setWindowTitle(tr("Buddy List"));
    widgetStack->setCurrentWidget(m_blist_window);
    showNormal();
    raise();
}

void
quail_main_window::showAccountsWindow()
{
    if (!m_accounts_window)
	{
        m_accounts_window = new quail_accounts_window(this);
        widgetStack->addWidget(m_accounts_window);
	}

    setWindowTitle(tr("Accounts"));
    widgetStack->setCurrentWidget(m_accounts_window);
    showNormal();
    raise();
}

void
quail_main_window::showConvWindow()
{
    if (!m_conv_window)
        return;

    setWindowTitle(tr("Conversations"));
    widgetStack->setCurrentWidget(m_conv_window);
}

void
quail_main_window::showPrefWindow()
{
    qDebug() << "QQuailMainWindow::showPrefWindow()";
    if (!m_pref_window)
    {
        qDebug() << "QQuailMainWindow::showPrefWindow().1";
        m_pref_window = new quail_prefs_dialog(this);
        widgetStack->addWidget(m_pref_window);
    }
    setWindowTitle(tr("Preferences"));
    widgetStack->setCurrentWidget(m_pref_window);
    qDebug() << "QQuailMainWindow::showPrefWindow().end";
}


void
quail_main_window::slot_quit()
{
    m_tray_icon->deleteLater();
    slotSaveSettings();
    qApp->quit();
}

void
quail_main_window::slot_show_meters()
{
    widgetStack->setCurrentWidget(meters);
    setWindowTitle(tr("Connection meters"));
}

quail_main_window *
qQuailGetMainWindow()
{
    return main_win;
}
