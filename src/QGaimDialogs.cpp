/**
 * @file QGaimDialogs.cpp Small dialogs that don't belong in their own files
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
#include "QGaimDialogs.h"
#include "QGaimAccountBox.h"

#include <libgaim/blist.h>
#include <libgaim/conversation.h>
#include <libgaim/notify.h>

#include <qcombobox.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qvbox.h>

QGaimAddBuddyDialog::QGaimAddBuddyDialog(QWidget *parent, const char *name,
										 WFlags fl)
	: QDialog(parent, name, fl)
{
	buildInterface();
}

void
QGaimAddBuddyDialog::setScreenName(const QString &screenName)
{
	screenNameEntry->setText(screenName);
}

void
QGaimAddBuddyDialog::setAlias(const QString &alias)
{
	aliasEntry->setText(alias);
}

void
QGaimAddBuddyDialog::setGroup(const QString &group)
{
	groupCombo->lineEdit()->setText(group);
}

void
QGaimAddBuddyDialog::setAccount(GaimAccount *account)
{
	accountCombo->setCurrentAccount(account);
}

void
QGaimAddBuddyDialog::buildInterface()
{
	QFrame *frame;
	QGridLayout *grid;
	QLabel *label;
	QLabel *spacer;
	QVBox *vbox;
	QVBoxLayout *layout;

	setCaption(tr("Add Buddy"));

	layout = new QVBoxLayout(this);
	layout->setAutoAdd(true);

	vbox = new QVBox(this);
	vbox->setSpacing(5);
	vbox->setMargin(6);

	label = new QLabel(tr("<p>Please enter the screen name of the person you "
						  "would like to add to your buddy list. You may "
						  "optionally enter an alias, or nickname, for the "
						  "buddy. The alias will be displayed in place of "
						  "the screen name whenever possible.</p>"),
					   vbox);

	frame = new QFrame(vbox);
	grid = new QGridLayout(frame, 1, 1);
	grid->setSpacing(5);

	/* Username */
	grid->addWidget(new QLabel(tr("Screen Name:"), frame), 0, 0);
	screenNameEntry = new QLineEdit(frame);
	grid->addWidget(screenNameEntry, 0, 1);

	/* Alias */
	grid->addWidget(new QLabel(tr("Alias:"), frame), 1, 0);
	aliasEntry = new QLineEdit(frame);
	grid->addWidget(aliasEntry, 1, 1);

	/* Group */
	grid->addWidget(new QLabel(tr("Group:"), frame), 2, 0);
	groupCombo = new QComboBox(true, frame, "group");
	grid->addWidget(groupCombo, 2, 1);

	populateGroupCombo();

	/* Account */
	grid->addWidget(new QLabel(tr("Account:"), frame), 3, 0);
	accountCombo = new QGaimAccountBox(false, frame, "account");
	grid->addWidget(accountCombo, 3, 1);

	/* Add a spacer. */
	spacer = new QLabel("", vbox);
	vbox->setStretchFactor(spacer, 1);
}

void
QGaimAddBuddyDialog::populateGroupCombo()
{
	GaimBlistNode *node = gaim_get_blist()->root;

	if (node == NULL)
		groupCombo->insertItem(tr("Buddies"));
	else
	{
		for (; node != NULL; node = node->next)
		{
			if (GAIM_BLIST_NODE_IS_GROUP(node))
			{
				struct group *g = (struct group *)node;

				groupCombo->insertItem(g->name);
			}
		}
	}
}

void
QGaimAddBuddyDialog::accept()
{
	GaimConversation *conv;
	QString screenname = screenNameEntry->text();
	QString alias = aliasEntry->text();
	QString group = groupCombo->currentText();
	GaimAccount *account;
	struct buddy *b;
	struct group *g;

	if (screenname.isEmpty())
	{
		gaim_notify_error(this, tr("Add Buddy"),
						  tr("You must specify a screen name to add."), NULL);
		return;
	}

	conv = gaim_find_conversation(screenname);

	if ((g = gaim_find_group(group)) == NULL)
	{
		g = gaim_group_new(group);
		gaim_blist_add_group(g, NULL);
	}

	account = accountCombo->getCurrentAccount();

	b = gaim_buddy_new(account, screenname, (alias.isEmpty() ? NULL : alias));

	gaim_blist_add_buddy(b, g, NULL);
	serv_add_buddy(gaim_account_get_connection(account), screenname);

	if (conv != NULL)
		gaim_conversation_update(conv, GAIM_CONV_UPDATE_ADD);

	gaim_blist_save();

	QDialog::accept();
}
