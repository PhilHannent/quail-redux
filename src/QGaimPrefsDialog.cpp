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
#include "QGaimTabWidget.h"

#include <qlabel.h>
#include <qlayout.h>

#include <qpe/resource.h>

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

	tabs = new QGaimTabWidget(this, "pref tabs");

	tabs->addTab(buildBuddyListTab(),
				 Resource::loadPixmap("gaim/blist"),
				 tr("Buddy List"));
	tabs->addTab(buildConversationsTab(),
				 Resource::loadPixmap("gaim/conversations"),
				 tr("Conversations"));
	tabs->addTab(buildNotificationTab(),
				 Resource::loadPixmap("gaim/warn"),
				 tr("Notification"));
	tabs->addTab(buildAwayIdleTab(),
				 Resource::loadPixmap("gaim/away"),
				 tr("Away/Idle"));
	tabs->addTab(buildProxyTab(),
				 Resource::loadPixmap("gaim/network"),
				 tr("Proxy"));
	tabs->addTab(buildPluginsTab(),
				 Resource::loadPixmap("gaim/connect"),
				 tr("Plugins"));
}

QWidget *
QGaimPrefsDialog::buildBuddyListTab()
{
	return new QLabel(this, "Buddy List");
}

QWidget *
QGaimPrefsDialog::buildConversationsTab()
{
	return new QLabel(this, "Conversations");
}

QWidget *
QGaimPrefsDialog::buildNotificationTab()
{
	return new QLabel(this, "Notification");
}

QWidget *
QGaimPrefsDialog::buildAwayIdleTab()
{
	return new QLabel(this, "Away/Idle");
}

QWidget *
QGaimPrefsDialog::buildProxyTab()
{
	return new QLabel(this, "Proxy");
}

QWidget *
QGaimPrefsDialog::buildPluginsTab()
{
	return new QLabel(this, "Plugins");
}
