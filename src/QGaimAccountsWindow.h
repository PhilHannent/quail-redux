#ifndef _QGAIM_ACCOUNTS_DIALOG_H_
#define _QGAIM_ACCOUNTS_DIALOG_H_

#include <libgaim/prpl.h>

#include <qvariant.h>
#include <qwidget.h>
#include <qmainwindow.h>
#include <qlistview.h>

class QAction;
class QGaimConnectionMeters;
class QPixmap;
class QToolBar;
class QToolButton;
class QVBox;

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

		QGaimConnectionMeters *getMeters() const;

	private:
		void buildInterface();
		void setupToolbar();
		void loadAccounts();

		QString getProtocolName(GaimProtocol protocol);

	private slots:
		void newAccount();
		void connectToAccount();
		void disconnectFromAccount();
		void showBlist();
		void accountsToggled(bool state);
		void accountSelected(QListViewItem *item);

	private:
		QPixmap *getProtocolIcon(GaimAccount *account);

	private:
		QToolBar *toolbar;
		QPopupMenu *accountMenu;
		QToolButton *accountsButton;

		QAction *connectButton;
		QAction *disconnectButton;

		QListView *accountsView;

		QGaimConnectionMeters *meters;
};

GaimConnectionUiOps *qGaimGetConnectionUiOps();

#endif /* _QGAIM_ACCOUNTS_DIALOG_H_ */
