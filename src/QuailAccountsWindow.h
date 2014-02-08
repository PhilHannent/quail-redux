/**
 * @file QQuailAccountsWindow.h Accounts window
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
#ifndef _QUAIL_ACCOUNTS_WINDOW_H_
#define _QUAIL_ACCOUNTS_WINDOW_H_

#include <libpurple/prpl.h>

#include <QCheckBox>
#include <QMainWindow>
#include <QTableWidget>
#include <QVariant>
#include <QWidget>

class QAction;
class QPixmap;
class QTimer;
class QToolBar;
class QToolButton;
class QVBoxLayout;

class quail_account_item : public QObject, public QTableWidgetItem
{
	Q_OBJECT

	public:
        quail_account_item();
        ~quail_account_item();

        void startPulse(QPixmap onlinePixmap, QString pixmapName);
		void stopPulse();

		void setAccount(PurpleAccount *account);
        PurpleAccount *get_account() const;

	protected slots:
		void updatePulse();

	private:
		PurpleAccount *account;
		QTimer *pulseTimer;
		bool pulseGrey;
		float pulseStep;
		QPixmap *pulseOrigPixmap;
        QString pulseOrigPixmapName;
};

class quail_accounts_window : public QMainWindow
{
	Q_OBJECT

	public:
        quail_accounts_window(QMainWindow *parent);
        ~quail_accounts_window();

		void accountSignedOn(PurpleAccount *account);
		void accountSignedOff(PurpleAccount *account);

        enum AccountsWindowColumnOrder {
            xUserName = 0,
            xProtocol,
            xEnabled };

    public slots:
        void slotUpdateAccounts();

	private slots:
		void newAccount();
		void editAccount();
		void deleteAccount();

		void connectToAccount();
		void disconnectFromAccount();

		void accountsToggled(bool state);
        void slot_account_selected();
        //void slot_table_item_clicked(QTableWidgetItem *);
        void slot_table_item_pressed(QTableWidgetItem *);

		void resizeEvent(QResizeEvent *event);

	protected:
        quail_account_item *get_item_from_account(PurpleAccount *account);

	private:
        void buildInterface();
        void setupToolbar();
        void loadAccounts();
        QMainWindow *parentMainWindow;

		QToolBar *toolbar;
        QMenu *accountMenu;
		QAction *accountsButton;

		QAction *editButton;
		QAction *deleteButton;
		QAction *connectButton;
		QAction *disconnectButton;

        QTableWidget *m_accounts_widget;
};

PurpleConnectionUiOps *qQuailGetConnectionUiOps();

#endif /* _QUAIL_ACCOUNTS_WINDOW_H_ */
