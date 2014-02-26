/**
 * @file QQuailPrefsWindow.cpp Preferences window
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
#include "QuailPrefsDialog.h"
#include "QuailBListWindow.h"
#include "QuailMainWindow.h"

#include <libpurple/debug.h>
#include <libpurple/prefs.h>

#include <QCheckBox>
#include <QDebug>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QTabWidget>
#include <QVBoxLayout>

quail_blist_pref_page::quail_blist_pref_page(QWidget *parent)
    : quail_pref_page(parent)
{
	buildInterface();
}

void
quail_blist_pref_page::accept()
{
    if (purple_prefs_get_bool("/quail/blist/show_idle_times") ==
		idleTimes->isChecked() &&
        purple_prefs_get_bool("/quail/blist/show_warning_levels") ==
		showWarnings->isChecked() &&
        purple_prefs_get_bool("/quail/blist/show_group_count") ==
		groupCount->isChecked() &&
        purple_prefs_get_bool("/quail/blist/show_large_icons") ==
		largeIcons->isChecked() &&
        purple_prefs_get_bool("/quail/blist/dim_idle_buddies") ==
		dimIdle->isChecked())
	{
		return;
	}

    purple_prefs_set_bool("/quail/blist/show_idle_times",
						idleTimes->isChecked());
    purple_prefs_set_bool("/quail/blist/show_warning_levels",
						showWarnings->isChecked());
    purple_prefs_set_bool("/quail/blist/show_group_count",
						groupCount->isChecked());
    purple_prefs_set_bool("/quail/blist/show_large_icons",
						largeIcons->isChecked());
    purple_prefs_set_bool("/quail/blist/dim_idle_buddies",
						dimIdle->isChecked());

	qQuailGetMainWindow()->getBlistWindow()->reloadList();
}

void
quail_blist_pref_page::buildInterface()
{
    qDebug() << "QQuailBlistPrefPage::buildInterface()";
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setSpacing(6);
	layout->setMargin(6);

	/* Show large icons */
    largeIcons = new QCheckBox(tr("Show large icons"), this);
    layout->addWidget(largeIcons);
    if (purple_prefs_get_bool("/quail/blist/show_large_icons"))
		largeIcons->setChecked(true);

	/* Show idle times */
    idleTimes = new QCheckBox(tr("Show idle times"), this);
    layout->addWidget(idleTimes);
    if (purple_prefs_get_bool("/quail/blist/show_idle_times"))
		idleTimes->setChecked(true);

	/* Show warning levels */
    showWarnings = new QCheckBox(tr("Show warning levels"), this);
    layout->addWidget(showWarnings);
    if (purple_prefs_get_bool("/quail/blist/show_warning_levels"))
		showWarnings->setChecked(true);

	/* Show numbers in groups */
    groupCount = new QCheckBox(tr("Show numbers in groups"), this);
    layout->addWidget(groupCount);
    if (purple_prefs_get_bool("/quail/blist/show_group_count"))
		groupCount->setChecked(true);

	/* Dim idle buddies */
    dimIdle = new QCheckBox(tr("Dim idle buddies"), this);
    layout->addWidget(dimIdle);
    if (purple_prefs_get_bool("/quail/blist/dim_idle_buddies"))
		dimIdle->setChecked(true);

}

/**************************************************************************
 * QQuailConvPrefPage
 **************************************************************************/
quail_conv_pref_page::quail_conv_pref_page(QWidget *parent)
    : quail_pref_page(parent)
{
}

void
quail_conv_pref_page::accept()
{
}

void
quail_conv_pref_page::buildInterface()
{
}


/**************************************************************************
 * QQuailNotifyPrefPage
 **************************************************************************/
quail_notify_pref_page::quail_notify_pref_page(QWidget *parent)
    : quail_pref_page(parent)
{
	buildInterface();
}

void
quail_notify_pref_page::accept()
{
    qDebug() << "QQuailNotifyPrefPage::accept()";
    if (purple_prefs_get_bool("/quail/notify/incoming_im") ==
		incomingIm->isChecked() &&
        purple_prefs_get_bool("/quail/notify/incoming_chat") ==
		incomingChat->isChecked() &&
        purple_prefs_get_bool("/quail/notify/use_buzzer") ==
		useBuzzer->isChecked() &&
        purple_prefs_get_bool("/quail/notify/use_led") ==
		useLed->isChecked())
	{
		return;
	}

    purple_prefs_set_bool("/quail/notify/incoming_im",
						incomingIm->isChecked());
    purple_prefs_set_bool("/quail/notify/incoming_chat",
						incomingChat->isChecked());
    purple_prefs_set_bool("/quail/notify/use_buzzer",
						useBuzzer->isChecked());
    purple_prefs_set_bool("/quail/notify/use_led",
						useLed->isChecked());
}

void
quail_notify_pref_page::buildInterface()
{
    qDebug() << "QQuailNotifyPrefPage::buildInterface()";

    QVBoxLayout *vbox = new QVBoxLayout(this);
	vbox->setSpacing(6);
	vbox->setMargin(6);

	/* Incoming IMs */
    incomingIm = new QCheckBox(tr("Incoming IMs"));
    vbox->addWidget(incomingIm);
    if (purple_prefs_get_bool("/quail/notify/incoming_im"))
		incomingIm->setChecked(true);

	/* Incoming Chats */
    incomingChat = new QCheckBox(tr("Incoming chat messages"));
    vbox->addWidget(incomingChat);
    if (purple_prefs_get_bool("/quail/notify/incoming_chat"))
		incomingChat->setChecked(true);

}


/**************************************************************************
 * QQuailAwayPrefPage
 **************************************************************************/
quail_away_pref_page::quail_away_pref_page(QWidget *parent)
    : quail_pref_page(parent)
{
    qDebug() << "QQuailAwayPrefPage::QQuailAwayPrefPage()";
}

void
quail_away_pref_page::accept()
{
}

void
quail_away_pref_page::buildInterface()
{
}


/**************************************************************************
 * QQuailProxyPrefPage
 **************************************************************************/
quail_proxy_pref_page::quail_proxy_pref_page(QWidget *parent)
    : quail_pref_page(parent)
{
    qDebug() << "QQuailProxyPrefPage::QQuailProxyPrefPage()";
}

void
quail_proxy_pref_page::accept()
{
}

void
quail_proxy_pref_page::buildInterface()
{
}


/**************************************************************************
 * QQuailPluginPrefPage
 **************************************************************************/
quail_plugin_pref_page::quail_plugin_pref_page(QWidget *parent)
    : quail_pref_page(parent)
{
    qDebug() << "QQuailPluginPrefPage::QQuailPluginPrefPage()";
}

void
quail_plugin_pref_page::accept()
{
}

void
quail_plugin_pref_page::buildInterface()
{
    qDebug() << "QQuailPluginPrefPage::buildInterface()";
}


/**************************************************************************
 * QQuailPrefsDialog
 **************************************************************************/
quail_prefs_dialog::quail_prefs_dialog(QWidget *parent)
    : QWidget(parent), m_parent(parent)
{
    qDebug() << "QQuailPrefsDialog::QQuailPrefsDialog()";
	buildInterface();
}

quail_prefs_dialog::~quail_prefs_dialog()
{
}

void
quail_prefs_dialog::buildInterface()
{
    qDebug() << "QQuailPrefsDialog::buildInterface()";
	QVBoxLayout *layout;

    setWindowTitle(tr("Preferences"));

	layout = new QVBoxLayout(this);

    tabs = new QTabWidget(this);
    layout->addWidget(tabs);
    blistPage    = new quail_blist_pref_page(this);
    notifyPage   = new quail_notify_pref_page(this);
#if 0
    convPage     = new QQuailConvPrefPage(this);
    awayIdlePage = new QQuailAwayPrefPage(this);
    proxyPage    = new QQuailProxyPrefPage(this);
    pluginPage   = new QQuailPluginPrefPage(this);
#endif

    tabs->addTab(blistPage, tr("Buddy List"));
    tabs->addTab(notifyPage, tr("Notification"));
#if 0
    tabs->addTab(convPage, tr("Conversations"));
    tabs->addTab(awayIdlePage, tr("Away/Idle"));
    tabs->addTab(proxyPage, tr("Proxy"));
    tabs->addTab(pluginPage, tr("Plugins"));
#endif

    tabs->setCurrentIndex(0);
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel,
                                                       Qt::Horizontal,
                                                       this);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(slotAccept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(slotRejected()));
    layout->addWidget(buttonBox);
    connect(this, SIGNAL(signalBListWindow()),
            m_parent, SLOT(showBlistWindow()));

}

void
quail_prefs_dialog::slotAccept()
{
    qDebug() << "QQuailPrefsDialog::accept()";
	blistPage->accept();
	notifyPage->accept();
#if 0
	convPage->accept();
	awayIdlePage->accept();
	proxyPage->accept();
	pluginPage->accept();
#endif
    emit signalBListWindow();
}

void
quail_prefs_dialog::slotRejected()
{
    qDebug() << "QQuailPrefsDialog::done()";
	close();
    emit signalBListWindow();
}
