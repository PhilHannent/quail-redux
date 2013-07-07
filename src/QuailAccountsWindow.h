/**
 * @file QQuailAccountsWindow.h Accounts window
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
#ifndef _QGAIM_ACCOUNTS_WINDOW_H_
#define _QGAIM_ACCOUNTS_WINDOW_H_

#include <libpurple/prpl.h>

#include <QVariant>
#include <QWidget>
#include <QMainWindow>
#include <QTableWidget>

class QAction;
class QPixmap;
class QTimer;
class QToolBar;
class QToolButton;
class QVBoxLayout;

class QQuailAccountItem : public QObject, public QTableWidgetItem
{
	Q_OBJECT

	public:
        QQuailAccountItem(int index);
        ~QQuailAccountItem();

		void startPulse(QPixmap onlinePixmap);
		void stopPulse();

		void setAccount(PurpleAccount *account);
		PurpleAccount *getAccount() const;

		virtual QString key(int column, bool ascending) const;

	protected slots:
		void updatePulse();

	private:
		PurpleAccount *account;
		int index;
		QTimer *pulseTimer;
		bool pulseGrey;
		float pulseStep;
		QPixmap *pulseOrigPixmap;
};

class QQuailAccountsWindow : public QMainWindow
{
	Q_OBJECT

	public:
		QQuailAccountsWindow(QMainWindow *parent);
		~QQuailAccountsWindow();

		void updateAccounts();

		void accountSignedOn(PurpleAccount *account);
		void accountSignedOff(PurpleAccount *account);

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
        void accountSelected();

		void resizeEvent(QResizeEvent *event);

	protected:
        QQuailAccountItem *getItemFromAccount(PurpleAccount *account);

	private:
		QMainWindow *parentMainWindow;

		QToolBar *toolbar;
        QMenu *accountMenu;
		QAction *accountsButton;

		QAction *editButton;
		QAction *deleteButton;
		QAction *connectButton;
		QAction *disconnectButton;

        QTableWidget *accountsWidget;
};

PurpleConnectionUiOps *qQuailGetConnectionUiOps();

#endif /* _QGAIM_ACCOUNTS_WINDOW_H_ */
