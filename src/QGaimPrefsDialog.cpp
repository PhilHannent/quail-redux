/**
 * @file QGaimPrefsWindow.cpp Preferences window
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
#include "QGaimPrefsDialog.h"
#include "QGaimMainWindow.h"

#include <libgaim/debug.h>
#include <libgaim/prefs.h>

#include <qcheckbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qvbox.h>

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
	bool dirty;

	if (gaim_prefs_get_bool("/gaim/qpe/blist/show_idle_times") !=
		idleTimes->isChecked() ||
		gaim_prefs_get_bool("/gaim/qpe/blist/show_group_count") !=
		groupCount->isChecked() ||
		gaim_prefs_get_bool("/gaim/qpe/blist/dim_idle_buddies") !=
		dimIdle->isChecked())
	{
		dirty = true;
	}

	gaim_prefs_set_bool("/gaim/qpe/blist/show_idle_times",
						idleTimes->isChecked());
	gaim_prefs_set_bool("/gaim/qpe/blist/show_group_count",
						groupCount->isChecked());
	gaim_prefs_set_bool("/gaim/qpe/blist/dim_idle_buddies",
						dimIdle->isChecked());

	if (dirty)
		qGaimGetMainWindow()->getBlistWindow()->reloadList();
}

void
QGaimBlistPrefPage::buildInterface()
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setAutoAdd(true);
	layout->setSpacing(6);
	layout->setMargin(6);

	/* Show idle times */
	idleTimes = new QCheckBox(tr("Show idle times"),
							  this, "idle times checkbox");

	if (gaim_prefs_get_bool("/gaim/qpe/blist/show_idle_times"))
		idleTimes->setChecked(true);

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
}

void
QGaimNotifyPrefPage::accept()
{
}

void
QGaimNotifyPrefPage::buildInterface()
{
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
	convPage     = new QGaimConvPrefPage(this,   "conv page");
	notifyPage   = new QGaimNotifyPrefPage(this, "notify page");
	awayIdlePage = new QGaimAwayPrefPage(this,   "awayIdle page");
	proxyPage    = new QGaimProxyPrefPage(this,  "proxy page");
	pluginPage   = new QGaimPluginPrefPage(this, "plugin page");

	tabs->addTab(blistPage,    "gaim/16x16/blist",         tr("Buddy List"));
	tabs->addTab(convPage,     "gaim/16x16/conversations", tr("Conversations"));
	tabs->addTab(notifyPage,   "gaim/16x16/warn",          tr("Notification"));
	tabs->addTab(awayIdlePage, "gaim/16x16/away",          tr("Away/Idle"));
	tabs->addTab(proxyPage,    "gaim/16x16/network",       tr("Proxy"));
	tabs->addTab(pluginPage,   "gaim/16x16/connect",       tr("Plugins"));

	tabs->setCurrentTab(blistPage);
}

void
QGaimPrefsDialog::accept()
{
	OWait wait(this);

	wait.show();

	blistPage->accept();
	convPage->accept();
	notifyPage->accept();
	awayIdlePage->accept();
	proxyPage->accept();
	pluginPage->accept();

	QDialog::accept();

	wait.hide();
}

void
QGaimPrefsDialog::done(int r)
{
	QDialog::done(r);

	close();
}
