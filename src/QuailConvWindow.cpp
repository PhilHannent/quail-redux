/**
 * @file QQuailConvWindow.cpp Conversation windows
 *
 * @Copyright (C) 2003-2004 Christian Hammond.
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
#include "QuailBuddyList.h"
#include "QuailConvWindow.h"
#include "QuailConvButton.h"
#include "QuailDialogs.h"
#include "QuailMultiLineEdit.h"
#include "QuailNotify.h"
#include "QuailProtocolUtils.h"
#include "QuailTabBar.h"
#include "QuailTabWidget.h"
#include "QuailMainWindow.h"
#include "base.h"

#include <libpurple/debug.h>
#include <libpurple/prefs.h>

#include <qpe/resource.h>
#include <qpe/qpeapplication.h>

#include <qaction.h>
#include <qbutton.h>
#include <qheader.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlistview.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qsplitter.h>
#include <qtabwidget.h>
#include <qtextview.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qvbox.h>

#include <stdio.h>

/**************************************************************************
 * QQuailConversation
 **************************************************************************/
QQuailConversation::QQuailConversation(GaimConversation *conv,
									 QWidget *parent, const char *name,
									 WFlags fl)
	: QWidget(parent, name, fl), conv(conv), text(NULL), notifying(false)
{
}

QQuailConversation::~QQuailConversation()
{
}

void
QQuailConversation::setGaimConversation(GaimConversation *conv)
{
	this->conv = conv;
}

GaimConversation *
QQuailConversation::getGaimConversation() const
{
	return conv;
}

void
QQuailConversation::write(const char *who, const char *message,
						 GaimMessageFlags flags, time_t)
{
	if (text == NULL)
		return;

	size_t length = strlen(message);

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

	if (flags & GAIM_MESSAGE_RECV)
	{
		GaimConversation *activeConv;
		GaimConvWindow *win;

		win = gaim_conversation_get_window(conv);
		activeConv = gaim_conv_window_get_active_conversation(win);

		if (conv != activeConv ||
			win != qQuailGetMainWindow()->getLastActiveConvWindow())
		{
			const char *prefName;

			if (gaim_conversation_get_type(conv) == GAIM_CONV_CHAT)
				prefName = "/gaim/qpe/notify/incoming_chat";
			else
				prefName = "/gaim/qpe/notify/incoming_im";

			if (gaim_prefs_get_bool(prefName))
			{
				if (notifying)
					qQuailNotifyUserStop();

				qQuailNotifyUser();
				notifying = true;
			}
		}
	}
}

void
QQuailConversation::setTitle(const char *title)
{
	title = NULL;
}

void
QQuailConversation::updated(GaimConvUpdateType type)
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
	else if (type == GAIM_CONV_UPDATE_ADD ||
			 type == GAIM_CONV_UPDATE_REMOVE)
	{
		GaimConvWindow *win = gaim_conversation_get_window(conv);
		QQuailConvWindow *qwin = (QQuailConvWindow *)win->ui_data;

		qwin->updateAddRemoveButton();
	}
}

void
QQuailConversation::setTabId(int id)
{
	tabId = id;
}

int
QQuailConversation::getTabId() const
{
	return tabId;
}

bool
QQuailConversation::meify(char *message, int len)
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
QQuailConversation::stripFontFace(const QString &str)
{
	QString newStr = str;

	newStr.replace(QRegExp(" FACE=\"?[^\"> ]+\"?", false), "");

	return newStr;
}

void
QQuailConversation::updateTabIcon()
{
	QQuailConvWindow *qwin;
	GaimConvWindow *win;
	GaimAccount *account;
	GaimBuddy *b;

	win = gaim_conversation_get_window(conv);
	qwin = (QQuailConvWindow *)win->ui_data;
	account = gaim_conversation_get_account(conv);

	b = gaim_find_buddy(account, gaim_conversation_get_name(conv));

	if (b == NULL)
	{
		qwin->getTabs()->changeTab(this,
			QQuailProtocolUtils::getProtocolIcon(account, QGAIM_PIXMAP_MENU),
			gaim_conversation_get_title(conv));
	}
	else
	{
		qwin->getTabs()->changeTab(this,
				QQuailBuddyList::getBuddyStatusIcon((GaimBlistNode *)b),
				gaim_conversation_get_title(conv));
	}
}

/**************************************************************************
 * QQuailConvChat
 **************************************************************************/
QQuailConvChat::QQuailConvChat(GaimConversation *conv, QWidget *parent,
					 const char *name, WFlags fl)
	: QQuailConversation(conv, parent, name, fl), chat(GAIM_CONV_CHAT(conv))
{
	buildInterface();
}

QQuailConvChat::~QQuailConvChat()
{
}

void
QQuailConvChat::write(const char *who, const char *message,
					 GaimMessageFlags flags, time_t mtime)
{
	gaim_conversation_write(conv, who, message, flags, mtime);
}

void
QQuailConvChat::addUser(const char *user)
{
	QListViewItem *item = new QListViewItem(userList);

	if (gaim_conv_chat_is_user_ignored(chat, user))
		item->setText(0, "X"); /* XXX */
	else
		item->setText(0, " ");

	item->setText(1, user);
	userList->insertItem(item);
}

void
QQuailConvChat::addUsers(GList *users)
{
	for (GList *l = users; l != NULL; l = l->next)
		addUser((const char *)l->data);
}

void
QQuailConvChat::renameUser(const char *oldName, const char *newName)
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
QQuailConvChat::removeUser(const char *user)
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
QQuailConvChat::removeUsers(GList *users)
{
	for (GList *l = users; l != NULL; l = l->next)
		removeUser((const char *)l->data);
}

void
QQuailConvChat::buildInterface()
{
	QGridLayout *l = new QGridLayout(this, 2, 1, 5, 5);

	text  = new QTextView(this);

	entry = new QQuailMultiLineEdit(this);
	entry->setWordWrap(QMultiLineEdit::WidgetWidth);
	entry->setHistoryEnabled(true);
	entry->setFixedVisibleLines(3);

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
	connect(entry, SIGNAL(textChanged()),
			this, SLOT(updateTyping()));

	entry->setFocus();
}

void
QQuailConvChat::focusInEvent(QFocusEvent *)
{
	entry->setFocus();
	text->verticalScrollBar()->setValue(text->verticalScrollBar()->maxValue());
}

void
QQuailConvChat::setShowUserList(bool show)
{
	if (show)
		userList->show();
	else
		userList->hide();

	text->verticalScrollBar()->setValue(text->verticalScrollBar()->maxValue());
}

bool
QQuailConvChat::getShowUserList() const
{
	return userList->isVisible();
}

void
QQuailConvChat::send()
{
	QString text = entry->text();

	if (text[text.length() - 1] == '\n')
		text.remove(text.length() - 1, 1);

	gaim_conv_chat_send(GAIM_CONV_CHAT(conv), text);

	entry->setText("");
}

void
QQuailConvChat::returnPressed()
{
	send();
}

void
QQuailConvChat::updateTyping()
{
	GaimConvWindow *win = gaim_conversation_get_window(conv);
	QQuailConvWindow *qwin = (QQuailConvWindow *)win->ui_data;

	qwin->setSendEnabled(entry->text().length() > 1);
}

void
QQuailConvChat::updated(GaimConvUpdateType type)
{
	if (type == GAIM_CONV_UPDATE_UNSEEN)
	{
		QQuailConvWindow *qwin;
		GaimConvWindow *win;
		QColor color;

		color = black;

		win = gaim_conversation_get_window(conv);
		qwin = (QQuailConvWindow *)win->ui_data;

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

			if (gaim_prefs_get_bool("/gaim/qpe/notify/incoming_chat"))
			{
				qQuailNotifyUser();
				notifying = true;
			}
		}
		else
		{
			if (notifying)
			{
				qQuailNotifyUserStop();
				notifying = false;
			}
		}

		qwin->getTabs()->setTabColor(getTabId(), color);
	}

	QQuailConversation::updated(type);
}

/**************************************************************************
 * QQuailConvIm
 **************************************************************************/
QQuailConvIm::QQuailConvIm(GaimConversation *conv, QWidget *parent,
				 const char *name, WFlags fl)
	: QQuailConversation(conv, parent, name, fl), im(GAIM_CONV_IM(conv))
{
	buildInterface();
}

QQuailConvIm::~QQuailConvIm()
{
}

void
QQuailConvIm::write(const char *who, const char *message,
			   GaimMessageFlags flags, time_t mtime)
{
	gaim_conversation_write(conv, who, message, flags, mtime);
}

void
QQuailConvIm::updated(GaimConvUpdateType type)
{
	if (type == GAIM_CONV_UPDATE_TYPING ||
		type == GAIM_CONV_UPDATE_UNSEEN)
	{
		QQuailConvWindow *qwin;
		GaimConvWindow *win;
		QColor color;
		/* XXX GaimTypingState typingState; */
		int typingState;
		GaimUnseenState unseenState;

		color = black;
		typingState = gaim_conv_im_get_typing_state(GAIM_CONV_IM(conv));
		unseenState = gaim_conversation_get_unseen(conv);

		if (typingState == GAIM_TYPING)
		{
			color.setRgb(0x46, 0xA0, 0x46);
		}
		else if (typingState == GAIM_TYPED)
		{
			color.setRgb(0xD1, 0x94, 0x0C);
		}
		else if (unseenState == GAIM_UNSEEN_NICK)
		{
			color.setRgb(0x31, 0x4E, 0x6C);
		}
		else if (unseenState == GAIM_UNSEEN_EVENT)
		{
			color.setRgb(0x86, 0x82, 0x72);
		}
		else if (unseenState == GAIM_UNSEEN_TEXT)
		{
			color.setRgb(0xDF, 0x42, 0x1E);
		}
		else if (notifying)
		{
			qQuailNotifyUserStop();
			notifying = false;
		}

		win = gaim_conversation_get_window(conv);
		qwin = (QQuailConvWindow *)win->ui_data;

		qwin->getTabs()->setTabColor(getTabId(), color);
	}

	QQuailConversation::updated(type);
}

void
QQuailConvIm::buildInterface()
{
	QGridLayout *l = new QGridLayout(this, 2, 1, 5, 5);

	text  = new QTextView(this);
	entry = new QQuailMultiLineEdit(this);
	entry->setWordWrap(QMultiLineEdit::WidgetWidth);
	entry->setHistoryEnabled(true);
	entry->setFixedVisibleLines(3);

	l->addWidget(text,  0, 0);
	l->addWidget(entry, 1, 0);

	connect(entry, SIGNAL(returnPressed()),
			this, SLOT(returnPressed()));
	connect(entry, SIGNAL(textChanged()),
			this, SLOT(updateTyping()));

	entry->setFocus();
}

void
QQuailConvIm::focusInEvent(QFocusEvent *)
{
	entry->setFocus();
	text->verticalScrollBar()->setValue(text->verticalScrollBar()->maxValue());
}

void
QQuailConvIm::send()
{
	QString text = entry->text();

	if (text[text.length() - 1] == '\n')
		text.remove(text.length() - 1, 1);

	gaim_conv_im_send(GAIM_CONV_IM(conv), text);

	entry->setText("");

	updateTyping();
}

void
QQuailConvIm::returnPressed()
{
	send();
}

void
QQuailConvIm::updateTyping()
{
	GaimConvWindow *win = gaim_conversation_get_window(conv);
	QQuailConvWindow *qwin = (QQuailConvWindow *)win->ui_data;
	size_t length = entry->text().length();

	qwin->setSendEnabled(length > 0);

	if (!gaim_prefs_get_bool("/core/conversations/im/send_typing"))
		return;

	if (gaim_conv_im_get_type_again_timeout(im))
		gaim_conv_im_stop_type_again_timeout(im);

	if (length == 0)
	{
		/* We deleted all of it, so we'll keep typing off. */
		serv_send_typing(gaim_conversation_get_gc(conv),
						 gaim_conversation_get_name(conv),
						 GAIM_NOT_TYPING);
	}
	else
	{
		gaim_conv_im_start_type_again_timeout(im);

		if (length == 1 ||
			 (gaim_conv_im_get_type_again(im) != 0 &&
			  time(NULL) > gaim_conv_im_get_type_again(im)))
		{
			int timeout = serv_send_typing(gaim_conversation_get_gc(conv),
										   gaim_conversation_get_name(conv),
										   GAIM_TYPING);

			if (timeout)
				gaim_conv_im_set_type_again(im, time(NULL) + timeout);
			else
				gaim_conv_im_set_type_again(im, 0);
		}
	}
}

/**************************************************************************
 * QQuailConvWindow
 **************************************************************************/
QQuailConvWindow::QQuailConvWindow(GaimConvWindow *win, QMainWindow *parent)
	: QMainWindow(), parentMainWindow(parent), win(win), convWinId(0)
{
	connect(parent, SIGNAL(pixmapSizeChanged(bool)),
			this, SLOT(setUsesBigPixmaps(bool)));
	setUsesBigPixmaps(parent->usesBigPixmaps());

	buildInterface();
}

QQuailConvWindow::~QQuailConvWindow()
{
	delete userMenu;
}

void
QQuailConvWindow::setGaimConvWindow(GaimConvWindow *win)
{
	this->win = win;
}

GaimConvWindow *
QQuailConvWindow::getGaimConvWindow() const
{
	return win;
}

void
QQuailConvWindow::switchConversation(unsigned int index)
{
	if (index == (unsigned int)tabs->getCurrentIndex())
		return;

	tabs->setCurrentIndex(index);
}

void
QQuailConvWindow::addConversation(GaimConversation *conv)
{
	QQuailConversation *qconv = NULL;
	GaimAccount *account;
	GaimBuddy *b;

	if (gaim_conversation_get_type(conv) == GAIM_CONV_IM)
		qconv = new QQuailConvIm(conv, tabs);
	else if (gaim_conversation_get_type(conv) == GAIM_CONV_CHAT)
		qconv = new QQuailConvChat(conv);
	else
		return;

	conv->ui_data = qconv;

	account = gaim_conversation_get_account(conv);

	b = gaim_find_buddy(account, gaim_conversation_get_name(conv));

	if (b == NULL)
	{
		tabs->addTab(qconv,
			QQuailProtocolUtils::getProtocolIcon(account, QGAIM_PIXMAP_MENU),
			gaim_conversation_get_title(conv));
	}
	else
	{
		tabs->addTab(qconv,
			QQuailBuddyList::getBuddyStatusIcon((GaimBlistNode *)b),
			gaim_conversation_get_title(conv));
	}

	qconv->setTabId(tabs->getLastId());

	if (gaim_conv_window_get_conversation_count(win) == 1)
	{
		qQuailGetMainWindow()->setCaption(gaim_conversation_get_title(conv));
		tabs->setCurrentPage(0);
	}
}

void
QQuailConvWindow::removeConversation(GaimConversation *conv)
{
	/* NOTE: This deletes conv->ui_data. Find out what to do here. */
	tabs->removePage((QQuailConversation *)conv->ui_data);

	conv->ui_data = NULL;
}

void
QQuailConvWindow::moveConversation(GaimConversation *conv,
								  unsigned int newIndex)
{
	conv = NULL; newIndex = 0;
}

int
QQuailConvWindow::getActiveIndex() const
{
	int currentId = tabs->currentPageIndex();
	int currentIndex = -1;
	GList *l;

	for (l = gaim_conv_window_get_conversations(win); l != NULL; l = l->next)
	{
		GaimConversation *conv   = (GaimConversation *)l->data;
		QQuailConversation *qconv = (QQuailConversation *)conv->ui_data;

		if (qconv->getTabId() == currentId)
			return gaim_conversation_get_index(conv);
	}

	return currentIndex;
}

void
QQuailConvWindow::updateAddRemoveButton()
{
	GaimConversation *conv = gaim_conv_window_get_active_conversation(win);

	if (gaim_find_buddy(gaim_conversation_get_account(conv),
						gaim_conversation_get_name(conv)) == NULL)
	{
		addRemoveButton->setText(tr("Add"));
		addRemoveButton->setIconSet(
			QIconSet(Resource::loadPixmap("gaim/16x16/add"),
					 Resource::loadPixmap("gaim/32x32/add")));
	}
	else
	{
		addRemoveButton->setText(tr("Remove"));
		addRemoveButton->setIconSet(
			QIconSet(Resource::loadPixmap("gaim/16x16/remove"),
					 Resource::loadPixmap("gaim/32x32/remove")));
	}
}

void
QQuailConvWindow::setSendEnabled(bool enabled)
{
	sendButton->setEnabled(enabled);
}

QQuailTabWidget *
QQuailConvWindow::getTabs() const
{
	return tabs;
}

void
QQuailConvWindow::setId(int id)
{
	convWinId = id;
}

int
QQuailConvWindow::getId() const
{
	return convWinId;
}

void
QQuailConvWindow::tabChanged(QWidget *widget)
{
	QQuailConversation *qconv = (QQuailConversation *)widget;
	GaimConversation *conv = qconv->getGaimConversation();
	GaimAccount *account = gaim_conversation_get_account(conv);
	GaimPlugin *prpl;
	GaimPluginProtocolInfo *prplInfo = NULL;

	gaim_conversation_set_unseen(conv, GAIM_UNSEEN_NONE);

	if (conv != NULL)
		qQuailGetMainWindow()->setCaption(gaim_conversation_get_title(conv));

	prpl = gaim_plugins_find_with_id(gaim_account_get_protocol_id(account));

	if (prpl != NULL)
		prplInfo = GAIM_PLUGIN_PROTOCOL_INFO(prpl);

	if (gaim_conversation_get_type(conv) == GAIM_CONV_CHAT)
	{
		QQuailConvChat *qchat = (QQuailConvChat *)qconv;

		userMenuButton->setEnabled(false);
		userListToggle->setEnabled(true);
		infoButton->setEnabled(false);
		userListToggle->setOn(qchat->getShowUserList());
	}
	else
	{
		userMenuButton->setEnabled(true);
		userListToggle->setEnabled(false);

		if (prplInfo != NULL && prplInfo->get_info != NULL)
			infoButton->setEnabled(true);
		else
			infoButton->setEnabled(false);

		if (gaim_find_buddy(account, gaim_conversation_get_name(conv)) == NULL)
		{
			addRemoveButton->setText(tr("Add"));
			addRemoveButton->setIconSet(
				QIconSet(Resource::loadPixmap("gaim/16x16/add"),
						 Resource::loadPixmap("gaim/32x32/add")));
		}
		else
		{
			addRemoveButton->setText(tr("Remove"));
			addRemoveButton->setIconSet(
				QIconSet(Resource::loadPixmap("gaim/16x16/remove"),
						 Resource::loadPixmap("gaim/32x32/remove")));
		}
	}

	qconv->setFocus();
}

void
QQuailConvWindow::destroy(bool destroyWindow, bool destroySubWindows)
{
	gaim_conv_window_destroy(win);

	QMainWindow::destroy(destroyWindow, destroySubWindows);

	if (qQuailGetMainWindow()->getLastActiveConvWindow() == getGaimConvWindow())
		qQuailGetMainWindow()->setLastActiveConvWindow(NULL);
}

void
QQuailConvWindow::closeEvent(QCloseEvent *)
{
	gaim_conv_window_destroy(win);
}

void
QQuailConvWindow::closeConv()
{
	gaim_conversation_destroy(gaim_conv_window_get_active_conversation(win));
}

static void
removeBuddyCb(GaimBuddy *buddy)
{
	GaimGroup *group;
	GaimConversation *conv;
	QString name;

	if (buddy == NULL)
		return;

	group = gaim_find_buddys_group(buddy);
	name = buddy->name;

	serv_remove_buddy(gaim_account_get_connection(buddy->account), name,
					  group->name);
	gaim_blist_remove_buddy(buddy);
	gaim_blist_save();

	conv = gaim_find_conversation(name);

	if (conv != NULL)
		gaim_conversation_update(conv, GAIM_CONV_UPDATE_REMOVE);
}

void
QQuailConvWindow::addRemoveBuddySlot()
{
	GaimConversation *conv = gaim_conv_window_get_active_conversation(win);
	GaimAccount *account = gaim_conversation_get_account(conv);
	const char *name = gaim_conversation_get_name(conv);
	GaimBuddy *buddy;

	if ((buddy = gaim_find_buddy(account, name)) == NULL)
	{
		QQuailAddBuddyDialog *dialog;

		dialog = new QQuailAddBuddyDialog(this, "", true);
		dialog->setScreenName(gaim_conversation_get_name(conv));
		dialog->setAccount(gaim_conversation_get_account(conv));

		dialog->showMaximized();
	}
	else
	{
		g_return_if_fail(buddy != NULL);

		int result = QMessageBox::information(this,
				tr("Remove Buddy"),
				tr("<p>You are about to remove %1 from your buddy list.</p>\n"
				   "<p>Do you want to continue?</p>").arg(name),
				tr("&Remove Buddy"), tr("&Cancel"),
				QString::null, 1, 1);

		if (result == 0)
			removeBuddyCb(buddy);
	}
}

void
QQuailConvWindow::userInfoSlot()
{
	GaimConversation *conv = gaim_conv_window_get_active_conversation(win);
	GaimAccount *account = gaim_conversation_get_account(conv);

	serv_get_info(gaim_account_get_connection(account),
				  gaim_conversation_get_name(conv));
}

void
QQuailConvWindow::send()
{
	GaimConversation *conv;
	QQuailConversation *qconv;

	conv = gaim_conv_window_get_active_conversation(win);

	qconv = (QQuailConversation *)conv->ui_data;

	qconv->send();
}

void
QQuailConvWindow::showAccountsWindow()
{
	qQuailGetMainWindow()->showAccountsWindow();
}

void
QQuailConvWindow::userListToggled(bool on)
{
	/* Make sure this is a chat. */
	GaimConversation *conv;
	QQuailConvChat *qchat;

	conv = gaim_conv_window_get_active_conversation(win);

	if (gaim_conversation_get_type(conv) != GAIM_CONV_CHAT)
		return;

	qchat = (QQuailConvChat *)conv->ui_data;

	qchat->setShowUserList(on);
}

void
QQuailConvWindow::showBlist()
{
	qQuailGetMainWindow()->showBlistWindow();
}

void
QQuailConvWindow::buildInterface()
{
	setupToolbar();

	tabs = new QQuailTabWidget(this, "conv tabs");

	connect(tabs, SIGNAL(currentChanged(QWidget *)),
			this, SLOT(tabChanged(QWidget *)));

	setCentralWidget(tabs);
}

void
QQuailConvWindow::setupToolbar()
{
	QToolButton *button;
	QLabel *label;
	QAction *a;

	setToolBarsMovable(false);

	toolbar = new QToolBar(this);

	/* Close Conversation */
	a = new QAction(tr("Close"),
					QIconSet(Resource::loadPixmap("gaim/16x16/close_conv"),
							 Resource::loadPixmap("gaim/32x32/close_conv")),
					QString::null, 0, this, 0);
	a->addTo(toolbar);

	connect(a, SIGNAL(activated()),
			this, SLOT(closeConv()));

	/* Separator */
	toolbar->addSeparator();

	/* Person actions */
	button = new QToolButton(toolbar, "blist");
	userMenuButton = button;
	button->setAutoRaise(true);
	button->setIconSet(QIconSet(Resource::loadPixmap("gaim/16x16/user"),
								Resource::loadPixmap("gaim/32x32/user")));
	button->setEnabled(false);

	userMenu = new QPopupMenu(button);
	button->setPopup(userMenu);
	button->setPopupDelay(0);

	/* Warn */
	a = new QAction(tr("Warn"),
					QIconSet(Resource::loadPixmap("gaim/16x16/warn"),
							 Resource::loadPixmap("gaim/32x32/warn")),
					QString::null, 0, this, 0);
	warnButton = a;
	a->addTo(userMenu);
	a->setEnabled(false);

	/* Block */
	a = new QAction(tr("Block"),
					QIconSet(Resource::loadPixmap("gaim/16x16/block"),
							 Resource::loadPixmap("gaim/32x32/block")),
					QString::null, 0, this, 0);
	blockButton = a;
	a->addTo(userMenu);
	a->setEnabled(false);

	/* Add */
	a = new QAction(tr("Add"),
					QIconSet(Resource::loadPixmap("gaim/16x16/add"),
							 Resource::loadPixmap("gaim/32x32/add")),
					QString::null, 0, this, 0);
	addRemoveButton = a;
	a->addTo(userMenu);

	connect(addRemoveButton, SIGNAL(activated()),
			this, SLOT(addRemoveBuddySlot()));

	/* Info */
	a = new QAction(tr("Get Information"),
					QIconSet(Resource::loadPixmap("gaim/16x16/info"),
							 Resource::loadPixmap("gaim/32x32/info")),
					QString::null, 0, this, 0);
	infoButton = a;
	a->addTo(userMenu);
	a->setEnabled(false);

	connect(infoButton, SIGNAL(activated()),
			this, SLOT(userInfoSlot()));

	/* Formatting */
	a = new QAction(tr("Formatting"),
					QIconSet(Resource::loadPixmap("gaim/16x16/formatting"),
							 Resource::loadPixmap("gaim/32x32/formatting")),
					QString::null, 0, this, 0);
	a->addTo(toolbar);
	a->setEnabled(false);

	/* Separator */
	toolbar->addSeparator();

	/* Send */
	a = new QAction(tr("Send"),
					QIconSet(Resource::loadPixmap("gaim/16x16/send-im"),
							 Resource::loadPixmap("gaim/32x32/send-im")),
					QString::null, 0, this, 0);
	sendButton = a;
	a->addTo(toolbar);
	a->setEnabled(false);

	connect(a, SIGNAL(activated()),
			this, SLOT(send()));

	/* Separator */
	toolbar->addSeparator();

	/* User List toggle */
	userListToggle = new QAction(tr("Show User List"),
		QIconSet(Resource::loadPixmap("gaim/16x16/userlist"),
				 Resource::loadPixmap("gaim/32x32/userlist")),
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
	a = new QAction(tr("Buddy List"),
					QIconSet(Resource::loadPixmap("gaim/16x16/blist"),
							 Resource::loadPixmap("gaim/32x32/blist")),
					QString::null, 0, this, 0);
	a->addTo(toolbar);

	connect(a, SIGNAL(activated()),
			this, SLOT(showBlist()));

	/* Accounts */
	a = new QAction(tr("Accounts"),
					QIconSet(Resource::loadPixmap("gaim/16x16/accounts"),
							 Resource::loadPixmap("gaim/32x32/accounts")),
					QString::null, 0, this, 0);
	a->addTo(toolbar);

	connect(a, SIGNAL(activated()),
			this, SLOT(showAccountsWindow()));

	/* Conversations */
	button = new QQuailConvButton(toolbar, "conversations");
	button->setOn(true);
}

/**************************************************************************
 * Conversation UI ops
 **************************************************************************/
static void
qQuailConvDestroy(GaimConversation *conv)
{
	QQuailConversation *qconv = (QQuailConversation *)conv->ui_data;

	if (qconv == NULL)
		return;

	conv->ui_data = NULL;

	delete qconv;
}

static void
qQuailConvWriteChat(GaimConversation *conv, const char *who,
				   const char *message, GaimMessageFlags flags, time_t mtime)
{
	QQuailConvChat *qchat = (QQuailConvChat *)conv->ui_data;

	qchat->write(who, message, flags, mtime);
}

static void
qQuailConvWriteIm(GaimConversation *conv, const char *who,
				 const char *message, GaimMessageFlags flags, time_t mtime)
{
	QQuailConvIm *qim = (QQuailConvIm *)conv->ui_data;

	qim->write(who, message, flags, mtime);
}

static void
qQuailConvWriteConv(GaimConversation *conv, const char *who,
				   const char *message, GaimMessageFlags flags, time_t mtime)
{
	QQuailConversation *qconv = (QQuailConversation *)conv->ui_data;

	qconv->write(who, message, flags, mtime);
}

static void
qQuailConvChatAddUser(GaimConversation *conv, const char *user)
{
	QQuailConvChat *qchat = (QQuailConvChat *)conv->ui_data;

	qchat->addUser(user);
}

static void
qQuailConvChatAddUsers(GaimConversation *conv, GList *users)
{
	QQuailConvChat *qchat = (QQuailConvChat *)conv->ui_data;

	qchat->addUsers(users);
}

static void
qQuailConvChatRenameUser(GaimConversation *conv, const char *oldName,
						const char *newName)
{
	QQuailConvChat *qchat = (QQuailConvChat *)conv->ui_data;

	qchat->renameUser(oldName, newName);
}

static void
qQuailConvChatRemoveUser(GaimConversation *conv, const char *user)
{
	QQuailConvChat *qchat = (QQuailConvChat *)conv->ui_data;

	qchat->removeUser(user);
}

static void
qQuailConvChatRemoveUsers(GaimConversation *conv, GList *users)
{
	QQuailConvChat *qchat = (QQuailConvChat *)conv->ui_data;

	qchat->removeUsers(users);
}

static void
qQuailConvUpdated(GaimConversation *conv, GaimConvUpdateType type)
{
	QQuailConversation *qconv = (QQuailConversation *)conv->ui_data;

	qconv->updated(type);
}

static GaimConversationUiOps convOps =
{
	qQuailConvDestroy,
	qQuailConvWriteChat,
	qQuailConvWriteIm,
	qQuailConvWriteConv,
	qQuailConvChatAddUser,
	qQuailConvChatAddUsers,
	qQuailConvChatRenameUser,
	qQuailConvChatRemoveUser,
	qQuailConvChatRemoveUsers,
	NULL,
	qQuailConvUpdated
};

static GaimConversationUiOps *
qQuailConvWindowGetConvUiOps()
{
	return &convOps;
}

/**************************************************************************
 * Window UI ops
 **************************************************************************/
static void
qQuailConvWindowNew(GaimConvWindow *win)
{
	QQuailConvWindow *qwin;
	QQuailMainWindow *mainWin = qQuailGetMainWindow();

	qwin = new QQuailConvWindow(win, mainWin);

	mainWin->addConversationWindow(qwin);

	mainWin->getWidgetStack()->addWidget(qwin, qwin->getId());
	mainWin->getWidgetStack()->raiseWidget(qwin->getId());

	win->ui_data = qwin;
}

static void
qQuailConvWindowDestroy(GaimConvWindow *win)
{
	QQuailConvWindow *qwin = (QQuailConvWindow *)win->ui_data;

	qQuailGetMainWindow()->removeConversationWindow(qwin);

	win->ui_data = NULL;
	delete qwin;
}

static void
qQuailConvWindowShow(GaimConvWindow *win)
{
	QQuailConvWindow *qwin = (QQuailConvWindow *)win->ui_data;

	qQuailGetMainWindow()->setLastActiveConvWindow(win);

	qwin->show();
}

static void
qQuailConvWindowHide(GaimConvWindow *win)
{
	QQuailConvWindow *qwin = (QQuailConvWindow *)win->ui_data;

	qwin->hide();
}

static void
qQuailConvWindowRaise(GaimConvWindow *win)
{
	QQuailConvWindow *qwin = (QQuailConvWindow *)win->ui_data;

	qQuailGetMainWindow()->setLastActiveConvWindow(win);

	qQuailGetMainWindow()->getWidgetStack()->raiseWidget(qwin->getId());
}

static void
qQuailConvWindowSwitchConv(GaimConvWindow *win, unsigned int index)
{
	QQuailConvWindow *qwin = (QQuailConvWindow *)win->ui_data;

	qwin->switchConversation(index);
}

static void
qQuailConvWindowAddConv(GaimConvWindow *win, GaimConversation *conv)
{
	QQuailConvWindow *qwin = (QQuailConvWindow *)win->ui_data;

	qwin->addConversation(conv);
}

static void
qQuailConvWindowRemoveConv(GaimConvWindow *win, GaimConversation *conv)
{
	QQuailConvWindow *qwin = (QQuailConvWindow *)win->ui_data;

	qwin->removeConversation(conv);
}

static void
qQuailConvWindowMoveConv(GaimConvWindow *win, GaimConversation *conv,
					unsigned int newIndex)
{
	QQuailConvWindow *qwin = (QQuailConvWindow *)win->ui_data;

	qwin->moveConversation(conv, newIndex);
}

static int
qQuailConvWindowGetActiveIndex(const GaimConvWindow *win)
{
	QQuailConvWindow *qwin = (QQuailConvWindow *)win->ui_data;

	return qwin->getActiveIndex();
}

static GaimConvWindowUiOps winOps =
{
	qQuailConvWindowGetConvUiOps,
	qQuailConvWindowNew,
	qQuailConvWindowDestroy,
	qQuailConvWindowShow,
	qQuailConvWindowHide,
	qQuailConvWindowRaise,
	NULL,
	qQuailConvWindowSwitchConv,
	qQuailConvWindowAddConv,
	qQuailConvWindowRemoveConv,
	qQuailConvWindowMoveConv,
	qQuailConvWindowGetActiveIndex
};

GaimConvWindowUiOps *
qQuailGetConvWindowUiOps()
{
	return &winOps;
}

