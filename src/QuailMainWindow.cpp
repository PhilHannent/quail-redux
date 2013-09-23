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

static QQuailMainWindow *mainWin = 0;

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
	purple_blist_set_ui_ops(qQuailGetBlistUiOps());
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
QQuailMainWindow::QQuailMainWindow(QWidget *parent)
    : QMainWindow(parent),
      accountsWin(NULL),
      blistWin(NULL),
      convWin(0),
      lastConvWin(0),
      prefWin(0),
      nextConvWinId(0),
      m_language("en")
{
    qDebug() << "QQuailMainWindow";
	mainWin = this;
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

QQuailMainWindow::~QQuailMainWindow()
{
	purple_core_quit();
}

void
QQuailMainWindow::buildInterface()
{
    widgetStack = new QStackedWidget(this);

	/* Create the connection meters box. */
    meters = new QQuailConnectionMeters(this);
    widgetStack->addWidget(meters);
//    vbox->addWidget(meters);

    setCentralWidget(widgetStack);
}

void
QQuailMainWindow::createActions()
{
    actShowBuddyList = new QAction(this);
    actShowBuddyList->setIcon(QIcon(":/data/images/actions/blist.png"));
    connect(actShowBuddyList, SIGNAL(triggered()),
            this, SLOT(showBlistWindow()));

    actShowAccounts = new QAction(this);
    actShowAccounts->setIcon(QIcon(":/data/images/actions/accounts.png"));
    connect(actShowAccounts, SIGNAL(triggered()),
            this, SLOT(showAccountsWindow()));

    actMinimize = new QAction(this);
    connect(actMinimize, SIGNAL(triggered()),
            this, SLOT(hide()));
    actQuit = new QAction(this);
    connect(actQuit, SIGNAL(triggered()),
            qApp, SLOT(quit()));
}

QMenu*
QQuailMainWindow::createStatusMenu()
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
QQuailMainWindow::slotStateChanges(QAction* act)
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
QQuailMainWindow::retranslateUi(QWidget * /*currentForm*/)
{
    qDebug() << "QQuailMainWindow::retranslateUi";
    actShowBuddyList->setText(tr("Show Buddy List"));
    actShowAccounts->setText(tr("Accounts"));
    actMinimize->setText(tr("Hide"));
    actQuit->setText(tr("Quit"));
}

void
QQuailMainWindow::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(actShowBuddyList);
    trayIconMenu->addAction(actShowAccounts);
    trayIconMenu->addAction(actMinimize);
    trayIconMenu->addSeparator();
    m_statusMenu = createStatusMenu();
    trayIconMenu->addMenu(m_statusMenu);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(actQuit);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(QIcon(":/data/images/logo.png"));
    trayIcon->show();
}

void
QQuailMainWindow::initCore()
{
    qDebug() << "QQuailMainWindow::initCore()";
    char *path;

	purple_core_set_ui_ops(qQuailGetCoreUiOps());
    qDebug() << "QQuailMainWindow::initCore().1";
	purple_eventloop_set_ui_ops(qQuailGetEventLoopUiOps());
    QApplication::processEvents();
    qDebug() << "QQuailMainWindow::initCore().2";
    if (!purple_core_init("quail")) {
        qDebug() << tr("Initialization of the Quail core failed.\n"
                  "Please report this!\n");
	}
    QApplication::processEvents();
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
QQuailMainWindow::closeEvent(QCloseEvent *event)
{
    qDebug() << "QQuailMainWindow::closeEvent()";
    QWidget *visibleWidget = widgetStack->currentWidget();

    if (visibleWidget == accountsWin || visibleWidget == blistWin)
    {
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

void QQuailMainWindow::switchLanguage()
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

void QQuailMainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LocaleChange) {
        switchLanguage();
    }
}

void
QQuailMainWindow::addConversationWindow(PurpleConversation *conv)
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
    if (convWin == 0)
    {
        convWin = new QQuailConvWindow(this);
        getWidgetStack()->addWidget(convWin);
        //qwin = new QQuailConvWindow(win, this);
        //qwin->setId(nextConvWinId++);
    }
    convWin->addConversation(conv);
    getWidgetStack()->setCurrentWidget(convWin);
    qDebug() << "QQuailMainWindow::addConversationWindow().end";
}

void
QQuailMainWindow::removeConversationWindow(QQuailConversation */*win*/)
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
QQuailMainWindow::slotSaveSettings()
{
    QSettings appSettings(APP_NAME, APP_MAJOR_VERSION);
    appSettings.setValue("geometry", saveGeometry());
    appSettings.setValue("state", saveState());
}

void
QQuailMainWindow::slotReadSettings()
{
    QSettings appSettings(APP_NAME, APP_MAJOR_VERSION);
    restoreGeometry(appSettings.value("geometry", saveGeometry()).toByteArray());
    restoreState(appSettings.value("state", saveState()).toByteArray());
}

QQuailBListWindow *
QQuailMainWindow::getBlistWindow() const
{
    return blistWin;
}

QQuailAccountsWindow *
QQuailMainWindow::getAccountsWindow() const
{
    return accountsWin;
}

void
QQuailMainWindow::setLastActiveConvWindow(QQuailConversation *win)
{
    qDebug() << "QQuailMainWindow::setLastActiveConvWindow()";
    lastConvWin = win;
}

QQuailConversation *
QQuailMainWindow::getLastActiveConvWindow() const
{
	return lastConvWin;
}

QStackedWidget *
QQuailMainWindow::getWidgetStack() const
{
	return widgetStack;
}

QQuailConnectionMeters *
QQuailMainWindow::getMeters() const
{
	return meters;
}

void
QQuailMainWindow::showBlistWindow()
{
    if (blistWin == NULL)
	{
		blistWin = new QQuailBListWindow(this);
        widgetStack->addWidget(blistWin);
	}

    setWindowTitle(tr("Buddy List"));
    widgetStack->setCurrentWidget(blistWin);
    showNormal();
    raise();
}

void
QQuailMainWindow::showAccountsWindow()
{
    if (accountsWin == NULL)
	{
		accountsWin = new QQuailAccountsWindow(this);
        widgetStack->addWidget(accountsWin);
	}

    setWindowTitle(tr("Accounts"));
    widgetStack->setCurrentWidget(accountsWin);
    showNormal();
    raise();
}

void
QQuailMainWindow::showConvWindow()
{
    if (convWin == 0)
        return;

    setWindowTitle(tr("Conversations"));
    widgetStack->setCurrentWidget(convWin);
}

void
QQuailMainWindow::showPrefWindow()
{
    qDebug() << "QQuailMainWindow::showPrefWindow()";
    if (prefWin == 0)
    {
        qDebug() << "QQuailMainWindow::showPrefWindow().1";
        prefWin = new QQuailPrefsDialog(this);
        widgetStack->addWidget(prefWin);
    }
    setWindowTitle(tr("Preferences"));
    widgetStack->setCurrentWidget(prefWin);
    qDebug() << "QQuailMainWindow::showPrefWindow().end";
}

//void
//QQuailMainWindow::saveSettings()
//{
//    saveGeometry();
//    saveState();
//}

QQuailMainWindow *
qQuailGetMainWindow()
{
	return mainWin;
}
