#ifndef _QGAIM_BLIST_WIN_H_
#define _QGAIM_BLIST_WIN_H_

#include <libgaim/blist.h>

#include <qvariant.h>
#include <qwidget.h>
#include <qmainwindow.h>
#include <qlistview.h>

#include "QGaimAccountsWindow.h"

class QMenuBar;
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

	private:
		void buildInterface();
		QMenuBar *buildMenuBar();

		void add_group(GaimBlistNode *node);

	private:
		struct gaim_buddy_list *gaimBlist;

		QToolButton *imButton;
		QToolButton *chatButton;
		QToolButton *infoButton;
		QToolButton *awayButton;

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
};

struct gaim_blist_ui_ops *qGaimGetBlistUiOps();

#endif /* _QGAIM_BLIST_WIN_H_ */
