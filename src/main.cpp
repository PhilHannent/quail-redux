#include <qpe/qpeapplication.h>

#include <qvariant.h>
#include <qtimer.h>

#include <libgaim/prefs.h>
#include <libgaim/conversation.h>
#include <libgaim/proxy.h>
#include <libgaim/sound.h>
#include <libgaim/pounce.h>
#include <libgaim/plugin.h>
#include <glib.h>
#include <unistd.h>

#include "QGaim.h"
#include "QGaimBListWindow.h"
#include "QGaimConvWindow.h"
#include "QGaimDebugWindow.h"

static QGaim *gaim = NULL;

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

	gaim_set_debug_ui_ops(qGaimGetDebugUiOps());
	gaim_set_blist_ui_ops(qGaimGetBlistUiOps());
	gaim_set_win_ui_ops(qGaimGetConvWindowUiOps());

	gaim_prefs_init();
	gaim_conversation_init();
	gaim_proxy_init();
	gaim_sound_init();
	gaim_pounces_init();

	plugin_search_paths[0] = "/home/chipx86/cvs/libgaim-root/lib/gaim";

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
			this, SLOT(doGlibLoop()));

	timer->start(10, FALSE);

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
QGaim::doGlibLoop()
{
	GMainContext *ctx = g_main_context_default();
	g_main_context_iteration(ctx, FALSE);
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
