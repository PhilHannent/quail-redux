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
#include "QGaimBuddyList.h"
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

#include <qpe/resource.h>
#include <qaction.h>
#include <qbutton.h>
#include <qheader.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlistview.h>
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
						 size_t length, GaimMessageFlags flags, time_t)
{
	if (text == NULL)
		return;

	if (length == (size_t)-1)
		length = strlen(message);

	QString txt;

	if (flags & GAIM_MESSAGE_SYSTEM)
	{
		txt  = "<b>";
		txt += message;
		txt += "</b><br>\n";
	}
	else
	{
		QString color, nick;
		char *newMessage;

		newMessage = (char *)g_memdup(message, length + 1);

		if (flags & GAIM_MESSAGE_WHISPER)
		{
			/* If we're whispering, it's not an auto-response. */
			if (meify(newMessage, length))
			{
				nick  = "***";
				nick += who;
				color = "#6C2585";
			}
			else
			{
				nick  = "*";
				nick += who;
				nick += "*:";
				color = "#00FF00";
			}
		}
		else
		{
			if (meify(newMessage, length))
			{
				if (flags & GAIM_MESSAGE_AUTO_RESP)
					nick = "&lt;AUTO-REPLY&gt; ***";
				else
					nick = "***";

				nick += who;

				if (flags & GAIM_MESSAGE_NICK)
					color = "#AF7F00";
				else
					color = "#062585";
			}
			else
			{
				nick = who;

				if (flags & GAIM_MESSAGE_AUTO_RESP)
					nick += "&lt;AUTO-REPLY&gt; :";
				else
					nick += ":";

				if (flags & GAIM_MESSAGE_NICK)
					color = "#AF7F00";
				else if (flags & GAIM_MESSAGE_RECV)
					color = "#A82F2F";
				else if (flags & GAIM_MESSAGE_SEND)
					color = "#16569E";
			}
		}

		txt  = "<font color=\"" + color + "\"><b>";
		txt += nick;
		txt += "</b></font> ";

		txt += stripFontFace(newMessage);
		txt += "<br>\n";

		g_free(newMessage);
	}

	text->setText(text->text() + txt);
	text->verticalScrollBar()->setValue(text->verticalScrollBar()->maxValue());
}

void
QGaimConversation::setTitle(const char *title)
{
	title = NULL;
}

void
QGaimConversation::updated(GaimConvUpdateType type)
{
	if (type == GAIM_CONV_UPDATE_ACCOUNT)
	{
		gaim_conversation_autoset_title(conv);

		if (gaim_conversation_get_type(conv) == GAIM_CONV_IM)
			; /* Update buddy icon. */

		updateTabIcon();
	}
	else if (type == GAIM_CONV_ACCOUNT_ONLINE ||
			 type == GAIM_CONV_ACCOUNT_OFFLINE)
	{
		updateTabIcon();
	}
	else if (type == GAIM_CONV_UPDATE_AWAY)
	{
		updateTabIcon();
	}
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

bool
QGaimConversation::meify(char *message, int len)
{
	char *c;
	bool insideHtml = false;

	if (message == NULL)
		return false;

	if (len == -1)
		len = strlen(message);

	for (c = message; *c != '\0'; c++, len--)
	{
		if (insideHtml)
		{
			if (*c == '>')
				insideHtml = false;
		}
		else
		{
			if (*c == '<')
				insideHtml = true;
			else
				break;
		}
	}

	if (*c != '\0' && !g_ascii_strncasecmp(c, "/me ", 4))
	{
		memmove(c, c + 4, len - 3);

		return true;
	}

	return false;
}

QString
QGaimConversation::stripFontFace(const QString &str)
{
	QString newStr = str;

	newStr.replace(QRegExp(" FACE=\"?[^\"> ]+\"?", false), "");

	return newStr;
}

void
QGaimConversation::updateTabIcon()
{
	QGaimConvWindow *qwin;
	GaimWindow *win;
	GaimAccount *account;
	struct buddy *b;

	win = gaim_conversation_get_window(conv);
	qwin = (QGaimConvWindow *)win->ui_data;
	account = gaim_conversation_get_account(conv);

	b = gaim_find_buddy(account, gaim_conversation_get_name(conv));

	if (b == NULL)
	{
		qwin->getTabs()->changeTab(this,
				QGaimProtocolUtils::getProtocolIcon(account),
				gaim_conversation_get_title(conv));
	}
	else
	{
		qwin->getTabs()->changeTab(this,
				QGaimBuddyList::getBuddyStatusIcon((GaimBlistNode *)b),
				gaim_conversation_get_title(conv));
	}
}


/**************************************************************************
 * QGaimChat
 **************************************************************************/
QGaimChat::QGaimChat(GaimConversation *conv, QWidget *parent,
					 const char *name, WFlags fl)
	: QGaimConversation(conv, parent, name, fl), chat(GAIM_CHAT(conv))
{
	buildInterface();
}

QGaimChat::~QGaimChat()
{
}

void
QGaimChat::write(const char *who, const char *message, GaimMessageFlags flags,
				 time_t mtime)
{
	gaim_conversation_write(conv, who, message, (size_t)-1, flags, mtime);
}

void
QGaimChat::addUser(const char *user)
{
	QListViewItem *item = new QListViewItem(userList);

	if (gaim_chat_is_user_ignored(chat, user))
		item->setText(0, "X"); /* XXX */
	else
		item->setText(0, " ");

	item->setText(1, user);
	userList->insertItem(item);
}

void
QGaimChat::addUsers(GList *users)
{
	for (GList *l = users; l != NULL; l = l->next)
		addUser((const char *)l->data);
}

void
QGaimChat::renameUser(const char *oldName, const char *newName)
{
	QListViewItem *item;

	for (item = userList->firstChild();
		 item != NULL;
		 item = item->nextSibling())
	{
		if (item->text(1) == oldName)
		{
			item->setText(1, newName);
			break;
		}
	}
}

void
QGaimChat::removeUser(const char *user)
{
	QListViewItem *item;

	for (item = userList->firstChild();
		 item != NULL;
		 item = item->nextSibling())
	{
		if (item->text(1) == user)
		{
			delete item;
			break;
		}
	}
}

void
QGaimChat::removeUsers(GList *users)
{
	for (GList *l = users; l != NULL; l = l->next)
		removeUser((const char *)l->data);
}

void
QGaimChat::buildInterface()
{
	QGridLayout *l = new QGridLayout(this, 2, 1, 5, 5);

	text  = new QTextView(this);

	entry = new QGaimMultiLineEdit(this);
	entry->setWordWrap(QMultiLineEdit::WidgetWidth);
	entry->setHistoryEnabled(true);
	entry->setFixedVisibleLines(5);

	userList = new QListView(this);
	userList->addColumn(tr("Ignored"), 5);
	userList->addColumn(tr("User"), -1);
	userList->setAllColumnsShowFocus(true);
	userList->setRootIsDecorated(false);
	userList->header()->hide();
	userList->setSorting(1);
	userList->hide();

	QPEApplication::setStylusOperation(userList->viewport(),
									   QPEApplication::RightOnHold);

	l->addWidget(text,  0, 0);
	l->addWidget(userList,  0, 1);
	l->addMultiCellWidget(entry, 1, 1, 0, 1);

	connect(entry, SIGNAL(returnPressed()),
			this, SLOT(returnPressed()));

	entry->setFocus();
}

void
QGaimChat::focusInEvent(QFocusEvent *)
{
	entry->setFocus();
	text->verticalScrollBar()->setValue(text->verticalScrollBar()->maxValue());
}

void
QGaimChat::setShowUserList(bool show)
{
	if (show)
		userList->show();
	else
		userList->hide();

	text->verticalScrollBar()->setValue(text->verticalScrollBar()->maxValue());
}

bool
QGaimChat::getShowUserList() const
{
	return userList->isVisible();
}

void
QGaimChat::send()
{
	QString text = entry->text();

	if (text[text.length() - 1] == '\n')
		text.remove(text.length() - 1, 1);

	gaim_chat_send(GAIM_CHAT(conv), text);

	entry->setText("");
}

void
QGaimChat::returnPressed()
{
	send();
}

void
QGaimChat::updated(GaimConvUpdateType type)
{
	if (type == GAIM_CONV_UPDATE_UNSEEN)
	{
		QGaimConvWindow *qwin;
		GaimWindow *win;
		QColor color;

		color = black;

		win = gaim_conversation_get_window(conv);
		qwin = (QGaimConvWindow *)win->ui_data;

		if (gaim_conversation_get_unseen(conv) == GAIM_UNSEEN_NICK)
		{
			color.setRgb(0x31, 0x4E, 0x6C);
		}
		else if (gaim_conversation_get_unseen(conv) == GAIM_UNSEEN_EVENT)
		{
			color.setRgb(0x86, 0x82, 0x72);
		}
		else if (gaim_conversation_get_unseen(conv) == GAIM_UNSEEN_TEXT)
		{
			color.setRgb(0xDF, 0x42, 0x1E);
		}

		qwin->getTabs()->setTabColor(getTabId(), color);
	}

	QGaimConversation::updated(type);
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
			   GaimMessageFlags flags, time_t mtime)
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
		else if (gaim_conversation_get_unseen(conv) == GAIM_UNSEEN_EVENT)
		{
			color.setRgb(0x86, 0x82, 0x72);
		}
		else if (gaim_conversation_get_unseen(conv) == GAIM_UNSEEN_TEXT)
		{
			color.setRgb(0xDF, 0x42, 0x1E);
		}

		win = gaim_conversation_get_window(conv);
		qwin = (QGaimConvWindow *)win->ui_data;

		qwin->getTabs()->setTabColor(getTabId(), color);
	}

	QGaimConversation::updated(type);
}

void
QGaimIm::buildInterface()
{
	QGridLayout *l = new QGridLayout(this, 2, 1, 5, 5);

	text  = new QTextView(this);
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
	text->verticalScrollBar()->setValue(text->verticalScrollBar()->maxValue());
}

void
QGaimIm::send()
{
	QString text = entry->text();

	if (text[text.length() - 1] == '\n')
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
	: QMainWindow(), win(win), convWinId(0)
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
	if (index == (unsigned int)tabs->getCurrentIndex())
		return;

	tabs->setCurrentIndex(index);
}

void
QGaimConvWindow::addConversation(GaimConversation *conv)
{
	QGaimConversation *qconv = NULL;
	GaimAccount *account;
	struct buddy *b;

	if (gaim_conversation_get_type(conv) == GAIM_CONV_IM)
		qconv = new QGaimIm(conv, tabs);
	else if (gaim_conversation_get_type(conv) == GAIM_CONV_CHAT)
		qconv = new QGaimChat(conv);
	else
		return;

	conv->ui_data = qconv;

	account = gaim_conversation_get_account(conv);

	b = gaim_find_buddy(account, gaim_conversation_get_name(conv));

	if (b == NULL)
	{
		tabs->addTab(qconv,
					 QGaimProtocolUtils::getProtocolIcon(account),
					 gaim_conversation_get_title(conv));
	}
	else
	{
		tabs->addTab(qconv,
					 QGaimBuddyList::getBuddyStatusIcon((GaimBlistNode *)b),
					 gaim_conversation_get_title(conv));
	}

	qconv->setTabId(tabs->getLastId());

	if (gaim_window_get_conversation_count(win) == 1)
	{
		qGaimGetHandle()->getMainWindow()->setCaption(
				gaim_conversation_get_title(conv));
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
	int currentId = tabs->currentPageIndex();
	int currentIndex = -1;
	GList *l;

	for (l = gaim_window_get_conversations(win); l != NULL; l = l->next)
	{
		GaimConversation *conv   = (GaimConversation *)l->data;
		QGaimConversation *qconv = (QGaimConversation *)conv->ui_data;

		if (qconv->getTabId() == currentId)
			return gaim_conversation_get_index(conv);
	}

	return currentIndex;
}

QGaimTabWidget *
QGaimConvWindow::getTabs() const
{
	return tabs;
}

void
QGaimConvWindow::setId(int id)
{
	convWinId = id;
}

int
QGaimConvWindow::getId() const
{
	return convWinId;
}

void
QGaimConvWindow::tabChanged(QWidget *widget)
{
	QGaimConversation *qconv = (QGaimConversation *)widget;
	GaimConversation *conv = qconv->getGaimConversation();

	gaim_conversation_set_unseen(conv, GAIM_UNSEEN_NONE);

	if (conv != NULL)
	{
		qGaimGetHandle()->getMainWindow()->setCaption(
				gaim_conversation_get_title(conv));
	}

	if (gaim_conversation_get_type(conv) == GAIM_CONV_CHAT)
	{
		QGaimChat *qchat = (QGaimChat *)qconv;

		userListToggle->setEnabled(true);
		userListToggle->setOn(qchat->getShowUserList());
	}
	else
		userListToggle->setEnabled(false);

	qconv->setFocus();
}

void
QGaimConvWindow::destroy(bool destroyWindow, bool destroySubWindows)
{
	gaim_window_destroy(win);

	QMainWindow::destroy(destroyWindow, destroySubWindows);

	if (qGaimGetHandle()->getLastActiveConvWindow() == getGaimWindow())
		qGaimGetHandle()->setLastActiveConvWindow(NULL);
}

void
QGaimConvWindow::closeEvent(QCloseEvent *e)
{
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
QGaimConvWindow::userListToggled(bool on)
{
	/* Make sure this is a chat. */
	GaimConversation *conv;
	QGaimChat *qchat;

	conv = gaim_window_get_active_conversation(win);

	if (gaim_conversation_get_type(conv) != GAIM_CONV_CHAT)
		return;

	qchat = (QGaimChat *)conv->ui_data;

	qchat->setShowUserList(on);
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
	QAction *a;

	setToolBarsMovable(false);

	toolbar = new QToolBar(this);

	/* Close Conversation */
	a = new QAction(tr("Close"),
					QIconSet(Resource::loadPixmap("gaim/close_conv")),
					QString::null, 0, this, 0);
	a->addTo(toolbar);

	connect(a, SIGNAL(activated()),
			this, SLOT(closeConv()));

	/* Separator */
	toolbar->addSeparator();

	/* Person actions */
	button = new QToolButton(toolbar, "blist");
	button->setAutoRaise(true);
	button->setPixmap(Resource::loadPixmap("gaim/user"));
	button->setEnabled(false);

	userMenu = new QPopupMenu(button);
	button->setPopup(userMenu);
	button->setPopupDelay(0);

	/* Warn */
	a = new QAction(tr("Warn"),
					QIconSet(Resource::loadPixmap("gaim/warn")),
					QString::null, 0, this, 0);
	warnButton = a;
	a->addTo(userMenu);
	a->setEnabled(false);

	/* Block */
	a = new QAction(tr("Block"),
					QIconSet(Resource::loadPixmap("gaim/block")),
					QString::null, 0, this, 0);
	blockButton = a;
	a->addTo(userMenu);
	a->setEnabled(false);

	/* Add */
	a = new QAction(tr("Add"),
					QIconSet(Resource::loadPixmap("gaim/add")),
					QString::null, 0, this, 0);
	addButton = a;
	a->addTo(userMenu);
	a->setEnabled(false);

	/* Remove */
	a = new QAction(tr("Remove"),
					QIconSet(Resource::loadPixmap("gaim/remove")),
					QString::null, 0, this, 0);
	removeButton = a;
	a->setEnabled(false);

	/* Info */
	a = new QAction(tr("Get Information"),
					QIconSet(Resource::loadPixmap("gaim/info")),
					QString::null, 0, this, 0);
	infoButton = a;
	a->addTo(userMenu);
	a->setEnabled(false);


	/* Formatting */
	a = new QAction(tr("Formatting"),
					QIconSet(Resource::loadPixmap("gaim/formatting")),
					QString::null, 0, this, 0);
	a->addTo(toolbar);
	a->setEnabled(false);

	/* Separator */
	toolbar->addSeparator();

	/* Send */
	a = new QAction(tr("Send"),
					QIconSet(Resource::loadPixmap("gaim/send-im")),
					QString::null, 0, this, 0);
	a->addTo(toolbar);

	connect(a, SIGNAL(activated()),
			this, SLOT(send()));

	/* Separator */
	toolbar->addSeparator();

	/* User List toggle */
	userListToggle = new QAction(tr("Show User List"),
								 Resource::loadPixmap("gaim/userlist"),
								 QString::null, 0, this, 0, true);
	userListToggle->addTo(toolbar);
	userListToggle->setEnabled(false);

	connect(userListToggle, SIGNAL(toggled(bool)),
			this, SLOT(userListToggled(bool)));

	/* Add some whitespace. */
	label = new QLabel(toolbar);
	label->setText("");
	toolbar->setStretchableWidget(label);

	/* Now we're going to construct the toolbar on the right. */
	toolbar->addSeparator();

	/* Buddy List */
	a = new QAction(tr("Buddy List"), Resource::loadPixmap("gaim/blist"),
					QString::null, 0, this, 0);
	a->addTo(toolbar);

	connect(a, SIGNAL(activated()),
			this, SLOT(showBlist()));

	/* Accounts */
	a = new QAction(tr("Accounts"),
					Resource::loadPixmap("gaim/accounts"),
					QString::null, 0, this, 0);
	a->addTo(toolbar);

	connect(a, SIGNAL(activated()),
			this, SLOT(showAccountsWindow()));

	/* Conversations */
	button = new QGaimConvButton(toolbar, "conversations");
	button->setOn(true);
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
				   const char *message, GaimMessageFlags flags, time_t mtime)
{
	QGaimChat *qchat = (QGaimChat *)conv->ui_data;

	qchat->write(who, message, flags, mtime);
}

static void
qGaimConvWriteIm(GaimConversation *conv, const char *who,
				 const char *message, size_t len, GaimMessageFlags flags,
				 time_t mtime)
{
	QGaimIm *qim = (QGaimIm *)conv->ui_data;

	qim->write(who, message, len, flags, mtime);
}

static void
qGaimConvWriteConv(GaimConversation *conv, const char *who,
				   const char *message, size_t length, GaimMessageFlags flags,
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
qGaimConvChatAddUsers(GaimConversation *conv, GList *users)
{
	QGaimChat *qchat = (QGaimChat *)conv->ui_data;

	qchat->addUsers(users);
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
qGaimConvChatRemoveUsers(GaimConversation *conv, GList *users)
{
	QGaimChat *qchat = (QGaimChat *)conv->ui_data;

	qchat->removeUsers(users);
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
	qGaimConvChatAddUsers,
	qGaimConvChatRenameUser,
	qGaimConvChatRemoveUser,
	qGaimConvChatRemoveUsers,
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

	qGaimGetHandle()->addConversationWindow(qwin);

	qGaimGetHandle()->getWidgetStack()->addWidget(qwin, qwin->getId());
	qGaimGetHandle()->getWidgetStack()->raiseWidget(qwin->getId());

	win->ui_data = qwin;
}

static void
qGaimWindowDestroy(GaimWindow *win)
{
	QGaimConvWindow *qwin = (QGaimConvWindow *)win->ui_data;

	qGaimGetHandle()->removeConversationWindow(qwin);

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

	gaim_debug(GAIM_DEBUG_MISC, "qGaimWindowRaise",
			   "Raising window with ID %d\n", qwin->getId());
	qGaimGetHandle()->getWidgetStack()->raiseWidget(qwin->getId());
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

