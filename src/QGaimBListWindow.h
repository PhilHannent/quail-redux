#ifndef _QGAIM_BLIST_WIN_H_
#define _QGAIM_BLIST_WIN_H_

#include <libgaim/blist.h>

#include <qvariant.h>
#include <qwidget.h>
#include <qmainwindow.h>
#include <qlistview.h>

#include "QGaimAccountsWindow.h"

class QMenuBar;
class QToolBar;
class QToolButton;
class QGaimAccountsWindow;

class QGaimBListItem : public QListViewItem
{
	public:
		QGaimBListItem(QListView *parent, GaimBlistNode *node);
		QGaimBListItem(QListViewItem *parent, GaimBlistNode *node);
		~QGaimBListItem();

		GaimBlistNode *getBlistNode() const;

		void paintBranches(QPainter *p, const QColorGroup &cg,
						   int width, int y, int height, GUIStyle s);

	protected:
		void init();
		QPixmap *getProtocolIcon(GaimAccount *account);

	private:
		GaimBlistNode *node;
};

class QGaimBListWindow : public QMainWindow
{
	Q_OBJECT
	
	public:
		QGaimBListWindow();
		~QGaimBListWindow();

		void setGaimBlist(struct gaim_buddy_list *list);
		struct gaim_buddy_list *getGaimBlist() const;

		void update(GaimBlistNode *node);

	protected slots:
		void im();
		void chat();
		void showAccountsWindow();
		void showConversations();
		void blistToggled(bool state);
		void sendIm();

	private:
		void buildInterface();
		void buildMenuBar();
		void buildToolBar();

		void add_group(GaimBlistNode *node);

	private:
		struct gaim_buddy_list *gaimBlist;

		QAction *imButton;
		QAction *chatButton;
		QAction *infoButton;
		QAction *awayButton;

		QToolButton *blistButton;
		QToolButton *convsButton;

		QPopupMenu *buddiesMenu;
		QPopupMenu *toolsMenu;
		QPopupMenu *helpMenu;
		QPopupMenu *awayMenu;
		QPopupMenu *pounceMenu;
		QPopupMenu *protocolMenu;

		QListView *buddylist;
		QMenuBar *menubar;
		QToolBar *toolbar;
};

struct gaim_blist_ui_ops *qGaimGetBlistUiOps();

#endif /* _QGAIM_BLIST_WIN_H_ */
