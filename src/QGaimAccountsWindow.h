/**
 * @file QGaimAccountsWindow.h Accounts window
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
#ifndef _QGAIM_ACCOUNTS_WINDOW_H_
#define _QGAIM_ACCOUNTS_WINDOW_H_

#include <libgaim/prpl.h>

#include <qvariant.h>
#include <qwidget.h>
#include <qmainwindow.h>
#include <qlistview.h>

class QAction;
class QPixmap;
class QTimer;
class QToolBar;
class QToolButton;
class QVBox;

class QGaimAccountListItem : public QObject, public QListViewItem
{
	Q_OBJECT

	public:
		QGaimAccountListItem(QListView *parent, int index);
		~QGaimAccountListItem();

		void startPulse(QPixmap onlinePixmap);
		void stopPulse();

		void setAccount(GaimAccount *account);
		GaimAccount *getAccount() const;

		virtual QString key(int column, bool ascending) const;

	protected slots:
		void updatePulse();

	private:
		GaimAccount *account;
		int index;
		QTimer *pulseTimer;
		bool pulseGrey;
		float pulseStep;
		QPixmap *pulseOrigPixmap;
};

class QGaimAccountsWindow : public QMainWindow
{
	Q_OBJECT

	public:
		QGaimAccountsWindow(QMainWindow *parent);
		~QGaimAccountsWindow();

		void updateAccounts();

		void accountSignedOn(GaimAccount *account);
		void accountSignedOff(GaimAccount *account);

	private:
		void buildInterface();
		void setupToolbar();
		void loadAccounts();

	private slots:
		void newAccount();
		void editAccount();
		void deleteAccount();

		void connectToAccount();
		void disconnectFromAccount();

		void showBlist();

		void accountsToggled(bool state);
		void accountSelected(QListViewItem *item);

		void resizeEvent(QResizeEvent *event);

	protected:
		QGaimAccountListItem *getItemFromAccount(GaimAccount *account);

	private:
		QMainWindow *parentMainWindow;

		QToolBar *toolbar;
		QPopupMenu *accountMenu;
		QAction *accountsButton;

		QAction *editButton;
		QAction *deleteButton;
		QAction *connectButton;
		QAction *disconnectButton;

		QListView *accountsView;
};

GaimConnectionUiOps *qGaimGetConnectionUiOps();

#endif /* _QGAIM_ACCOUNTS_WINDOW_H_ */
