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
#include <QCloseEvent>
#include <QDebug>
#include <QDesktopWidget>
#include <QMenu>
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

#include "QuailConnectionMeter.h"
#include "QuailConvButton.h"
#include "QuailConvWindow.h"
#include "QuailDebugWindow.h"
#include "QuailEventLoop.h"
#include "QuailNotify.h"
#include "QuailRequest.h"

static QQuailMainWindow *mainWin = NULL;

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

	qQuailNotifyInit();
}

static void
qQuailCoreDebugInit(void)
{
    qDebug() << "QQuailMainWindow::qQuailCoreDebugInit";
	purple_debug_set_ui_ops(qQuailGetDebugUiOps());
}

static void
qQuailCoreUiInit(void)
{
    qDebug() << "QQuailMainWindow::qQuailCoreUiInit";
	purple_blist_set_ui_ops(qQuailGetBlistUiOps());
	purple_connections_set_ui_ops(qQuailGetConnectionUiOps());
    //purple_conversations_set_win_ui_ops(qQuailGetConvWindowUiOps());
	purple_notify_set_ui_ops(qQuailGetNotifyUiOps());
    //purple_request_set_ui_ops(qQuailGetRequestUiOps());
}

static void
qQuailCoreQuit(void)
{
    qDebug() << "QQuailMainWindow::qQuailCoreQuit";
	exit(0);
}

static PurpleCoreUiOps coreOps =
{
	qQuailPrefsInit,
	qQuailCoreDebugInit,
	qQuailCoreUiInit,
	qQuailCoreQuit
};

static PurpleCoreUiOps *
qQuailGetCoreUiOps()
{
	return &coreOps;
}


/**************************************************************************
 * QQuailMainWindow
 **************************************************************************/
QQuailMainWindow::QQuailMainWindow(QWidget *parent, const char *name, Qt::WindowFlags fl)
    : QMainWindow(parent),
	  accountsWin(NULL), blistWin(NULL), nextConvWinId(0)
{
    qDebug() << "QQuailMainWindow";
	mainWin = this;

	buildInterface();

	initCore();

	/* We have to do these separately. Ugh. */
	showBlistWindow();

	purple_set_blist(purple_blist_new());
	purple_blist_load();

    //purple_accounts_auto_login("quail");
}

QQuailMainWindow::~QQuailMainWindow()
{
	purple_core_quit();
}

void
QQuailMainWindow::buildInterface()
{
    qDebug() << "QQuailMainWindow::buildInterface()";
    QVBoxLayout *vbox = new QVBoxLayout(this);
    qDebug() << "QQuailMainWindow::buildInterface().1";
    toolbar = new QToolBar(this);
    this->addToolBar(toolbar);

    widgetStack = new QStackedWidget(this);
    vbox->addWidget(widgetStack);
	vbox->setStretchFactor(widgetStack, 1);

	/* Create the connection meters box. */
    meters = new QQuailConnectionMeters();
    updateGlobalToolBar();
    vbox->addWidget(meters);

    setCentralWidget(widgetStack);
}

void
QQuailMainWindow::updateGlobalToolBar()
{
    qDebug() << "QQuailMainWindow::addGlobalToolBar()";
    QAction *a;
    QToolButton *button;
    toolbar->setMovable(true);
    /* IM */
    imButton = new QAction(QIcon(QPixmap(":/data/images/actions/new-im.png")),
                    tr("Send IM"),
                    this);
    toolbar->addAction(imButton);
    imButton->setEnabled(true);
    connect(imButton, SIGNAL(triggered(bool)),
            this, SIGNAL(signalImButton(bool)));
    qDebug() << "QQuailMainWindow::addGlobalToolBar().1";
    /* Chat */
    chatButton = new QAction(QIcon(QPixmap(":/data/images/actions/new-chat.png")),
                             tr("Open Chat"),
                             this);
    toolbar->addAction(chatButton);
    chatButton->setEnabled(true);
    connect(chatButton, SIGNAL(triggered(bool)),
            this, SIGNAL(signalChatButton(bool)));

    qDebug() << "QQuailMainWindow::addGlobalToolBar().2";
    toolbar->addSeparator();

    /* Add */
    button = new QToolButton(toolbar);
    button->setAutoRaise(true);
    button->setIcon(QIcon(QPixmap(":/data/images/actions/add.png")));
    button->setEnabled(true);
    button->setText(tr("Add..."));
    addButton = button;

    addMenu = new QMenu(button);
    button->setMenu(addMenu);
    toolbar->addWidget(addButton);

    /* Add Buddy */
    addBuddyButton = new QAction(QIcon(QPixmap(":/data/images/actions/user.png")),
                                 tr("Add Buddy"),
                                 this);
    addMenu->addAction(addBuddyButton);
    connect(addBuddyButton, SIGNAL(triggered(bool)),
            this, SIGNAL(signalShowAddBuddy(bool)));
    qDebug() << "QQuailMainWindow::addGlobalToolBar().3";

    /* Add Chat */
    addChatButton = new QAction(QIcon(QPixmap(":/data/images/actions/new-chat.png")),
                                tr("Add Chat"),
                                this);
    addMenu->addAction(addChatButton);
    connect(addChatButton, SIGNAL(triggered(bool)),
            this, SIGNAL(signalShowAddChat(bool)));
    qDebug() << "QQuailMainWindow::addGlobalToolBar().4";

    /* Add Group */
    addGroupButton = new QAction(QIcon(QPixmap(":/data/images/actions/new-group.png")),
                                 tr("Add Group"),
                                 this);
    addMenu->addAction(addGroupButton);
    connect(addGroupButton, SIGNAL(triggered(bool)),
            this, SIGNAL(signalShowAddGroup(bool)));
    qDebug() << "QQuailMainWindow::addGlobalToolBar().5";

    /* Remove */
    removeButton = new QAction(QIcon(QPixmap(":/data/images/actions/remove.png")),
                               tr("Remove"),
                               this);
    toolbar->addAction(removeButton);
    removeButton->setEnabled(true);
    connect(removeButton, SIGNAL(triggered(bool)),
            this, SIGNAL(signalShowRemoveBuddy(bool)));
    qDebug() << "QQuailMainWindow::addGlobalToolBar().6";

    toolbar->addSeparator();

    /* Settings menu */
    button = new QToolButton(this);
    toolbar->addWidget(button);
    button->setAutoRaise(true);
    button->setIcon(QIcon(QPixmap(":/data/images/actions/settings.png")));
    button->setText(tr("Settings"));

    settingsMenu = new QMenu(this);
    button->setMenu(settingsMenu);

    /* Show Offline Buddies */
    showOfflineButton = new QAction(QIcon(QPixmap(":/data/images/actions/offline_buddies.png")),
                                    tr("Show Offline Buddies"),
                                    this);
    showOfflineButton->setChecked(purple_prefs_get_bool("/quail/blist/show_offline_buddies"));
    settingsMenu->addAction(showOfflineButton);
    connect(showOfflineButton, SIGNAL(toggled(bool)),
            this, SIGNAL(signalShowOfflineBuddys(bool)));
    qDebug() << "QQuailMainWindow::addGlobalToolBar().7";

    /* Separator */
    settingsMenu->addSeparator();

    /* Preferences */
    a = new QAction(tr("Preferences"), this);
    settingsMenu->addAction(a);
    connect(a, SIGNAL(triggered(bool)),
            this, SIGNAL(signalShowPrefs(bool)));
    qDebug() << "QQuailMainWindow::addGlobalToolBar().8";

    /* Now we're going to construct the toolbar on the right. */
    //toolbar->addSeparator();

    /* Buddy List */
    blistButton = new QAction(QIcon(QPixmap(":/data/images/actions/blist.png")),
                              tr("Buddy List"),
                              this);
    blistButton->setChecked(true);
    toolbar->addAction(blistButton);
    connect(blistButton, SIGNAL(toggled(bool)),
            this, SIGNAL(signalShowBuddyList(bool)));
    qDebug() << "QQuailMainWindow::addGlobalToolBar().9";

    /* Accounts */
    a = new QAction(QIcon(QPixmap(":/data/images/actions/accounts.png")),
                    tr("Accounts"),
                    this);
    toolbar->addAction(a);
    connect(a, SIGNAL(triggered(bool)),
            this, SIGNAL(signalShowAccounts(bool)));
    qDebug() << "QQuailMainWindow::addGlobalToolBar().10";

    /* Conversations */
    button = new QQuailConvButton(this);
    toolbar->addWidget(button);

    this->addToolBar(toolbar);
    qDebug() << "QQuailMainWindow::addGlobalToolBar().end";

}

void
QQuailMainWindow::initCore()
{
    qDebug() << "QQuailMainWindow::initCore()";
	char *plugin_search_paths[1];

	purple_core_set_ui_ops(qQuailGetCoreUiOps());
	purple_eventloop_set_ui_ops(qQuailGetEventLoopUiOps());

    if (!purple_core_init("quail")) {
        qDebug() << tr("Initialization of the Quail core failed.\n"
                  "Please report this!\n");
	}

#ifdef LOCAL_COMPILE
	plugin_search_paths[0] = "/opt/Qtopia/lib/gaim";
#else
    plugin_search_paths[0] = "/usr/lib/libpurple";
#endif

//	purple_plugins_set_search_paths(sizeof(plugin_search_paths) /
//								  sizeof(*plugin_search_paths),
//								  plugin_search_paths);

	purple_plugins_probe(NULL);

	purple_prefs_load();

    //purple_accounts_load();
	purple_pounces_load();
}

void
QQuailMainWindow::closeEvent(QCloseEvent *event)
{
    QWidget *visibleWidget = widgetStack->currentWidget();

	if (visibleWidget == accountsWin || visibleWidget == blistWin)
		event->accept();
	else
	{
		/* This had better be a conversation window... */

		QQuailConvWindow *qwin = (QQuailConvWindow *)visibleWidget;

        //purple_conv_window_destroy(qwin->getConvWindow());

		event->ignore();
	}
}

void
QQuailMainWindow::addConversationWindow(PurpleConversation *conv)
{
    qDebug() << "QQuailMainWindow::addConversationWindow()";
    PurpleConversationType conv_type = purple_conversation_get_type(conv);
    QQuailConversation *win;

    if (conv_type == PURPLE_CONV_TYPE_IM) {
        win = new QQuailConvIm(conv, this);
    } else if (conv_type == PURPLE_CONV_TYPE_CHAT) {
        win = new QQuailConvChat(conv, this);
    }

    conv->ui_data = win;
    //win->setId(nextConvWinId++);

    getWidgetStack()->addWidget(win);
}

void
QQuailMainWindow::removeConversationWindow(QQuailConversation *win)
{
    qDebug() << "QQuailMainWindow::removeConversationWindow()";
    GList *l;
    QQuailConversation *newWin = NULL;

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

QQuailBListWindow *
QQuailMainWindow::getBlistWindow() const
{
    qDebug() << "QQuailMainWindow::getBlistWindow()";
    return blistWin;
}

QQuailAccountsWindow *
QQuailMainWindow::getAccountsWindow() const
{
    qDebug() << "QQuailMainWindow::getAccountsWindow()";
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
    qDebug() << "QQuailMainWindow::showBlistWindow()";
    if (blistWin == NULL)
	{
		blistWin = new QQuailBListWindow(this);
        widgetStack->addWidget(blistWin);
	}

    setWindowTitle(tr("Buddy List"));
    widgetStack->setCurrentWidget(blistWin);
}

void
QQuailMainWindow::showAccountsWindow()
{
    qDebug() << "QQuailMainWindow::showAccountsWindow()";
    if (accountsWin == NULL)
	{
		accountsWin = new QQuailAccountsWindow(this);
        widgetStack->addWidget(accountsWin);
	}

    setWindowTitle(tr("Accounts"));
    widgetStack->setCurrentWidget(accountsWin);
}

QQuailMainWindow *
qQuailGetMainWindow()
{
	return mainWin;
}
