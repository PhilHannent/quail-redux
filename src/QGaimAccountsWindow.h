#ifndef _QGAIM_ACCOUNTS_DIALOG_H_
#define _QGAIM_ACCOUNTS_DIALOG_H_

#include <libgaim/prpl.h>

#include <qvariant.h>
#include <qwidget.h>
#include <qmainwindow.h>
#include <qlistview.h>

class QAction;
class QMenuBar;
class QPixmap;
class QToolButton;

class QGaimAccountListItem : public QListViewItem
{
	public:
		QGaimAccountListItem(QListView *parent, int index);
		~QGaimAccountListItem();

		void setAccount(GaimAccount *account);
		GaimAccount *getAccount() const;

		virtual QString key(int column, bool ascending) const;

	private:
		GaimAccount *account;
		int index;
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
		void disconnectFromAccount();
		void showBlist();
		void accountsToggled(bool state);
		void showConversations();
		void accountSelected(QListViewItem *item);

	private:
		QPixmap *getProtocolIcon(GaimAccount *account);

	private:
		QMenuBar *menubar;
		QPopupMenu *accountMenu;
		QToolButton *accountsButton;

		QAction *connectButton;
		QAction *disconnectButton;

		QListView *accountsView;
};

#endif /* _QGAIM_ACCOUNTS_DIALOG_H_ */