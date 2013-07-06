/**
 * @file QQuailMainWindow.cpp Main gaim window
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
#include "QuailMainWindow.h"

#include <qtimer.h>
#include <qvariant.h>
#include <qvbox.h>
#include <qpe/qpeapplication.h>

#include <libpurple/prefs.h>
#include <libpurple/conversation.h>
#include <libpurple/core.h>
#include <libpurple/proxy.h>
#include <libpurple/sound.h>
#include <libpurple/pounce.h>
#include <libpurple/plugin.h>

#include "QuailConnectionMeter.h"
#include "QuailConvWindow.h"
#include "QuailDebugWindow.h"
#include "QuailEventLoop.h"
#include "QuailNotify.h"
#include "QuailRequest.h"
#include "base.h"

static QQuailMainWindow *mainWin = NULL;

/**************************************************************************
 * Core stuff
 **************************************************************************/
static void
qQuailPrefsInit(void)
{
	purple_prefs_add_none("/gaim");
	purple_prefs_add_none("/gaim/qpe");
	purple_prefs_add_none("/gaim/qpe/blist");
	purple_prefs_add_bool("/gaim/qpe/blist/show_offline_buddies", false);
	purple_prefs_add_bool("/gaim/qpe/blist/show_empty_groups",    false);
	purple_prefs_add_bool("/gaim/qpe/blist/show_idle_times",      true);
	purple_prefs_add_bool("/gaim/qpe/blist/show_warning_levels",  true);
	purple_prefs_add_bool("/gaim/qpe/blist/show_group_count",     true);
	purple_prefs_add_bool("/gaim/qpe/blist/show_large_icons",
						(QPEApplication::desktop()->width() >= 600));
	purple_prefs_add_bool("/gaim/qpe/blist/dim_idle_buddies",     true);

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
	purple_conversations_set_win_ui_ops(qQuailGetConvWindowUiOps());
	purple_notify_set_ui_ops(qQuailGetNotifyUiOps());
	purple_request_set_ui_ops(qQuailGetRequestUiOps());
}

static void
qQuailCoreQuit(void)
{
	exit(0);
}

static GaimCoreUiOps coreOps =
{
	qQuailPrefsInit,
	qQuailCoreDebugInit,
	qQuailCoreUiInit,
	qQuailCoreQuit
};

static GaimCoreUiOps *
qQuailGetCoreUiOps()
{
	return &coreOps;
}


/**************************************************************************
 * QQuailMainWindow
 **************************************************************************/
QQuailMainWindow::QQuailMainWindow(QWidget *parent, const char *name, WFlags fl)
	: QMainWindow(parent, name, fl),
	  accountsWin(NULL), blistWin(NULL), nextConvWinId(0)
{
	mainWin = this;

	buildInterface();

	initCore();

	/* We have to do these separately. Ugh. */
	showBlistWindow();

	purple_set_blist(purple_blist_new());
	purple_blist_load();

	purple_accounts_auto_login("qpe-gaim");
}

QQuailMainWindow::~QQuailMainWindow()
{
	purple_core_quit();
}

void
QQuailMainWindow::buildInterface()
{
	QVBox *vbox = new QVBox(this);

	widgetStack = new QWidgetStack(vbox);
	vbox->setStretchFactor(widgetStack, 1);

	/* Create the connection meters box. */
	meters = new QQuailConnectionMeters(vbox);

	setCentralWidget(vbox);
}

void
QQuailMainWindow::initCore()
{
	char *plugin_search_paths[1];

	purple_core_set_ui_ops(qQuailGetCoreUiOps());
	purple_eventloop_set_ui_ops(qQuailGetEventLoopUiOps());

	if (!purple_core_init("qpe-gaim")) {
		qFatal(tr("Initialization of the Gaim core failed.\n"
				  "Please report this!\n"));
	}

#ifdef LOCAL_COMPILE
	plugin_search_paths[0] = "/opt/Qtopia/lib/gaim";
#else
	plugin_search_paths[0] = "/usr/lib/gaim";
#endif

	purple_plugins_set_search_paths(sizeof(plugin_search_paths) /
								  sizeof(*plugin_search_paths),
								  plugin_search_paths);

	purple_plugins_probe(NULL);

	purple_prefs_load();

	purple_accounts_load();
	purple_pounces_load();
}

void
QQuailMainWindow::closeEvent(QCloseEvent *event)
{
	QWidget *visibleWidget = widgetStack->visibleWidget();

	if (visibleWidget == accountsWin || visibleWidget == blistWin)
		event->accept();
	else
	{
		/* This had better be a conversation window... */

		QQuailConvWindow *qwin = (QQuailConvWindow *)visibleWidget;

		purple_conv_window_destroy(qwin->getGaimConvWindow());

		event->ignore();
	}
}

void
QQuailMainWindow::addConversationWindow(QQuailConvWindow *win)
{
	win->setId(nextConvWinId++);

	getWidgetStack()->addWidget(win, win->getId());
}

void
QQuailMainWindow::removeConversationWindow(QQuailConvWindow *win)
{
	GList *l;
	GaimConvWindow *newWin = NULL;

	l = g_list_find(purple_get_windows(), win->getGaimConvWindow());

	getWidgetStack()->removeWidget(win);

	if (l != NULL)
	{
		if (l->next != NULL)
			newWin = (GaimConvWindow *)l->next->data;
		else if (l->prev != NULL)
			newWin = (GaimConvWindow *)l->prev->data;

		if (newWin != NULL)
		{
			QQuailConvWindow *qwin = (QQuailConvWindow *)newWin->ui_data;

			getWidgetStack()->raiseWidget(qwin);

			return;
		}
	}

	showBlistWindow();
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
QQuailMainWindow::setLastActiveConvWindow(GaimConvWindow *win)
{
	lastConvWin = win;
}

GaimConvWindow *
QQuailMainWindow::getLastActiveConvWindow() const
{
	return lastConvWin;
}

QWidgetStack *
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
		widgetStack->addWidget(blistWin, 0);
	}

	setCaption(tr("Gaim - Buddy List"));
	widgetStack->raiseWidget(blistWin);
}

void
QQuailMainWindow::showAccountsWindow()
{
	if (accountsWin == NULL)
	{
		accountsWin = new QQuailAccountsWindow(this);
		widgetStack->addWidget(accountsWin, 1);
	}

	setCaption(tr("Gaim - Accounts"));
	widgetStack->raiseWidget(accountsWin);
}

QQuailMainWindow *
qQuailGetMainWindow()
{
	return mainWin;
}
