#include "QGaimBListWindow.h"
#include "QGaim.h"
#include "base.h"

#include <qpe/qpeapplication.h>
#include <libgaim/multi.h>
#include <libgaim/debug.h>

#include <qaction.h>
#include <qlayout.h>
#include <qlistview.h>
#include <qmenubar.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qheader.h>

/**************************************************************************
 * QGaimBListItem
 **************************************************************************/
QGaimBListItem::QGaimBListItem(QListView *parent, GaimBlistNode *node)
	: QListViewItem(parent), node(node)
{
	
}

QGaimBListItem::QGaimBListItem(QListViewItem *parent, GaimBlistNode *node)
	: QListViewItem(parent), node(node)
{
}

QGaimBListItem::~QGaimBListItem()
{
}

GaimBlistNode *
QGaimBListItem::getBlistNode() const
{
	return node;
}

void
QGaimBListItem::paintBranches(QPainter *p, const QColorGroup &cg,
							  int width, int, int height, GUIStyle)
{
	p->fillRect(0, 0, width, height, QBrush(cg.base()));
}


/**************************************************************************
 * QGaimBListWindow
 **************************************************************************/
QGaimBListWindow::QGaimBListWindow()
	: QMainWindow()
{
	buildInterface();
}

QGaimBListWindow::~QGaimBListWindow()
{
}

void
QGaimBListWindow::setGaimBlist(struct gaim_buddy_list *list)
{
	gaimBlist = list;
}

struct gaim_buddy_list *
QGaimBListWindow::getGaimBlist() const
{
	return gaimBlist;
}

void
QGaimBListWindow::update(GaimBlistNode *node)
{
	QGaimBListItem *item = (QGaimBListItem *)node->ui_data;
	bool expand = false;
	bool new_entry = true;

	if (item == NULL)
	{
		new_entry = true;

		if (GAIM_BLIST_NODE_IS_BUDDY(node))
		{
			struct buddy *buddy = (struct buddy *)node;

			if (buddy->present != GAIM_BUDDY_OFFLINE ||
				(gaim_account_is_connected(buddy->account) && 0)) // TODO
			{
				char *collapsed =
					gaim_group_get_setting((struct group *)node->parent,
										   "collapsed");

				if (node->parent != NULL && node->parent->ui_data == NULL)
				{
					/* Add the buddy's group. */
					add_group(node->parent);
				}

				expand = !collapsed;

				g_free(collapsed);

				/* Add the buddy here. */
				node->ui_data = new QGaimBListItem(
					(QGaimBListItem *)(node->parent->ui_data), node);
			}
		}
		else if (GAIM_BLIST_NODE_IS_CHAT(node) &&
				 gaim_account_is_connected(((struct chat *)node)->account))
		{
			char *collapsed =
				gaim_group_get_setting((struct group *)node->parent,
									   "collapsed");

			if (node->parent != NULL && node->parent->ui_data == NULL)
			{
				/* Add the chat's group. */
				add_group(node->parent);
			}

			expand = !collapsed;

			g_free(collapsed);

			/* Add the chat here. */
			node->ui_data = new QGaimBListItem(
				(QGaimBListItem *)node->parent->ui_data, node);
		}
		else if (GAIM_BLIST_NODE_IS_GROUP(node) && 0) /* TODO */
		{
			add_group(node);
			expand = true;
		}
	}
	else if (GAIM_BLIST_NODE_IS_GROUP(node))
	{
		if (gaim_blist_get_group_online_count((struct group *)node) == 0)
		{
			item = (QGaimBListItem *)node->ui_data;

			delete item;
			node->ui_data = NULL;
		}
		else
		{
			add_group(node);
		}
	}

	item = (QGaimBListItem *)node->ui_data;

	if (item == NULL)
		return;

	if (GAIM_BLIST_NODE_IS_CHAT(node))
	{
		struct chat *chat = (struct chat *)node;

		if (gaim_account_is_connected(chat->account))
		{
			if (chat->alias)
				item->setText(0, chat->alias);
			else
			{
				GaimConnection *gc;
				struct proto_chat_entry *pce;
				GList *parts, *tmp;

				gc = gaim_account_get_connection(chat->account);

				parts = GAIM_PLUGIN_PROTOCOL_INFO(gc->prpl)->chat_info(gc);
				pce = (struct proto_chat_entry *)parts->data;
				item->setText(0, (char *)g_hash_table_lookup(chat->components,
															 pce->identifier));

				for (tmp = parts; tmp != NULL; tmp = tmp->next)
					g_free(tmp->data);

				g_list_free(parts);
			}
		}
		else
		{
			delete item;
			node->ui_data = NULL;
		}
	}
	else if (GAIM_BLIST_NODE_IS_BUDDY(node))
	{
		struct buddy *buddy = (struct buddy *)node;

		if (buddy->present != GAIM_BUDDY_OFFLINE ||
			(gaim_account_is_connected(buddy->account) && 0)) // XXX
		{
			if (buddy->idle > 0)
			{
				time_t t;
				int ihrs, imin;
				char *idle;

				time(&t);

				ihrs = (t - ((struct buddy *)node)->idle) / 3600;
				imin = ((t - ((struct buddy*)node)->idle) / 60) % 60;

				if (ihrs > 0)
					idle = g_strdup_printf("(%d:%02d)", ihrs, imin);
				else
					idle = g_strdup_printf("(%d)", imin);

				QString str = idle;

				item->setText(1, str);

				g_free(idle);
			}

			item->setText(0, gaim_get_buddy_alias(buddy));
		}
		else
		{
			delete item;
			node->ui_data = NULL;
		}
	}
	else if (GAIM_BLIST_NODE_IS_BUDDY(node) && !new_entry)
	{
		delete item;
		node->ui_data = NULL;
	}

	if (expand)
		item->setOpen(true);
}

void
QGaimBListWindow::buildInterface()
{
	setCaption(tr("Gaim - Buddy List"));

	/* Setup the menubar */
	buildMenuBar();

	/* Setup the list view */
	buddylist = new QListView(this, "BuddyList");
	buddylist->addColumn(tr("Buddy"), -1);
	buddylist->setAllColumnsShowFocus(TRUE);
	buddylist->setRootIsDecorated(true);
	buddylist->header()->hide();

	QPEApplication::setStylusOperation(buddylist->viewport(),
									   QPEApplication::RightOnHold);

	setCentralWidget(buddylist);
}

QMenuBar *
QGaimBListWindow::buildMenuBar()
{
	QPixmap *pixmap;
	QAction *a;
	QToolButton *button;

	menubar = new QMenuBar(this);

	/* Build the Buddies menu */
	buddiesMenu = new QPopupMenu(this);
	buddiesMenu->setCheckable(TRUE);
	menubar->insertItem(tr("&Buddies"), buddiesMenu);

	/* Buddies -> New Instant Message */
	a = new QAction(tr("New &Instant Message..."), QString::null, 0, this, 0);
	a->addTo(buddiesMenu);

	connect(a, SIGNAL(activated()),
			this, SLOT(im()));

	/* Buddies -> Join A Chat */
	a = new QAction(tr("Join a &Chat..."), QString::null, 0, this, 0);
	a->addTo(buddiesMenu);

	/* Buddies -> Get User Info */
	a = new QAction(tr("Get &User Info"), QString::null, 0, this, 0);
	a->addTo(buddiesMenu);

	buddiesMenu->insertSeparator();

	/* Buddies -> Show Offline Buddies */
	a = new QAction(tr("Show &Offline Buddies"), QString::null,
					0, this, 0, TRUE);
	a->addTo(buddiesMenu);

	/* Buddies -> Show Empty Groups */
	a = new QAction(tr("Show &Empty Groups"), QString::null,
					0, this, 0, TRUE);
	a->addTo(buddiesMenu);

	/* Buddies -> Add a Buddy */
	a = new QAction(tr("&Add a Buddy..."), QString::null, 0, this, 0);
	a->addTo(buddiesMenu);

	/* Buddies -> Add a Chat */
	a = new QAction(tr("&Add a Chat..."), QString::null, 0, this, 0);
	a->addTo(buddiesMenu);

	/* Buddies -> Add a Group */
	a = new QAction(tr("Add a &Group..."), QString::null, 0, this, 0);
	a->addTo(buddiesMenu);

	buddiesMenu->insertSeparator();

	/* Buddies -> Signoff */
	a = new QAction(tr("&Signoff"), QString::null, 0, this, 0);
	a->addTo(buddiesMenu);

	/* Buddies -> Quit */
	a = new QAction(tr("&Quit"), QString::null, 0, this, 0);
	a->addTo(buddiesMenu);


	/* Build the Tools menu */
	toolsMenu = new QPopupMenu(this);

	menubar->insertItem(tr("&Tools"), toolsMenu);

	/* Tools -> Away */
	awayMenu = new QPopupMenu(this);
	toolsMenu->insertItem(tr("&Away"), awayMenu);

	/* Tools -> Buddy Pounce */
	pounceMenu = new QPopupMenu(this);
	toolsMenu->insertItem(tr("Buddy &Pounce"), pounceMenu);

	/* Tools -> Protocol Options */
	protocolMenu = new QPopupMenu(this);
	toolsMenu->insertItem(tr("P&rotocol Options"), protocolMenu);

	toolsMenu->insertSeparator();

	/* Tools -> Accounts */
	a = new QAction(tr("A&ccounts"), QString::null, 0, this, 0);
	a->addTo(toolsMenu);

	connect(a, SIGNAL(activated()),
			this, SLOT(showAccountsWindow()));

	/* Tools -> File Transfers */
	a = new QAction(tr("&File Transfers"), QString::null, 0, this, 0);
	a->addTo(toolsMenu);

	/* Tools -> Preferences */
	a = new QAction(tr("Preferences"), QString::null, 0, this, 0);
	a->addTo(toolsMenu);

	/* Tools -> Privacy */
	a = new QAction(tr("Pr&ivacy"), QString::null, 0, this, 0);
	a->addTo(toolsMenu);

	toolsMenu->insertSeparator();

	/* Tools -> View System Log */
	a = new QAction(tr("View System &Log"), QString::null, 0, this, 0);
	a->addTo(toolsMenu);


	/* Help menu */
	helpMenu = new QPopupMenu(this);
	menubar->insertItem(tr("&Help"), helpMenu);

	/* Help -> Debug Window */
	a = new QAction(tr("&Debug Window"), QString::null, 0, this, 0);
	a->addTo(helpMenu);

	/* Help -> About */
	a = new QAction(tr("&About"), QString::null, 0, this, 0);
	a->addTo(helpMenu);


	/* Now we're going to construct the toolbar on the right. */
	menubar->insertSeparator();

	/* Buddy List */
	pixmap = new QPixmap(DATA_PREFIX "images/blist.png");
	blistButton = button = new QToolButton(NULL, "blist");
	button->setAutoRaise(true);
	button->setPixmap(*pixmap);
	button->setToggleButton(true);
	button->setOn(true);
	menubar->insertItem(button);
	delete pixmap;

	connect(button, SIGNAL(toggled(bool)),
			this, SLOT(blistToggled(bool)));

	/* Accounts */
	pixmap = new QPixmap(DATA_PREFIX "images/accounts.png");
	button = new QToolButton(NULL, "accounts");
	button->setAutoRaise(true);
	button->setPixmap(*pixmap);
	menubar->insertItem(button);
	delete pixmap;

	connect(button, SIGNAL(clicked()),
			this, SLOT(showAccountsWindow()));

	/* Conversations */
	pixmap = new QPixmap(DATA_PREFIX "images/edit.png");
	button = new QToolButton(NULL, "accounts");
	button->setAutoRaise(true);
	button->setPixmap(*pixmap);
	menubar->insertItem(button);
	delete pixmap;

	connect(button, SIGNAL(clicked()),
			this, SLOT(showConversations()));

	return menubar;
}

void
QGaimBListWindow::add_group(GaimBlistNode *node)
{
	QGaimBListItem *item = new QGaimBListItem(buddylist, node);
	node->ui_data = item;

	item->setText(0, ((struct group *)node)->name);

	item->setExpandable(true);
}

/**************************************************************************
 * Slots
 **************************************************************************/
void
QGaimBListWindow::im()
{
}

void
QGaimBListWindow::chat()
{
}

void
QGaimBListWindow::showAccountsWindow()
{
	qGaimGetHandle()->showAccountsWindow();
}

void
QGaimBListWindow::showConversations()
{
}

void
QGaimBListWindow::blistToggled(bool)
{
	blistButton->setOn(true);
}

/**************************************************************************
 * Gaim callbacks
 **************************************************************************/
static void
signedOnOffCb(GaimConnection *gc, struct gaim_buddy_list *blist)
{
	gc = NULL; blist = NULL;
}

/**************************************************************************
 * blist UI
 **************************************************************************/
static void
qGaimBlistNewList(struct gaim_buddy_list *blist)
{
	QGaimBListWindow *win = qGaimGetHandle()->getBlistWindow();
	blist->ui_data = win;
	win->setGaimBlist(blist);

	/* Setup some signal handlers */
	gaim_signal_connect(blist->ui_data, event_signon,
						(void *)signedOnOffCb, blist);
	gaim_signal_connect(blist->ui_data, event_signoff,
						(void *)signedOnOffCb, blist);
}

static void
qGaimBlistNewNode(GaimBlistNode *)
{
}

static void
qGaimBlistShow(struct gaim_buddy_list *blist)
{
	QGaimBListWindow *blist_win = (QGaimBListWindow *)blist->ui_data;

	blist_win->show();
}

static void
qGaimBlistUpdate(struct gaim_buddy_list *blist, GaimBlistNode *node)
{
	QGaimBListWindow *blist_win = (QGaimBListWindow *)blist->ui_data;
	blist_win->update(node);
}

static void
qGaimBlistRemove(struct gaim_buddy_list *, GaimBlistNode *node)
{
	QGaimBListItem *item = (QGaimBListItem *)node->ui_data;

	delete item;

	node->ui_data = NULL;
}

static void
qGaimBlistDestroy(struct gaim_buddy_list *blist)
{
	QGaimBListWindow *qblist = (QGaimBListWindow *)blist->ui_data;

	delete qblist;

	blist->ui_data = NULL;
}

static void
qGaimBlistSetVisible(struct gaim_buddy_list *blist, gboolean show)
{
	QGaimBListWindow *qblist = (QGaimBListWindow *)blist->ui_data;

	if (show)
		qblist->show();
	else
		qblist->hide();
}

static struct gaim_blist_ui_ops blistUiOps =
{
	qGaimBlistNewList,
	qGaimBlistNewNode,
	qGaimBlistShow,
	qGaimBlistUpdate,
	qGaimBlistRemove,
	qGaimBlistDestroy,
	qGaimBlistSetVisible
};

struct gaim_blist_ui_ops *
qGaimGetBlistUiOps()
{
	return &blistUiOps;
}