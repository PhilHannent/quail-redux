/**
 * @file main.cpp Main file
 *
 * @Copyright (C) 2003 Christian Hammond.
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
#include <qpe/qpeapplication.h>

#include <qvariant.h>
#include <qtimer.h>

#include <libgaim/prefs.h>
#include <libgaim/conversation.h>
#include <libgaim/core.h>
#include <libgaim/proxy.h>
#include <libgaim/sound.h>
#include <libgaim/pounce.h>
#include <libgaim/plugin.h>
#include <glib.h>
#include <unistd.h>
#include <stdio.h>

#include "QGaim.h"
#include "QGaimBListWindow.h"
#include "QGaimConvWindow.h"
#include "QGaimDebugWindow.h"
#include "QGaimNotify.h"
#include "QGaimRequest.h"
#include "base.h"

static QGaim *gaim = NULL;

static void
qGaimPrefsInit(void)
{
	gaim_prefs_add_none("/gaim");
	gaim_prefs_add_none("/gaim/qpe");
	gaim_prefs_add_none("/gaim/qpe/blist");
	gaim_prefs_add_bool("/gaim/qpe/blist/show_offline_buddies", false);
	gaim_prefs_add_bool("/gaim/qpe/blist/show_empty_groups", false);
}

static void
qGaimCoreDebugInit(void)
{
	gaim_set_debug_ui_ops(qGaimGetDebugUiOps());
}

static void
qGaimCoreUiInit(void)
{
	gaim_set_blist_ui_ops(qGaimGetBlistUiOps());
	gaim_set_connection_ui_ops(qGaimGetConnectionUiOps());
	gaim_set_win_ui_ops(qGaimGetConvWindowUiOps());
	gaim_set_notify_ui_ops(qGaimGetNotifyUiOps());
	gaim_set_request_ui_ops(qGaimGetRequestUiOps());
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

QGaim::QGaim(int argc, char **argv)
	: blistWin(NULL), accountsWin(NULL), nextConvWinId(2)
{
	app = new QPEApplication(argc, argv);
}

QGaim::~QGaim()
{
	delete app;
}

void
QGaim::init()
{
	char *plugin_search_paths[1];

	mainWindow = new QMainWindow();
	mainWindow->showMaximized();

	widgetStack = new QWidgetStack(mainWindow);
	mainWindow->setCentralWidget(widgetStack);

	showBlistWindow();

	gaim_set_core_ui_ops(qGaimGetCoreUiOps());

	if (!gaim_core_init("qpe-gaim")) {
		qFatal(tr("Initialization of the Gaim core failed.\n"
				  "Please report this!\n"));
	}

//	plugin_search_paths[0] = "/usr/lib/gaim";
	plugin_search_paths[0] = "/opt/Qtopia/lib/gaim";

	gaim_plugins_set_search_paths(sizeof(plugin_search_paths) /
								  sizeof(*plugin_search_paths),
								  plugin_search_paths);

	gaim_plugins_probe(NULL);

	gaim_prefs_load();

	gaim_accounts_load();
	gaim_pounces_load();

	gaim_set_blist(gaim_blist_new());
	gaim_blist_load();

	/* Setup the timer for the glib context loops */
	QTimer *timer = new QTimer();
	connect(timer, SIGNAL(timeout()),
			this, SLOT(doMainLoop()));

	timer->start(10, FALSE);

	gaim_accounts_auto_login("qpe-gaim");

	app->showMainWidget(mainWindow);
}

int
QGaim::exec()
{
	return app->exec();
}

void
QGaim::addConversationWindow(QGaimConvWindow *win)
{
	win->setId(nextConvWinId++);

	getWidgetStack()->addWidget(win, win->getId());
}

void
QGaim::removeConversationWindow(QGaimConvWindow *win)
{
	GList *l;
	GaimWindow *newWin = NULL;

	l = g_list_find(gaim_get_windows(), win->getGaimWindow());

	getWidgetStack()->removeWidget(win);

	if (l != NULL)
	{
		if (l->next != NULL)
			newWin = (GaimWindow *)l->next->data;
		else if (l->prev != NULL)
			newWin = (GaimWindow *)l->prev->data;

		if (newWin != NULL)
		{
			QGaimConvWindow *qwin = (QGaimConvWindow *)newWin->ui_data;

			getWidgetStack()->raiseWidget(qwin);

			return;
		}
	}

	getWidgetStack()->raiseWidget(0);
	getMainWindow()->setCaption(tr("Gaim - Buddy List"));
}

QGaimBListWindow *
QGaim::getBlistWindow() const
{
	return blistWin;
}

QGaimAccountsWindow *
QGaim::getAccountsWindow() const
{
	return accountsWin;
}

void
QGaim::setLastActiveConvWindow(GaimWindow *win)
{
	lastConvWin = win;
}

GaimWindow *
QGaim::getLastActiveConvWindow() const
{
	return lastConvWin;
}

QWidgetStack *
QGaim::getWidgetStack() const
{
	return widgetStack;
}

QMainWindow *
QGaim::getMainWindow() const
{
	return mainWindow;
}

void
QGaim::showBlistWindow()
{
	if (blistWin == NULL)
	{
		blistWin = new QGaimBListWindow();
		widgetStack->addWidget(blistWin, 0);
	}

	mainWindow->setCaption(tr("Gaim - Buddy List"));
	widgetStack->raiseWidget(0);
}

void
QGaim::showAccountsWindow()
{
	if (accountsWin == NULL)
	{
		accountsWin = new QGaimAccountsWindow();
		widgetStack->addWidget(accountsWin, 1);
	}

	mainWindow->setCaption(tr("Gaim - Accounts"));
	widgetStack->raiseWidget(1);
}

void
QGaim::doMainLoop()
{
	gaim_core_mainloop_iteration();
}

QGaim *
qGaimGetHandle(void)
{
	return gaim;
}

int
main(int argc, char **argv)
{
	int result;

	gaim = new QGaim(argc, argv);
	gaim->init();

	result = gaim->exec();

	delete gaim;

	return result;
}
