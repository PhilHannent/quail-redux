#include "QGaimConvWindow.h"
#include "QGaim.h"
#include "base.h"

#include <libgaim/debug.h>

#include <qmultilineedit.h>
#include <qsplitter.h>
#include <qtabwidget.h>
#include <qtextview.h>
#include <qvbox.h>
#include <qlayout.h>
#include <qbutton.h>
#include <qtoolbutton.h>
#include <qmenubar.h>
#include <qaction.h>

#include <stdio.h>


/**************************************************************************
 * QGaimConversation
 **************************************************************************/
QGaimConversation::QGaimConversation(GaimConversation *conv,
									 QWidget *parent, const char *name,
									 WFlags fl)
	: QWidget(parent, name, fl), conv(conv), text(NULL)
{
}

QGaimConversation::~QGaimConversation()
{
}

void
QGaimConversation::setGaimConversation(GaimConversation *conv)
{
	this->conv = conv;
}

GaimConversation *
QGaimConversation::getGaimConversation() const
{
	return conv;
}

void
QGaimConversation::write(const char *who, const char *message,
						 size_t length, int flags, time_t mtime)
{
	if (text == NULL)
		return;

	QString txt;

	if (flags & WFLAG_SYSTEM)
	{
		txt  = "<b>";
		txt += message;
		txt += "</b><br>\n";
	}
	else
	{
		QString color, nick;

		nick = who;

		if (flags & WFLAG_AUTO)
		{
			nick += "&lt;AUTO-REPLY&gt; :";
		}
		else
			nick += ":";

		if (flags & WFLAG_NICK)
			color = "#AF7F00";
		else if (flags & WFLAG_RECV)
			color = "#A82F2F";
		else if (flags & WFLAG_SEND)
			color = "#16569E";

		txt  = "<font color=\"" + color + "\"><b>";
		txt += nick;
		txt += "</b></font> ";

		txt += message;
		txt += "<br>\n";
	}

	text->setText(text->text() + txt);

	length = 0; flags = 0; mtime = 0;
}

void
QGaimConversation::setTitle(const char *title)
{
	title = NULL;
}

void
QGaimConversation::updated(GaimConvUpdateType)
{
}

/**************************************************************************
 * QGaimChat
 **************************************************************************/
QGaimChat::QGaimChat(GaimConversation *conv, QWidget *parent = NULL,
					 const char *name = NULL, WFlags fl = 0)
	: QGaimConversation(conv, parent, name, fl)
{
	buildInterface();
}

QGaimChat::~QGaimChat()
{
}

void
QGaimChat::write(const char *who, const char *message, int flags,
				 time_t mtime)
{
	gaim_conversation_write(conv, who, message, (size_t)-1, flags, mtime);
}

void
QGaimChat::addUser(const char *user)
{
	user = NULL;
}

void
QGaimChat::renameUser(const char *oldName, const char *newName)
{
	oldName = NULL; newName = NULL;
}

void
QGaimChat::removeUser(const char *user)
{
	user = NULL;
}

void
QGaimChat::buildInterface()
{
}

/**************************************************************************
 * QGaimIm
 **************************************************************************/
QGaimIm::QGaimIm(GaimConversation *conv, QWidget *parent = NULL,
				 const char *name = NULL, WFlags fl = 0)
	: QGaimConversation(conv, parent, name, fl)
{
	buildInterface();
}

QGaimIm::~QGaimIm()
{
}

void
QGaimIm::write(const char *who, const char *message, size_t len,
			   int flags, time_t mtime)
{
	gaim_conversation_write(conv, who, message, len, flags, mtime);
}

void
QGaimIm::buildInterface()
{
	QGridLayout *l = new QGridLayout(this, 2, 1, 5, 5);
	QString str;

	text  = new QTextView(str, QString::null, this);
	entry = new QMultiLineEdit(this);
	entry->setFixedVisibleLines(5);

	l->addWidget(text, 0, 0);
	l->addWidget(entry, 1, 0);

//	l->setRowStretch(0, 0);
//	l->setRowStretch(1, 1);
}

/**************************************************************************
 * QGaimConvWindow
 **************************************************************************/
QGaimConvWindow::QGaimConvWindow(GaimWindow *win)
	: QMainWindow(), win(win)
{
	buildInterface();
}

QGaimConvWindow::~QGaimConvWindow()
{
}

void
QGaimConvWindow::setGaimWindow(GaimWindow *win)
{
	this->win = win;
}

GaimWindow *
QGaimConvWindow::getGaimWindow() const
{
	return win;
}

void
QGaimConvWindow::switchConversation(unsigned int index)
{
	GaimConversation *conv = gaim_window_get_conversation_at(win, index);

	if (conv != NULL)
		setCaption(gaim_conversation_get_title(conv));

	tabs->setCurrentPage(index);
}

void
QGaimConvWindow::addConversation(GaimConversation *conv)
{
	QGaimConversation *qconv;

//	if (gaim_conversation_get_type(conv) == GAIM_CONV_IM)
		qconv = new QGaimIm(conv, tabs);
//	else if (gaim_conversation_get_type(conv) == GAIM_CONV_CHAT)
//		qconv = new QGaimChat(conv);

	conv->ui_data = qconv;

	tabs->addTab(qconv, gaim_conversation_get_title(conv));

	if (gaim_window_get_conversation_count(win) == 1)
	{
		setCaption(gaim_conversation_get_title(conv));
		tabs->setCurrentPage(0);
	}
}

void
QGaimConvWindow::removeConversation(GaimConversation *conv)
{
	tabs->removePage((QGaimConvWindow *)conv->ui_data);
}

void
QGaimConvWindow::moveConversation(GaimConversation *conv,
								  unsigned int newIndex)
{
	conv = NULL; newIndex = 0;
}

int
QGaimConvWindow::getActiveIndex() const
{
	return tabs->currentPageIndex();
}

void
QGaimConvWindow::showAccountsWindow()
{
	qGaimGetHandle()->showAccountsWindow();
}

void
QGaimConvWindow::conversationsToggled(bool)
{
	convsButton->setOn(true);
}

void
QGaimConvWindow::showBlist()
{
	qGaimGetHandle()->showBlistWindow();
}

void
QGaimConvWindow::buildInterface()
{
	buildMenuBar();

	tabs = new QTabWidget(this, "conv tabs");

	setCentralWidget(tabs);
}

QMenuBar *
QGaimConvWindow::buildMenuBar()
{
	QToolButton *button;
	QPixmap *pixmap;
	QAction *a;

	menubar = new QMenuBar(this);

	/* Conversation menu. */
	convMenu = new QPopupMenu(this);
	menubar->insertItem(tr("&Conversation"), convMenu);

	/* Close */
	a = new QAction(tr("&Close"), QString::null, 0, this, 0);
	a->addTo(convMenu);


	/* Now we're going to construct the toolbar on the right. */
	menubar->insertSeparator();

	/* Buddy List */
	pixmap = new QPixmap(DATA_PREFIX "images/blist.png");
	button = new QToolButton(NULL, "blist");
	button->setAutoRaise(true);
	button->setPixmap(*pixmap);
	menubar->insertItem(button);
	delete pixmap;

	connect(button, SIGNAL(clicked()),
			this, SLOT(showBlist()));

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
	pixmap = new QPixmap(DATA_PREFIX "images/conversations.png");
	convsButton = button = new QToolButton(NULL, "conversations");
	button->setAutoRaise(true);
	button->setPixmap(*pixmap);
	button->setToggleButton(true);
	button->setOn(true);
	menubar->insertItem(button);
	delete pixmap;

	connect(button, SIGNAL(toggled(bool)),
			this, SLOT(conversationsToggled(bool)));

	return menubar;
}

/**************************************************************************
 * Conversation UI ops
 **************************************************************************/
static void
qGaimConvDestroy(GaimConversation *conv)
{
	QGaimConversation *qconv = (QGaimConversation *)conv->ui_data;

	conv->ui_data = NULL;
	delete qconv;
}

static void
qGaimConvWriteChat(GaimConversation *conv, const char *who,
				   const char *message, int flags, time_t mtime)
{
	QGaimChat *qchat = (QGaimChat *)conv->ui_data;

	qchat->write(who, message, flags, mtime);
}

static void
qGaimConvWriteIm(GaimConversation *conv, const char *who,
				 const char *message, size_t len, int flags, time_t mtime)
{
	QGaimIm *qim = (QGaimIm *)conv->ui_data;

	qim->write(who, message, len, flags, mtime);
}

static void
qGaimConvWriteConv(GaimConversation *conv, const char *who,
				   const char *message, size_t length, int flags,
				   time_t mtime)
{
	QGaimConversation *qconv = (QGaimConversation *)conv->ui_data;

	qconv->write(who, message, length, flags, mtime);
}

static void
qGaimConvChatAddUser(GaimConversation *conv, const char *user)
{
	QGaimChat *qchat = (QGaimChat *)conv->ui_data;

	qchat->addUser(user);
}

static void
qGaimConvChatRenameUser(GaimConversation *conv, const char *oldName,
						const char *newName)
{
	QGaimChat *qchat = (QGaimChat *)conv->ui_data;

	qchat->renameUser(oldName, newName);
}

static void
qGaimConvChatRemoveUser(GaimConversation *conv, const char *user)
{
	QGaimChat *qchat = (QGaimChat *)conv->ui_data;

	qchat->removeUser(user);
}

static void
qGaimConvSetTitle(GaimConversation *conv, const char *title)
{
	QGaimConversation *qconv = (QGaimConversation *)conv->ui_data;

	qconv->setTitle(title);
}

static void
qGaimConvUpdated(GaimConversation *conv, GaimConvUpdateType type)
{
	QGaimConversation *qconv = (QGaimConversation *)conv->ui_data;

	qconv->updated(type);
}

static GaimConversationUiOps convOps =
{
	qGaimConvDestroy,
	qGaimConvWriteChat,
	qGaimConvWriteIm,
	qGaimConvWriteConv,
	qGaimConvChatAddUser,
	qGaimConvChatRenameUser,
	qGaimConvChatRemoveUser,
	qGaimConvSetTitle,
	NULL,
	qGaimConvUpdated
};

static GaimConversationUiOps *
qGaimWindowGetConvUiOps()
{
	return &convOps;
}

/**************************************************************************
 * Window UI ops
 **************************************************************************/
static void
qGaimWindowNew(GaimWindow *win)
{
	QGaimConvWindow *qwin;

	qwin = new QGaimConvWindow(win);
	win->ui_data = qwin;

	qwin->showMaximized();
}

static void
qGaimWindowDestroy(GaimWindow *win)
{
	QGaimConvWindow *qwin = (QGaimConvWindow *)win->ui_data;

	win->ui_data = NULL;
	delete qwin;
}

static void
qGaimWindowSwitchConv(GaimWindow *win, unsigned int index)
{
	QGaimConvWindow *qwin = (QGaimConvWindow *)win->ui_data;

	qwin->switchConversation(index);
}

static void
qGaimWindowAddConv(GaimWindow *win, GaimConversation *conv)
{
	QGaimConvWindow *qwin = (QGaimConvWindow *)win->ui_data;

	qwin->addConversation(conv);
}

static void
qGaimWindowRemoveConv(GaimWindow *win, GaimConversation *conv)
{
	QGaimConvWindow *qwin = (QGaimConvWindow *)win->ui_data;

	qwin->removeConversation(conv);
}

static void
qGaimWindowMoveConv(GaimWindow *win, GaimConversation *conv,
					unsigned int newIndex)
{
	QGaimConvWindow *qwin = (QGaimConvWindow *)win->ui_data;

	qwin->moveConversation(conv, newIndex);
}

static int
qGaimWindowGetActiveIndex(const GaimWindow *win)
{
	QGaimConvWindow *qwin = (QGaimConvWindow *)win->ui_data;

	return qwin->getActiveIndex();
}

static GaimWindowUiOps winOps =
{
	qGaimWindowGetConvUiOps,
	qGaimWindowNew,
	qGaimWindowDestroy,
	NULL,
	NULL,
	NULL,
	NULL,
	qGaimWindowSwitchConv,
	qGaimWindowAddConv,
	qGaimWindowRemoveConv,
	qGaimWindowMoveConv,
	qGaimWindowGetActiveIndex
};

GaimWindowUiOps *
qGaimGetConvWindowUiOps()
{
	return &winOps;
}

QPopupMenu *
qGaimBuildConvMenu()
{
	QPopupMenu *menu;
	GaimConversation *conv;
	GList *l;

	menu = new QPopupMenu();

	for (l = gaim_get_conversations(); l != NULL; l = l->next)
	{
		conv = (GaimConversation *)l->data;

		gaim_debug(GAIM_DEBUG_INFO, "qGaimBuildConvMenu",
				   "Adding %s\n",
				   gaim_conversation_get_title(conv));

		menu->insertItem(gaim_conversation_get_title(conv));
	}

	return menu;
}
