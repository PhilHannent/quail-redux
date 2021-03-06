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
class quail_account_box;
class QGridLayout;
class QLabel;
class QLineEdit;

class quail_dialog : public QDialog
{
    Q_OBJECT

public:
    quail_dialog(QWidget *parent = 0);

protected:
    virtual void buildInterface() = 0;

    QPushButton *okButton;
    QPushButton *cancelButton;
protected slots:
    virtual void accept() = 0;

};

class quail_add_buddy_dialog : public quail_dialog
{
	Q_OBJECT

	public:
        quail_add_buddy_dialog(QWidget *parent = 0);

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
		quail_account_box *accountCombo;
};

class quail_add_chat_dialog : public quail_dialog
{
	Q_OBJECT

	public:
        quail_add_chat_dialog(QWidget *parent = 0);

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
		quail_account_box *accountCombo;
		QGridLayout *grid;
		QFrame *widgetsFrame;
        QList<QWidget*> labels;
        QList<QWidget*> widgets;
};

class quail_new_im_dialog : public quail_dialog
{
	Q_OBJECT

	public:
        quail_new_im_dialog(QWidget *parent = 0);

		void setScreenName(const QString &screenName);
		void setAccount(PurpleAccount *account);

	protected:
		void buildInterface();

	protected slots:
		void accept();

	private:
		QLineEdit *screenNameEntry;
		quail_account_box *accountCombo;
};

class quail_join_chat_dialog : public quail_dialog
{
	Q_OBJECT

	public:
        quail_join_chat_dialog(QWidget *parent = 0);

		void setAccount(PurpleAccount *account);

	protected:
		void buildInterface();
		void rebuildWidgetsFrame();

	protected slots:
		void accountChanged(int index);
		void accept();

	private:
		quail_account_box *accountCombo;
		QGridLayout *grid;
		QFrame *widgetsFrame;
        QList<QWidget*> labels;
        QList<QWidget*> widgets;
};

#endif /* _QUAIL_DIALOGS_H_ */
