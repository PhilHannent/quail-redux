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
#include "base.h"

static QGaim *gaim = NULL;

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
}

static void
qGaimCoreQuit(void)
{
	exit(0);
}

static GaimCoreUiOps coreOps =
{
	NULL,
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
	: blistWin(NULL), accountsWin(NULL)
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

	blistWin = new QGaimBListWindow();

	gaim_set_core_ui_ops(qGaimGetCoreUiOps());

	if (!gaim_core_init("qpe-gaim")) {
		fprintf(stderr,
				"Initialization of the Gaim core failed.\n"
				"Please report this!\n");
		abort();
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

	gaim_set_blist(gaim_blist_new());
	gaim_blist_load();

	/* Setup the timer for the glib context loops */
	QTimer *timer = new QTimer();
	connect(timer, SIGNAL(timeout()),
			this, SLOT(doMainLoop()));

	timer->start(10, FALSE);

	gaim_accounts_auto_login("qpe-gaim");

	app->showMainWidget(blistWin);
}

int
QGaim::exec()
{
	return app->exec();
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

void
QGaim::showBlistWindow()
{
	if (blistWin == NULL)
		blistWin = new QGaimBListWindow();

	blistWin->showMaximized();
	blistWin->raise();
}

void
QGaim::showAccountsWindow()
{
	if (accountsWin == NULL)
		accountsWin = new QGaimAccountsWindow();

	accountsWin->showMaximized();
	accountsWin->raise();
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
