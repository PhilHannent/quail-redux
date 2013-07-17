﻿/**
 * @file QQuailDialogs.h Small dialogs that don't belong in their own files
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
#ifndef _QUAIL_DIALOGS_H_
#define _QUAIL_DIALOGS_H_

#include <libpurple/account.h>

#include <QDialog>
#include <QList>

class QComboBox;
class QFrame;
class QQuailAccountBox;
class QGridLayout;
class QLabel;
class QLineEdit;

class QQuailAddBuddyDialog : public QDialog
{
	Q_OBJECT

	public:
        QQuailAddBuddyDialog(QWidget *parent = NULL);

		void setScreenName(const QString &screenName);
		void setAlias(const QString &alias);
		void setGroup(const QString &group);
		void setAccount(PurpleAccount *account);

	protected:
		void buildInterface();
		void populateGroupCombo();

	protected slots:
		void accept();

	private:
		QLineEdit *screenNameEntry;
		QLineEdit *aliasEntry;
		QComboBox *groupCombo;
		QQuailAccountBox *accountCombo;
};

class QQuailAddChatDialog : public QDialog
{
	Q_OBJECT

	public:
        QQuailAddChatDialog(QWidget *parent = NULL);

		void setAlias(const QString &alias);
		void setGroup(const QString &group);
		void setAccount(PurpleAccount *account);

	protected:
		void buildInterface();
		void populateGroupCombo();
		void rebuildWidgetsFrame();

	protected slots:
		void accountChanged(int index);
		void accept();

	private:
		QLabel *aliasLabel;
		QLabel *groupLabel;
		QLineEdit *aliasEntry;
		QComboBox *groupCombo;
		QQuailAccountBox *accountCombo;
		QGridLayout *grid;
		QFrame *widgetsFrame;
        QList<QWidget*> labels;
        QList<QWidget*> widgets;
};

class QQuailNewImDialog : public QDialog
{
	Q_OBJECT

	public:
        QQuailNewImDialog(QWidget *parent = NULL);

		void setScreenName(const QString &screenName);
		void setAccount(PurpleAccount *account);

	protected:
		void buildInterface();

	protected slots:
		void accept();

	private:
		QLineEdit *screenNameEntry;
		QQuailAccountBox *accountCombo;
};

class QQuailJoinChatDialog : public QDialog
{
	Q_OBJECT

	public:
        QQuailJoinChatDialog(QWidget *parent = NULL);

		void setAccount(PurpleAccount *account);

	protected:
		void buildInterface();
		void rebuildWidgetsFrame();

	protected slots:
		void accountChanged(int index);
		void accept();

	private:
		QQuailAccountBox *accountCombo;
		QGridLayout *grid;
		QFrame *widgetsFrame;
        QList<QWidget*> labels;
        QList<QWidget*> widgets;
};

#endif /* _QUAIL_DIALOGS_H_ */
