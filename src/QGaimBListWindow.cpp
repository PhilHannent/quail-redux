#include "QGaimBListWindow.h"
#include "QGaimConvButton.h"
#include "QGaimConvWindow.h"
#include "QGaimProtocolUtils.h"
#include "QGaim.h"
#include "base.h"

#include <qpe/qpeapplication.h>
#include <libgaim/multi.h>
#include <libgaim/debug.h>

#include <qaction.h>
#include <qheader.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlistview.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>

/**************************************************************************
 * QGaimBListItem
 **************************************************************************/
QGaimBListItem::QGaimBListItem(QListView *parent, GaimBlistNode *node)
	: QListViewItem(parent), node(node)
{
	init();
}

QGaimBListItem::QGaimBListItem(QListViewItem *parent, GaimBlistNode *node)
	: QListViewItem(parent), node(node)
{
	init();
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

void
QGaimBListItem::init()
{
	if (GAIM_BLIST_NODE_IS_BUDDY(node))
	{
		struct buddy *buddy = (struct buddy *)node;
		QPixmap *pixmap;

		pixmap = QGaimProtocolUtils::getProtocolIcon(buddy->account);

		if (pixmap != NULL)
		{
			setPixmap(0, *pixmap);
			delete pixmap;
		}
	}
}

/**************************************************************************
 * QGaimBListWindow
 **************************************************************************/
QGaimBListWindow::QGaimBListWindow()
	: QMainWindow(), convsMenu(NULL)
{
	buildInterface();
}

QGaimBListWindow::~QGaimBListWindow()
{
	if (convsMenu != NULL)
		delete convsMenu;
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

	setToolBarsMovable(FALSE);

	/* Setup the toolbar */
	buildToolBar();

	/* Setup the list view */
	buddylist = new QListView(this, "BuddyList");
	buddylist->addColumn(tr("Buddy"), -1);
	buddylist->setAllColumnsShowFocus(TRUE);
	buddylist->setRootIsDecorated(true);
	buddylist->header()->hide();

	QPEApplication::setStylusOperation(buddylist->viewport(),
									   QPEApplication::RightOnHold);

	connect(buddylist, SIGNAL(currentChanged(QListViewItem *)),
			this, SLOT(nodeChanged(QListViewItem *)));
	connect(buddylist, SIGNAL(doubleClicked(QListViewItem *)),
			this, SLOT(sendIm()));

	setCentralWidget(buddylist);
}

void
QGaimBListWindow::buildToolBar()
{
	QPixmap *pixmap;
	QToolButton *button;
	QLabel *label;

	toolbar = new QToolBar(this);
	toolbar->setHorizontalStretchable(TRUE);

	/* IM */
	imButton = button = newButton(toolbar, DATA_PREFIX "images/send-im.png");
	button->setEnabled(false);

	connect(button, SIGNAL(clicked()),
			this, SLOT(sendIm()));

	/* Chat */
	/* TODO */

	/* Get User Info */
	button = infoButton = newButton(toolbar, DATA_PREFIX "images/info.png");
	button->setEnabled(false);

	toolbar->addSeparator();

	/* Add Buddy/Group */
	button = newButton(toolbar, DATA_PREFIX "images/add.png");
	button->setEnabled(false);

	/* New Group */
	button = newButton(toolbar, DATA_PREFIX "images/new-group.png");
	button->setEnabled(false);

	/* Remove */
	button = newButton(toolbar, DATA_PREFIX "images/remove.png");
	button->setEnabled(false);

	/* Add some whitespace. */
	label = new QLabel(toolbar);
	label->setText("");
	toolbar->setStretchableWidget(label);

	/* Now we're going to construct the toolbar on the right. */
	toolbar->addSeparator();

	/* Buddy List */
	pixmap = new QPixmap(DATA_PREFIX "images/blist.png");
	blistButton = button = new QToolButton(toolbar, "blist");
	button->setAutoRaise(true);
	button->setPixmap(*pixmap);
	button->setToggleButton(true);
	button->setOn(true);
	delete pixmap;

	connect(button, SIGNAL(toggled(bool)),
			this, SLOT(blistToggled(bool)));

	/* Accounts */
	pixmap = new QPixmap(DATA_PREFIX "images/accounts.png");
	button = new QToolButton(toolbar, "accounts");
	button->setAutoRaise(true);
	button->setPixmap(*pixmap);
	delete pixmap;

	connect(button, SIGNAL(clicked()),
			this, SLOT(showAccountsWindow()));

	/* Conversations */
	button = new QGaimConvButton(toolbar, "conversations");
}

void
QGaimBListWindow::add_group(GaimBlistNode *node)
{
	QGaimBListItem *item = new QGaimBListItem(buddylist, node);
	node->ui_data = item;

	item->setText(0, ((struct group *)node)->name);

	item->setExpandable(true);
}

QToolButton *
QGaimBListWindow::newButton(QToolBar *toolbar, const QString image,
							bool toggle, bool on)
{
	QPixmap *pixmap;
	QToolButton *button;

	pixmap = new QPixmap(image);
	button = new QToolButton(toolbar);
	button->setAutoRaise(true);
	button->setPixmap(*pixmap);
	button->setToggleButton(toggle);
	button->setOn(on);
	delete pixmap;

	return button;
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
QGaimBListWindow::blistToggled(bool)
{
	blistButton->setOn(true);
}

void
QGaimBListWindow::sendIm()
{
	QGaimBListItem *item;
	GaimBlistNode *node;

	item = (QGaimBListItem *)buddylist->selectedItem();
	node = item->getBlistNode();

	if (GAIM_BLIST_NODE_IS_BUDDY(node))
	{
		struct buddy *buddy = (struct buddy *)item->getBlistNode();
		GaimConversation *conv;
		GaimWindow *win;

		conv = gaim_conversation_new(GAIM_CONV_IM, buddy->account,
									 buddy->name);

		win = gaim_conversation_get_window(conv);
		gaim_window_raise(win);

		gaim_window_switch_conversation(win, gaim_conversation_get_index(conv));
	}
}

void
QGaimBListWindow::nodeChanged(QListViewItem *_item)
{
	QGaimBListItem *item;
	GaimBlistNode *node;

	if (_item == NULL)
	{
		imButton->setEnabled(false);
	}
	else
	{
		item = (QGaimBListItem *)_item;
		node = item->getBlistNode();

		if (GAIM_BLIST_NODE_IS_BUDDY(node))
		{
			imButton->setEnabled(true);
		}
		else
		{
			imButton->setEnabled(false);
		}
	}
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
