/**
 * @file QGaimMainWindow.cpp Main gaim window
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

static QGaimMainWindow *mainWin = NULL;

/**************************************************************************
 * Core stuff
 **************************************************************************/
static void
qGaimPrefsInit(void)
{
	gaim_prefs_add_none("/gaim");
	gaim_prefs_add_none("/gaim/qpe");
	gaim_prefs_add_none("/gaim/qpe/blist");
	gaim_prefs_add_bool("/gaim/qpe/blist/show_offline_buddies", false);
	gaim_prefs_add_bool("/gaim/qpe/blist/show_empty_groups",    false);
	gaim_prefs_add_bool("/gaim/qpe/blist/show_idle_times",      true);
	gaim_prefs_add_bool("/gaim/qpe/blist/show_warning_levels",  true);
	gaim_prefs_add_bool("/gaim/qpe/blist/show_group_count",     true);
	gaim_prefs_add_bool("/gaim/qpe/blist/show_large_icons",
						(QPEApplication::desktop()->width() >= 600));
	gaim_prefs_add_bool("/gaim/qpe/blist/dim_idle_buddies",     true);

	qGaimNotifyInit();
}

static void
qGaimCoreDebugInit(void)
{
	gaim_debug_set_ui_ops(qGaimGetDebugUiOps());
}

static void
qGaimCoreUiInit(void)
{
	gaim_blist_set_ui_ops(qGaimGetBlistUiOps());
	gaim_connections_set_ui_ops(qGaimGetConnectionUiOps());
	gaim_conversations_set_win_ui_ops(qGaimGetConvWindowUiOps());
	gaim_notify_set_ui_ops(qGaimGetNotifyUiOps());
	gaim_request_set_ui_ops(qGaimGetRequestUiOps());
}

static void
qGaimCoreQuit(void)
{
	exit(0);
}

static GaimCoreUiOps coreOps =
{
	qGaimPrefsInit,
	qGaimCoreDebugInit,
	qGaimCoreUiInit,
	qGaimCoreQuit
};

static GaimCoreUiOps *
qGaimGetCoreUiOps()
{
	return &coreOps;
}


/**************************************************************************
 * QGaimMainWindow
 **************************************************************************/
QGaimMainWindow::QGaimMainWindow(QWidget *parent, const char *name, WFlags fl)
	: QMainWindow(parent, name, fl),
	  accountsWin(NULL), blistWin(NULL), nextConvWinId(0)
{
	mainWin = this;

	buildInterface();

	initCore();

	/* We have to do these separately. Ugh. */
	showBlistWindow();

	gaim_set_blist(gaim_blist_new());
	gaim_blist_load();

	gaim_accounts_auto_login("qpe-gaim");
}

QGaimMainWindow::~QGaimMainWindow()
{
	gaim_core_quit();
}

void
QGaimMainWindow::buildInterface()
{
	QVBox *vbox = new QVBox(this);

	widgetStack = new QWidgetStack(vbox);
	vbox->setStretchFactor(widgetStack, 1);

	/* Create the connection meters box. */
	meters = new QGaimConnectionMeters(vbox);

	setCentralWidget(vbox);
}

void
QGaimMainWindow::initCore()
{
	char *plugin_search_paths[1];

	gaim_core_set_ui_ops(qGaimGetCoreUiOps());
	gaim_eventloop_set_ui_ops(qGaimGetEventLoopUiOps());

	if (!gaim_core_init("qpe-gaim")) {
		qFatal(tr("Initialization of the Gaim core failed.\n"
				  "Please report this!\n"));
	}

#ifdef LOCAL_COMPILE
	plugin_search_paths[0] = "/opt/Qtopia/lib/gaim";
#else
	plugin_search_paths[0] = "/usr/lib/gaim";
#endif

	gaim_plugins_set_search_paths(sizeof(plugin_search_paths) /
								  sizeof(*plugin_search_paths),
								  plugin_search_paths);

	gaim_plugins_probe(NULL);

	gaim_prefs_load();

	gaim_accounts_load();
	gaim_pounces_load();
}

void
QGaimMainWindow::closeEvent(QCloseEvent *event)
{
	QWidget *visibleWidget = widgetStack->visibleWidget();

	if (visibleWidget == accountsWin || visibleWidget == blistWin)
		event->accept();
	else
	{
		/* This had better be a conversation window... */

		QGaimConvWindow *qwin = (QGaimConvWindow *)visibleWidget;

		gaim_conv_window_destroy(qwin->getGaimConvWindow());

		event->ignore();
	}
}

void
QGaimMainWindow::addConversationWindow(QGaimConvWindow *win)
{
	win->setId(nextConvWinId++);

	getWidgetStack()->addWidget(win, win->getId());
}

void
QGaimMainWindow::removeConversationWindow(QGaimConvWindow *win)
{
	GList *l;
	GaimConvWindow *newWin = NULL;

	l = g_list_find(gaim_get_windows(), win->getGaimConvWindow());

	getWidgetStack()->removeWidget(win);

	if (l != NULL)
	{
		if (l->next != NULL)
			newWin = (GaimConvWindow *)l->next->data;
		else if (l->prev != NULL)
			newWin = (GaimConvWindow *)l->prev->data;

		if (newWin != NULL)
		{
			QGaimConvWindow *qwin = (QGaimConvWindow *)newWin->ui_data;

			getWidgetStack()->raiseWidget(qwin);

			return;
		}
	}

	showBlistWindow();
}

QGaimBListWindow *
QGaimMainWindow::getBlistWindow() const
{
	return blistWin;
}

QGaimAccountsWindow *
QGaimMainWindow::getAccountsWindow() const
{
	return accountsWin;
}

void
QGaimMainWindow::setLastActiveConvWindow(GaimConvWindow *win)
{
	lastConvWin = win;
}

GaimConvWindow *
QGaimMainWindow::getLastActiveConvWindow() const
{
	return lastConvWin;
}

QWidgetStack *
QGaimMainWindow::getWidgetStack() const
{
	return widgetStack;
}

QGaimConnectionMeters *
QGaimMainWindow::getMeters() const
{
	return meters;
}

void
QGaimMainWindow::showBlistWindow()
{
	if (blistWin == NULL)
	{
		blistWin = new QGaimBListWindow(this);
		widgetStack->addWidget(blistWin, 0);
	}

	setCaption(tr("Gaim - Buddy List"));
	widgetStack->raiseWidget(blistWin);
}

void
QGaimMainWindow::showAccountsWindow()
{
	if (accountsWin == NULL)
	{
		accountsWin = new QGaimAccountsWindow(this);
		widgetStack->addWidget(accountsWin, 1);
	}

	setCaption(tr("Gaim - Accounts"));
	widgetStack->raiseWidget(accountsWin);
}

QGaimMainWindow *
qGaimGetMainWindow()
{
	return mainWin;
}
