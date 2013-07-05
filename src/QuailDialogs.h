/**
 * @file QGaimDialogs.h Small dialogs that don't belong in their own files
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
#ifndef _QGAIM_DIALOGS_H_
#define _QGAIM_DIALOGS_H_

#include <libpurple/account.h>

#include <qdialog.h>
#include <qlist.h>

class QComboBox;
class QFrame;
class QGaimAccountBox;
class QGridLayout;
class QLabel;
class QLineEdit;

class QGaimAddBuddyDialog : public QDialog
{
	Q_OBJECT

	public:
		QGaimAddBuddyDialog(QWidget *parent = NULL, const char *name = NULL,
							WFlags fl = 0);

		void setScreenName(const QString &screenName);
		void setAlias(const QString &alias);
		void setGroup(const QString &group);
		void setAccount(GaimAccount *account);

	protected:
		void buildInterface();
		void populateGroupCombo();

	protected slots:
		void accept();

	private:
		QLineEdit *screenNameEntry;
		QLineEdit *aliasEntry;
		QComboBox *groupCombo;
		QGaimAccountBox *accountCombo;
};

class QGaimAddChatDialog : public QDialog
{
	Q_OBJECT

	public:
		QGaimAddChatDialog(QWidget *parent = NULL, const char *name = NULL,
						   WFlags fl = 0);

		void setAlias(const QString &alias);
		void setGroup(const QString &group);
		void setAccount(GaimAccount *account);

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
		QGaimAccountBox *accountCombo;
		QGridLayout *grid;
		QFrame *widgetsFrame;
		QList<QWidget> labels;
		QList<QWidget> widgets;
};

class QGaimNewImDialog : public QDialog
{
	Q_OBJECT

	public:
		QGaimNewImDialog(QWidget *parent = NULL, const char *name = NULL,
						 WFlags fl = 0);

		void setScreenName(const QString &screenName);
		void setAccount(GaimAccount *account);

	protected:
		void buildInterface();

	protected slots:
		void accept();

	private:
		QLineEdit *screenNameEntry;
		QGaimAccountBox *accountCombo;
};

class QGaimJoinChatDialog : public QDialog
{
	Q_OBJECT

	public:
		QGaimJoinChatDialog(QWidget *parent = NULL, const char *name = NULL,
							WFlags fl = 0);

		void setAccount(GaimAccount *account);

	protected:
		void buildInterface();
		void rebuildWidgetsFrame();

	protected slots:
		void accountChanged(int index);
		void accept();

	private:
		QGaimAccountBox *accountCombo;
		QGridLayout *grid;
		QFrame *widgetsFrame;
		QList<QWidget> labels;
		QList<QWidget> widgets;
};

#endif /* _QGAIM_DIALOGS_H_ */
