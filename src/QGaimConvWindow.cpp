/**
 * @file QGaimConvWindow.cpp Conversation windows
 *
 * @Copyright (C) 2003 Christian Hammond.
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
#include "QGaimConvWindow.h"
#include "QGaimConvButton.h"
#include "QGaimMultiLineEdit.h"
#include "QGaimProtocolUtils.h"
#include "QGaimTabBar.h"
#include "QGaimTabWidget.h"
#include "QGaim.h"
#include "base.h"

#include <libgaim/debug.h>
#include <libgaim/prefs.h>

#include <qaction.h>
#include <qbutton.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpopupmenu.h>
#include <qsplitter.h>
#include <qtabwidget.h>
#include <qtextview.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qvbox.h>

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
	text->verticalScrollBar()->setValue(text->verticalScrollBar()->maxValue());

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

void
QGaimConversation::setTabId(int id)
{
	tabId = id;
}

int
QGaimConversation::getTabId() const
{
	return tabId;
}

/**************************************************************************
 * QGaimChat
 **************************************************************************/
QGaimChat::QGaimChat(GaimConversation *conv, QWidget *parent,
					 const char *name, WFlags fl)
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

void
QGaimChat::send()
{
	QString text = entry->text();

	text.remove(text.length() - 1, 1);

	gaim_chat_send(GAIM_CHAT(conv), text);

	entry->setText("");
}


/**************************************************************************
 * QGaimIm
 **************************************************************************/
QGaimIm::QGaimIm(GaimConversation *conv, QWidget *parent,
				 const char *name, WFlags fl)
	: QGaimConversation(conv, parent, name, fl), im(GAIM_IM(conv))
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
QGaimIm::updated(GaimConvUpdateType type)
{
	if (type == GAIM_CONV_UPDATE_TYPING ||
		type == GAIM_CONV_UPDATE_UNSEEN)
	{
		QGaimConvWindow *qwin;
		GaimWindow *win;
		QColor color;

		color = black;

		if (gaim_im_get_typing_state(GAIM_IM(conv)) == GAIM_TYPING)
		{
			color.setRgb(0x46, 0xA0, 0x46);
		}
		else if (gaim_im_get_typing_state(GAIM_IM(conv)) == GAIM_TYPED)
		{
			color.setRgb(0xD1, 0x94, 0x0C);
		}
		else if (gaim_conversation_get_unseen(conv) == GAIM_UNSEEN_NICK)
		{
			color.setRgb(0x31, 0x4E, 0x6C);
		}
		else if (gaim_conversation_get_unseen(conv) == GAIM_UNSEEN_TEXT)
		{
			color.setRgb(0xDF, 0x42, 0x1E);
		}

		win = gaim_conversation_get_window(conv);
		qwin = (QGaimConvWindow *)win->ui_data;

		gaim_debug(GAIM_DEBUG_MISC, "QGaimConvWindow",
				   "tab ID = %d\n", getTabId());
		qwin->getTabs()->setTabColor(getTabId(), color);
	}
	else
		QGaimConversation::updated(type);
}

void
QGaimIm::buildInterface()
{
	QGridLayout *l = new QGridLayout(this, 2, 1, 5, 5);
	QString str;

	text  = new QTextView(str, QString::null, this);
	entry = new QGaimMultiLineEdit(this);
	entry->setWordWrap(QMultiLineEdit::WidgetWidth);
	entry->setHistoryEnabled(true);
	entry->setFixedVisibleLines(5);

	l->addWidget(text,  0, 0);
	l->addWidget(entry, 1, 0);

	connect(entry, SIGNAL(returnPressed()),
			this, SLOT(returnPressed()));
	connect(entry, SIGNAL(textChanged()),
			this, SLOT(updateTyping()));

	entry->setFocus();
}

void
QGaimIm::focusInEvent(QFocusEvent *)
{
	entry->setFocus();
}

void
QGaimIm::send()
{
	QString text = entry->text();

	text.remove(text.length() - 1, 1);

	gaim_im_send(GAIM_IM(conv), text);

	entry->setText("");

	updateTyping();
}

void
QGaimIm::returnPressed()
{
	send();
}

void
QGaimIm::updateTyping()
{
	if (!gaim_prefs_get_bool("/core/conversations/im/send_typing"))
		return;

	if (gaim_im_get_type_again_timeout(im))
		gaim_im_stop_type_again_timeout(im);

	if (entry->text().length() == 0)
	{
		/* We deleted all of it, so we'll keep typing off. */

		serv_send_typing(gaim_conversation_get_gc(conv),
						 gaim_conversation_get_name(conv),
						 GAIM_NOT_TYPING);
	}
	else
	{
		gaim_im_start_type_again_timeout(im);

		if (entry->text().length() == 1 ||
			 (gaim_im_get_type_again(im) != 0 &&
			  time(NULL) > gaim_im_get_type_again(im)))
		{
			int timeout = serv_send_typing(gaim_conversation_get_gc(conv),
										   gaim_conversation_get_name(conv),
										   GAIM_TYPING);

			if (timeout)
				gaim_im_set_type_again(im, time(NULL) + timeout);
			else
				gaim_im_set_type_again(im, 0);
		}
	}
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
	delete userMenu;
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
	tabs->setCurrentPage(index);
}

void
QGaimConvWindow::addConversation(GaimConversation *conv)
{
	QGaimConversation *qconv = NULL;
	QPixmap *pixmap;

	if (gaim_conversation_get_type(conv) == GAIM_CONV_IM)
		qconv = new QGaimIm(conv, tabs);
	else if (gaim_conversation_get_type(conv) == GAIM_CONV_CHAT)
		qconv = new QGaimChat(conv);
	else
		return;

	conv->ui_data = qconv;

	pixmap = QGaimProtocolUtils::getProtocolIcon(
			gaim_conversation_get_account(conv));

	if (pixmap != NULL)
	{
		tabs->addTab(qconv, QIconSet(*pixmap),
					 gaim_conversation_get_title(conv));
	}
	else
	{
		tabs->addTab(qconv, gaim_conversation_get_title(conv));
	}

	delete pixmap;

	qconv->setTabId(tabs->getLastId());

	if (gaim_window_get_conversation_count(win) == 1)
	{
		setCaption(gaim_conversation_get_title(conv));
		tabs->setCurrentPage(0);
	}
}

void
QGaimConvWindow::removeConversation(GaimConversation *conv)
{
	/* NOTE: This deletes conv->ui_data. Find out what to do here. */
	tabs->removePage((QGaimConversation *)conv->ui_data);

	conv->ui_data = NULL;
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
	return tabs->getCurrentIndex();
}

QGaimTabWidget *
QGaimConvWindow::getTabs() const
{
	return tabs;
}

void
QGaimConvWindow::tabChanged(QWidget *widget)
{
	QGaimConversation *qconv = (QGaimConversation *)widget;
	GaimConversation *conv = qconv->getGaimConversation();

	gaim_conversation_set_unseen(conv, GAIM_UNSEEN_NONE);

	if (conv != NULL)
		setCaption(gaim_conversation_get_title(conv));
}

void
QGaimConvWindow::destroy(bool destroyWindow, bool destroySubWindows)
{
	gaim_debug(GAIM_DEBUG_INFO, "QGaimConvWindow", "destroy\n");

	gaim_window_destroy(win);

	QMainWindow::destroy(destroyWindow, destroySubWindows);

	if (qGaimGetHandle()->getLastActiveConvWindow() == getGaimWindow())
		qGaimGetHandle()->setLastActiveConvWindow(NULL);
}

void
QGaimConvWindow::closeEvent(QCloseEvent *e)
{
	gaim_debug(GAIM_DEBUG_INFO, "QGaimConvWindow", "closeEvent\n");

	gaim_window_destroy(win);

	QMainWindow::closeEvent(e);
}

void
QGaimConvWindow::closeConv()
{
	gaim_conversation_destroy(gaim_window_get_active_conversation(win));
}

void
QGaimConvWindow::send()
{
	GaimConversation *conv;
	QGaimConversation *qconv;

	conv = gaim_window_get_active_conversation(win);

	qconv = (QGaimConversation *)conv->ui_data;

	qconv->send();
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
	setupToolbar();

	tabs = new QGaimTabWidget(this, "conv tabs");

	connect(tabs, SIGNAL(currentChanged(QWidget *)),
			this, SLOT(tabChanged(QWidget *)));

	setCentralWidget(tabs);
}

void
QGaimConvWindow::setupToolbar()
{
	QToolButton *button;
	QLabel *label;
	QPixmap *pixmap;
	QAction *a;

	setToolBarsMovable(false);

	toolbar = new QToolBar(this);

	/* Close Conversation */
	a = new QAction(tr("Close"),
					QIconSet(QPixmap(DATA_PREFIX "images/close_conv.png")),
					QString::null, 0, this, 0);
	a->addTo(toolbar);

	connect(a, SIGNAL(activated()),
			this, SLOT(closeConv()));

	/* Separator */
	toolbar->addSeparator();

	pixmap = new QPixmap(DATA_PREFIX "images/user.png");
	button = new QToolButton(toolbar, "blist");
	button->setAutoRaise(true);
	button->setPixmap(*pixmap);
	delete pixmap;

	userMenu = new QPopupMenu(button);
	button->setPopup(userMenu);
	button->setPopupDelay(0);

	/* Warn */
	a = new QAction(tr("Warn"),
					QIconSet(QPixmap(DATA_PREFIX "images/warn.png")),
					QString::null, 0, this, 0);
	warnButton = a;
	a->addTo(userMenu);
	a->setEnabled(false);

	/* Block */
	a = new QAction(tr("Block"),
					QIconSet(QPixmap(DATA_PREFIX "images/block.png")),
					QString::null, 0, this, 0);
	blockButton = a;
	a->addTo(userMenu);
	a->setEnabled(false);

	/* Add */
	a = new QAction(tr("Add"),
					QIconSet(QPixmap(DATA_PREFIX "images/add.png")),
					QString::null, 0, this, 0);
	addButton = a;
	a->addTo(userMenu);
	a->setEnabled(false);

	/* Remove */
	a = new QAction(tr("Remove"),
					QIconSet(QPixmap(DATA_PREFIX "images/remove.png")),
					QString::null, 0, this, 0);
	removeButton = a;
	a->setEnabled(false);

	/* Info */
	a = new QAction(tr("Get Information"),
					QIconSet(QPixmap(DATA_PREFIX "images/info.png")),
					QString::null, 0, this, 0);
	infoButton = a;
	a->addTo(userMenu);
	a->setEnabled(false);


	/* Formatting */
	a = new QAction(tr("Formatting"),
					QIconSet(QPixmap(DATA_PREFIX "images/formatting.png")),
					QString::null, 0, this, 0);
	a->addTo(toolbar);
	a->setEnabled(false);

	/* Separator */
	toolbar->addSeparator();

	/* Send */
	a = new QAction(tr("Send"),
					QIconSet(QPixmap(DATA_PREFIX "images/send-im.png")),
					QString::null, 0, this, 0);
	a->addTo(toolbar);

	connect(a, SIGNAL(activated()),
			this, SLOT(send()));

	/* Add some whitespace. */
	label = new QLabel(toolbar);
	label->setText("");
	toolbar->setStretchableWidget(label);

	/* Now we're going to construct the toolbar on the right. */
	toolbar->addSeparator();

	/* Buddy List */
	pixmap = new QPixmap(DATA_PREFIX "images/blist.png");
	button = new QToolButton(toolbar, "blist");
	button->setAutoRaise(true);
	button->setPixmap(*pixmap);
	delete pixmap;

	connect(button, SIGNAL(clicked()),
			this, SLOT(showBlist()));

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
	button->setOn(true);

	connect(button, SIGNAL(toggled(bool)),
			this, SLOT(conversationsToggled(bool)));
}

/**************************************************************************
 * Conversation UI ops
 **************************************************************************/
static void
qGaimConvDestroy(GaimConversation *conv)
{
	QGaimConversation *qconv = (QGaimConversation *)conv->ui_data;

	if (qconv == NULL)
		return;

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

	gaim_debug(GAIM_DEBUG_INFO, "QGaimConvWindow", "qGaimWindowNew\n");

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
qGaimWindowShow(GaimWindow *win)
{
	QGaimConvWindow *qwin = (QGaimConvWindow *)win->ui_data;

	qGaimGetHandle()->setLastActiveConvWindow(win);

	qwin->show();
}

static void
qGaimWindowHide(GaimWindow *win)
{
	QGaimConvWindow *qwin = (QGaimConvWindow *)win->ui_data;

	qwin->hide();
}

static void
qGaimWindowRaise(GaimWindow *win)
{
	QGaimConvWindow *qwin = (QGaimConvWindow *)win->ui_data;

	qGaimGetHandle()->setLastActiveConvWindow(win);

	qwin->raise();
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

	gaim_debug(GAIM_DEBUG_INFO, "QGaimConvWindow", "add conversation\n");

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
	qGaimWindowShow,
	qGaimWindowHide,
	qGaimWindowRaise,
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

