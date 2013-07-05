/**
 * @file QGaimPrefsWindow.cpp Preferences window
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
#include "QuailPrefsDialog.h"
#include "QuailMainWindow.h"

#include <libgaim/debug.h>
#include <libgaim/prefs.h>

#include <qcheckbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qvbox.h>
#include <qvgroupbox.h>

#include <opie/otabwidget.h>
#include <opie/owait.h>

#include <qpe/resource.h>

/**************************************************************************
 * QGaimBlistPrefPage
 **************************************************************************/
QGaimBlistPrefPage::QGaimBlistPrefPage(QWidget *parent, const char *name)
	: QGaimPrefPage(parent, name)
{
	buildInterface();
}

void
QGaimBlistPrefPage::accept()
{
	if (gaim_prefs_get_bool("/gaim/qpe/blist/show_idle_times") ==
		idleTimes->isChecked() &&
		gaim_prefs_get_bool("/gaim/qpe/blist/show_warning_levels") ==
		showWarnings->isChecked() &&
		gaim_prefs_get_bool("/gaim/qpe/blist/show_group_count") ==
		groupCount->isChecked() &&
		gaim_prefs_get_bool("/gaim/qpe/blist/show_large_icons") ==
		largeIcons->isChecked() &&
		gaim_prefs_get_bool("/gaim/qpe/blist/dim_idle_buddies") ==
		dimIdle->isChecked())
	{
		return;
	}

	gaim_prefs_set_bool("/gaim/qpe/blist/show_idle_times",
						idleTimes->isChecked());
	gaim_prefs_set_bool("/gaim/qpe/blist/show_warning_levels",
						showWarnings->isChecked());
	gaim_prefs_set_bool("/gaim/qpe/blist/show_group_count",
						groupCount->isChecked());
	gaim_prefs_set_bool("/gaim/qpe/blist/show_large_icons",
						largeIcons->isChecked());
	gaim_prefs_set_bool("/gaim/qpe/blist/dim_idle_buddies",
						dimIdle->isChecked());

	qGaimGetMainWindow()->getBlistWindow()->reloadList();
}

void
QGaimBlistPrefPage::buildInterface()
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setAutoAdd(true);
	layout->setSpacing(6);
	layout->setMargin(6);

	/* Show large icons */
	largeIcons = new QCheckBox(tr("Show large icons"),
							   this, "large icons checkbox");

	if (gaim_prefs_get_bool("/gaim/qpe/blist/show_large_icons"))
		largeIcons->setChecked(true);

	/* Show idle times */
	idleTimes = new QCheckBox(tr("Show idle times"),
							  this, "idle times checkbox");

	if (gaim_prefs_get_bool("/gaim/qpe/blist/show_idle_times"))
		idleTimes->setChecked(true);

	/* Show warning levels */
	showWarnings = new QCheckBox(tr("Show warning levels"),
								 this, "warning levels checkbox");

	if (gaim_prefs_get_bool("/gaim/qpe/blist/show_warning_levels"))
		showWarnings->setChecked(true);

	/* Show numbers in groups */
	groupCount = new QCheckBox(tr("Show numbers in groups"),
							   this, "group count checkbox");

	if (gaim_prefs_get_bool("/gaim/qpe/blist/show_group_count"))
		groupCount->setChecked(true);

	/* Dim idle buddies */
	dimIdle = new QCheckBox(tr("Dim idle buddies"),
							this, "dim idle checkbox");

	if (gaim_prefs_get_bool("/gaim/qpe/blist/dim_idle_buddies"))
		dimIdle->setChecked(true);

	/* Spacer */
	QLabel *spacer = new QLabel("", this);
	layout->setStretchFactor(spacer, 1);
}

/**************************************************************************
 * QGaimConvPrefPage
 **************************************************************************/
QGaimConvPrefPage::QGaimConvPrefPage(QWidget *parent, const char *name)
	: QGaimPrefPage(parent, name)
{
}

void
QGaimConvPrefPage::accept()
{
}

void
QGaimConvPrefPage::buildInterface()
{
}


/**************************************************************************
 * QGaimNotifyPrefPage
 **************************************************************************/
QGaimNotifyPrefPage::QGaimNotifyPrefPage(QWidget *parent, const char *name)
	: QGaimPrefPage(parent, name)
{
	buildInterface();
}

void
QGaimNotifyPrefPage::accept()
{
	if (gaim_prefs_get_bool("/gaim/qpe/notify/incoming_im") ==
		incomingIm->isChecked() &&
		gaim_prefs_get_bool("/gaim/qpe/notify/incoming_chat") ==
		incomingChat->isChecked() &&
		gaim_prefs_get_bool("/gaim/qpe/notify/use_buzzer") ==
		useBuzzer->isChecked() &&
		gaim_prefs_get_bool("/gaim/qpe/notify/use_led") ==
		useLed->isChecked())
	{
		return;
	}

	gaim_prefs_set_bool("/gaim/qpe/notify/incoming_im",
						incomingIm->isChecked());
	gaim_prefs_set_bool("/gaim/qpe/notify/incoming_chat",
						incomingChat->isChecked());
	gaim_prefs_set_bool("/gaim/qpe/notify/use_buzzer",
						useBuzzer->isChecked());
	gaim_prefs_set_bool("/gaim/qpe/notify/use_led",
						useLed->isChecked());
}

void
QGaimNotifyPrefPage::buildInterface()
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	QVGroupBox *groupBox;

	layout->setAutoAdd(true);

	QVBox *vbox = new QVBox(this);
	vbox->setSpacing(6);
	vbox->setMargin(6);

	/* Notify On groupbox */
	groupBox = new QVGroupBox(tr("Notify On"), vbox);
	/* Incoming IMs */
	incomingIm = new QCheckBox(tr("Incoming IMs"),
							   groupBox, "incoming ims checkbox");

	if (gaim_prefs_get_bool("/gaim/qpe/notify/incoming_im"))
		incomingIm->setChecked(true);

	/* Incoming Chats */
	incomingChat = new QCheckBox(tr("Incoming chat messages"),
								 groupBox, "incoming chat checkbox");

	if (gaim_prefs_get_bool("/gaim/qpe/notify/incoming_chat"))
		incomingChat->setChecked(true);

	/* Notify Using groupbox */
	groupBox = new QVGroupBox(tr("Notify Using"), vbox);

	/* Buzzer */
	useBuzzer = new QCheckBox(tr("Buzzer"), groupBox, "buzzer checkbox");

	if (gaim_prefs_get_bool("/gaim/qpe/notify/use_buzzer"))
		useBuzzer->setChecked(true);

	/* LED */
	useLed = new QCheckBox(tr("LED"), groupBox, "led checkbox");

	if (gaim_prefs_get_bool("/gaim/qpe/notify/use_led"))
		useLed->setChecked(true);

	/* Spacer */
	QLabel *spacer = new QLabel("", vbox);
	vbox->setStretchFactor(spacer, 1);
}


/**************************************************************************
 * QGaimAwayPrefPage
 **************************************************************************/
QGaimAwayPrefPage::QGaimAwayPrefPage(QWidget *parent, const char *name)
	: QGaimPrefPage(parent, name)
{
}

void
QGaimAwayPrefPage::accept()
{
}

void
QGaimAwayPrefPage::buildInterface()
{
}


/**************************************************************************
 * QGaimProxyPrefPage
 **************************************************************************/
QGaimProxyPrefPage::QGaimProxyPrefPage(QWidget *parent, const char *name)
	: QGaimPrefPage(parent, name)
{
}

void
QGaimProxyPrefPage::accept()
{
}

void
QGaimProxyPrefPage::buildInterface()
{
}


/**************************************************************************
 * QGaimPluginPrefPage
 **************************************************************************/
QGaimPluginPrefPage::QGaimPluginPrefPage(QWidget *parent, const char *name)
	: QGaimPrefPage(parent, name)
{
}

void
QGaimPluginPrefPage::accept()
{
}

void
QGaimPluginPrefPage::buildInterface()
{
}


/**************************************************************************
 * QGaimPrefsDialog
 **************************************************************************/
QGaimPrefsDialog::QGaimPrefsDialog(QWidget *parent, const char *name,
								   WFlags fl)
	: QDialog(parent, name, fl)
{
	buildInterface();
}

QGaimPrefsDialog::~QGaimPrefsDialog()
{
}

void
QGaimPrefsDialog::buildInterface()
{
	QVBoxLayout *layout;

	setCaption(tr("Preferences"));

	layout = new QVBoxLayout(this);
	layout->setAutoAdd(true);

	tabs = new OTabWidget(this, "pref tabs");

	blistPage    = new QGaimBlistPrefPage(this,  "blist page");
	notifyPage   = new QGaimNotifyPrefPage(this, "notify page");
#if 0
	convPage     = new QGaimConvPrefPage(this,   "conv page");
	awayIdlePage = new QGaimAwayPrefPage(this,   "awayIdle page");
	proxyPage    = new QGaimProxyPrefPage(this,  "proxy page");
	pluginPage   = new QGaimPluginPrefPage(this, "plugin page");
#endif

	tabs->addTab(blistPage,    "gaim/16x16/blist",         tr("Buddy List"));
	tabs->addTab(notifyPage,   "gaim/16x16/warn",          tr("Notification"));
#if 0
	tabs->addTab(convPage,     "gaim/16x16/conversations", tr("Conversations"));
	tabs->addTab(awayIdlePage, "gaim/16x16/away",          tr("Away/Idle"));
	tabs->addTab(proxyPage,    "gaim/16x16/network",       tr("Proxy"));
	tabs->addTab(pluginPage,   "gaim/16x16/connect",       tr("Plugins"));
#endif

	tabs->setCurrentTab(blistPage);
}

void
QGaimPrefsDialog::accept()
{
	OWait wait(this);

	wait.show();

	blistPage->accept();
	notifyPage->accept();
#if 0
	convPage->accept();
	awayIdlePage->accept();
	proxyPage->accept();
	pluginPage->accept();
#endif

	QDialog::accept();

	wait.hide();
}

void
QGaimPrefsDialog::done(int r)
{
	QDialog::done(r);

	close();
}
