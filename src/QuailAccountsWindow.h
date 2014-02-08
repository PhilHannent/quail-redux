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

        void start_pulse(QPixmap onlinePixmap, QString pixmapName);
        void stop_pulse();

        void set_account(PurpleAccount *m_account);
        PurpleAccount *get_account() const;

	protected slots:
        void update_pulse();

	private:
        PurpleAccount *m_account;
        QTimer *m_pulse_timer;
        bool m_pulse_grey;
        float m_pulse_step;
        QPixmap *m_pulse_orig_pixmap;
        QString m_pulse_orig_pixmap_name;
};

class quail_accounts_window : public QMainWindow
{
	Q_OBJECT

public:
    quail_accounts_window(QMainWindow *parent);
    ~quail_accounts_window();

    void account_signed_on(PurpleAccount *account);
    void account_signed_off(PurpleAccount *account);

    enum AccountsWindowColumnOrder {
        xUserName = 0,
        xProtocol,
        xEnabled };

signals:
    void signal_update_accounts();

public slots:
    void slot_update_accounts();

private slots:
    void new_account();
    void edit_account();
    void delete_account();

    void slot_account_selected();
    void slot_table_item_pressed(QTableWidgetItem *);

    void resizeEvent(QResizeEvent *event);

private:
    void build_interface();
    void setup_toolbar();
    quail_account_item *get_item_from_account(PurpleAccount *account);

    QMainWindow *m_parent;
    QToolBar *m_toolbar;

    QAction *m_edit_btn;
    QAction *m_delete_btn;

    QTableWidget *m_accounts_widget;
};

PurpleConnectionUiOps *qQuailGetConnectionUiOps();

#endif /* _QUAIL_ACCOUNTS_WINDOW_H_ */
