#ifndef _QGAIM_ACCOUNTS_DIALOG_H_
#define _QGAIM_ACCOUNTS_DIALOG_H_

#include <libgaim/prpl.h>

#include <qvariant.h>
#include <qwidget.h>
#include <qmainwindow.h>
#include <qlistview.h>

class QAction;
class QMenuBar;
class QToolButton;

class QGaimAccountListItem : public QListViewItem
{
	public:
		QGaimAccountListItem(QListView *parent);
		~QGaimAccountListItem();

		void setAccount(GaimAccount *account);
		GaimAccount *getAccount() const;

	private:
		GaimAccount *account;
};

class QGaimAccountsWindow : public QMainWindow
{
	Q_OBJECT
	
	public:
		QGaimAccountsWindow();
		~QGaimAccountsWindow();

	private:
		void buildInterface();
		void setupMenubar();
		void setupToolbar();
		void loadAccounts();

		QString getProtocolName(GaimProtocol protocol);

	private slots:
		void connectToAccount();
		void showBlist();
		void accountsToggled(bool state);
		void showConversations();

	private:
		QMenuBar *menubar;
		QToolButton *accountsButton;
		QPopupMenu *accountMenu;

		QListView *accountsView;
};

#endif /* _QGAIM_ACCOUNTS_DIALOG_H_ */
