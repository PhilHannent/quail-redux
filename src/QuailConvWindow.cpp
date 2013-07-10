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

//#include <qpe/resource.h>
//#include <qpe/qpeapplication.h>

#include <QAction>
#include <QPushButton>
//#include <qheader.h>
#include <QLabel>
#include <QLayout>
#include <QListView>
#include <QMessageBox>
#include <QMenu>
#include <QSplitter>
#include <QTabWidget>
#include <QTextEdit>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>

#include <stdio.h>

/**************************************************************************
 * QQuailConversation
 **************************************************************************/
QQuailConversation::QQuailConversation(PurpleConversation *conv,
									 QWidget *parent, const char *name,
                                     Qt::WindowFlags fl)
	: QWidget(parent, name, fl), conv(conv), text(NULL), notifying(false)
{
}

QQuailConversation::~QQuailConversation()
{
}

void
QQuailConversation::setConversation(PurpleConversation *conv)
{
	this->conv = conv;
}

PurpleConversation *
QQuailConversation::getConversation() const
{
	return conv;
}

void
QQuailConversation::write(const char *who, const char *message,
                         PurpleMessageFlags flags, time_t)
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
        PurpleConversation *activeConv;
        QQuailConvWindow *win;

		win = purple_conversation_get_window(conv);
		activeConv = purple_conv_window_get_active_conversation(win);

		if (conv != activeConv ||
			win != qQuailGetMainWindow()->getLastActiveConvWindow())
		{
			const char *prefName;

			if (purple_conversation_get_type(conv) == GAIM_CONV_CHAT)
                prefName = "/quail/notify/incoming_chat";
			else
                prefName = "/quail/notify/incoming_im";

			if (purple_prefs_get_bool(prefName))
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
QQuailConversation::updated(PurpleConvUpdateType type)
{
	if (type == GAIM_CONV_UPDATE_ACCOUNT)
	{
		purple_conversation_autoset_title(conv);

        if (purple_conversation_get_type(conv) == PURPLE_CONV_IM)
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
        QQuailConvWindow *win = purple_conversation_get_window(conv);
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
    QQuailConvWindow *win;
	PurpleAccount *account;
	PurpleBuddy *b;

	win = purple_conversation_get_window(conv);
	qwin = (QQuailConvWindow *)win->ui_data;
	account = purple_conversation_get_account(conv);

	b = purple_find_buddy(account, purple_conversation_get_name(conv));

	if (b == NULL)
	{
		qwin->getTabs()->changeTab(this,
			QQuailProtocolUtils::getProtocolIcon(account, QGAIM_PIXMAP_MENU),
			purple_conversation_get_title(conv));
	}
	else
	{
		qwin->getTabs()->changeTab(this,
				QQuailBuddyList::getBuddyStatusIcon((PurpleBlistNode *)b),
				purple_conversation_get_title(conv));
	}
}

/**************************************************************************
 * QQuailConvChat
 **************************************************************************/
QQuailConvChat::QQuailConvChat(PurpleConversation *conv, QWidget *parent,
                     const char *name, Qt::WindowFlags fl)
	: QQuailConversation(conv, parent, name, fl), chat(GAIM_CONV_CHAT(conv))
{
	buildInterface();
}

QQuailConvChat::~QQuailConvChat()
{
}

void
QQuailConvChat::write(const char *who, const char *message,
                     PurpleMessageFlags flags, time_t mtime)
{
	purple_conversation_write(conv, who, message, flags, mtime);
}

void
QQuailConvChat::addUser(const char *user)
{
    QListWidgetItem *item = new QListWidgetItem(userList);

	if (purple_conv_chat_is_user_ignored(chat, user))
        item->setText("X"); /* XXX */
	else
        item->setText(" ");

    item->setText(user);
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
    QListWidgetItem *item;

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
    QListWidgetItem *item;

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
    l->addWidget(entry, 1, 1, 0, 1);

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

	purple_conv_chat_send(GAIM_CONV_CHAT(conv), text);

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
    QQuailConvWindow *win = purple_conversation_get_window(conv);
	QQuailConvWindow *qwin = (QQuailConvWindow *)win->ui_data;

	qwin->setSendEnabled(entry->text().length() > 1);
}

void
QQuailConvChat::updated(PurpleConvUpdateType type)
{
	if (type == GAIM_CONV_UPDATE_UNSEEN)
	{
		QQuailConvWindow *qwin;
        QQuailConvWindow *win;
		QColor color;

		color = black;

		win = purple_conversation_get_window(conv);
		qwin = (QQuailConvWindow *)win->ui_data;

		if (purple_conversation_get_unseen(conv) == GAIM_UNSEEN_NICK)
		{
			color.setRgb(0x31, 0x4E, 0x6C);
		}
		else if (purple_conversation_get_unseen(conv) == GAIM_UNSEEN_EVENT)
		{
			color.setRgb(0x86, 0x82, 0x72);
		}
		else if (purple_conversation_get_unseen(conv) == GAIM_UNSEEN_TEXT)
		{
			color.setRgb(0xDF, 0x42, 0x1E);

            if (purple_prefs_get_bool("/quail/notify/incoming_chat"))
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
QQuailConvIm::QQuailConvIm(PurpleConversation *conv, QWidget *parent,
                 const char *name, Qt::WindowFlags fl)
    : QQuailConversation(conv, parent, name, fl), im(PURPLE_CONV_IM(conv))
{
	buildInterface();
}

QQuailConvIm::~QQuailConvIm()
{
}

void
QQuailConvIm::write(const char *who, const char *message,
               PurpleMessageFlags flags, time_t mtime)
{
	purple_conversation_write(conv, who, message, flags, mtime);
}

void
QQuailConvIm::updated(PurpleConvUpdateType type)
{
	if (type == GAIM_CONV_UPDATE_TYPING ||
		type == GAIM_CONV_UPDATE_UNSEEN)
	{
		QQuailConvWindow *qwin;
        QQuailConvWindow *win;
		QColor color;
		/* XXX GaimTypingState typingState; */
		int typingState;
		GaimUnseenState unseenState;

		color = black;
        typingState = purple_conv_im_get_typing_state(PURPLE_CONV_IM(conv));
		unseenState = purple_conversation_get_unseen(conv);

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

		win = purple_conversation_get_window(conv);
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

    purple_conv_im_send(PURPLE_CONV_IM(conv), text);

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
    QQuailConvWindow *win = purple_conversation_get_window(conv);
	QQuailConvWindow *qwin = (QQuailConvWindow *)win->ui_data;
	size_t length = entry->text().length();

	qwin->setSendEnabled(length > 0);

	if (!purple_prefs_get_bool("/core/conversations/im/send_typing"))
		return;

	if (purple_conv_im_get_type_again_timeout(im))
		purple_conv_im_stop_type_again_timeout(im);

	if (length == 0)
	{
		/* We deleted all of it, so we'll keep typing off. */
		serv_send_typing(purple_conversation_get_gc(conv),
						 purple_conversation_get_name(conv),
						 GAIM_NOT_TYPING);
	}
	else
	{
		purple_conv_im_start_type_again_timeout(im);

		if (length == 1 ||
			 (purple_conv_im_get_type_again(im) != 0 &&
			  time(NULL) > purple_conv_im_get_type_again(im)))
		{
			int timeout = serv_send_typing(purple_conversation_get_gc(conv),
										   purple_conversation_get_name(conv),
										   GAIM_TYPING);

			if (timeout)
				purple_conv_im_set_type_again(im, time(NULL) + timeout);
			else
				purple_conv_im_set_type_again(im, 0);
		}
	}
}

/**************************************************************************
 * QQuailConvWindow
 **************************************************************************/
QQuailConvWindow::QQuailConvWindow(QQuailConvWindow *win, QMainWindow *parent)
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
QQuailConvWindow::setConvWindow(QQuailConvWindow *win)
{
	this->win = win;
}

QQuailConvWindow *
QQuailConvWindow::getConvWindow() const
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
QQuailConvWindow::addConversation(PurpleConversation *conv)
{
	QQuailConversation *qconv = NULL;
	PurpleAccount *account;
	PurpleBuddy *b;

    if (purple_conversation_get_type(conv) == PURPLE_CONV_IM)
		qconv = new QQuailConvIm(conv, tabs);
	else if (purple_conversation_get_type(conv) == GAIM_CONV_CHAT)
		qconv = new QQuailConvChat(conv);
	else
		return;

	conv->ui_data = qconv;

	account = purple_conversation_get_account(conv);

	b = purple_find_buddy(account, purple_conversation_get_name(conv));

	if (b == NULL)
	{
		tabs->addTab(qconv,
			QQuailProtocolUtils::getProtocolIcon(account, QGAIM_PIXMAP_MENU),
			purple_conversation_get_title(conv));
	}
	else
	{
		tabs->addTab(qconv,
			QQuailBuddyList::getBuddyStatusIcon((PurpleBlistNode *)b),
			purple_conversation_get_title(conv));
	}

	qconv->setTabId(tabs->getLastId());

	if (purple_conv_window_get_conversation_count(win) == 1)
	{
        qQuailGetMainWindow()->setWindowTitle(purple_conversation_get_title(conv));
		tabs->setCurrentPage(0);
	}
}

void
QQuailConvWindow::removeConversation(PurpleConversation *conv)
{
	/* NOTE: This deletes conv->ui_data. Find out what to do here. */
	tabs->removePage((QQuailConversation *)conv->ui_data);

	conv->ui_data = NULL;
}

void
QQuailConvWindow::moveConversation(PurpleConversation *conv,
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

	for (l = purple_conv_window_get_conversations(win); l != NULL; l = l->next)
	{
        PurpleConversation *conv   = (PurpleConversation *)l->data;
		QQuailConversation *qconv = (QQuailConversation *)conv->ui_data;

		if (qconv->getTabId() == currentId)
			return purple_conversation_get_index(conv);
	}

	return currentIndex;
}

void
QQuailConvWindow::updateAddRemoveButton()
{
    PurpleConversation *conv = purple_conv_window_get_active_conversation(win);

	if (purple_find_buddy(purple_conversation_get_account(conv),
						purple_conversation_get_name(conv)) == NULL)
	{
		addRemoveButton->setText(tr("Add"));
        addRemoveButton->setIcon(
            QIcon(QPixmap(":/data/images/actions/add.png")));
	}
	else
	{
		addRemoveButton->setText(tr("Remove"));
        addRemoveButton->setIcon(
            QIcon(QPixmap(":/data/images/actions/remove.png")));
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
    PurpleConversation *conv = qconv->getPurpleConversation();
	PurpleAccount *account = purple_conversation_get_account(conv);
	PurplePlugin *prpl;
	PurplePluginProtocolInfo *prplInfo = NULL;

	purple_conversation_set_unseen(conv, GAIM_UNSEEN_NONE);

	if (conv != NULL)
        qQuailGetMainWindow()->setWindowTitle(purple_conversation_get_title(conv));

	prpl = purple_plugins_find_with_id(purple_account_get_protocol_id(account));

	if (prpl != NULL)
        prplInfo = PURPLE_PLUGIN_PROTOCOL_INFO(prpl);

	if (purple_conversation_get_type(conv) == GAIM_CONV_CHAT)
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

		if (purple_find_buddy(account, purple_conversation_get_name(conv)) == NULL)
		{
			addRemoveButton->setText(tr("Add"));
            addRemoveButton->setIcon(
                QIcon(QPixmap(":/data/images/actions/add.png")));
		}
		else
		{
			addRemoveButton->setText(tr("Remove"));
            addRemoveButton->setIcon(
                QIcon(QPixmap(":/data/images/actions/remove.png")));
		}
	}

	qconv->setFocus();
}

void
QQuailConvWindow::destroy(bool destroyWindow, bool destroySubWindows)
{
	purple_conv_window_destroy(win);

	QMainWindow::destroy(destroyWindow, destroySubWindows);

    if (qQuailGetMainWindow()->getLastActiveConvWindow() == getConvWindow())
		qQuailGetMainWindow()->setLastActiveConvWindow(NULL);
}

void
QQuailConvWindow::closeEvent(QCloseEvent *)
{
	purple_conv_window_destroy(win);
}

void
QQuailConvWindow::closeConv()
{
	purple_conversation_destroy(purple_conv_window_get_active_conversation(win));
}

static void
removeBuddyCb(PurpleBuddy *buddy)
{
	PurpleGroup *group;
    PurpleConversation *conv;
	QString name;

	if (buddy == NULL)
		return;

	group = purple_find_buddys_group(buddy);
	name = buddy->name;

	serv_remove_buddy(purple_account_get_connection(buddy->account), name,
					  group->name);
	purple_blist_remove_buddy(buddy);
	purple_blist_save();

	conv = purple_find_conversation(name);

	if (conv != NULL)
        purple_conversation_update(conv, PURPLE_CONV_UPDATE_REMOVE);
}

void
QQuailConvWindow::addRemoveBuddySlot()
{
    PurpleConversation *conv = purple_conv_window_get_active_conversation(win);
	PurpleAccount *account = purple_conversation_get_account(conv);
	const char *name = purple_conversation_get_name(conv);
	PurpleBuddy *buddy;

	if ((buddy = purple_find_buddy(account, name)) == NULL)
	{
		QQuailAddBuddyDialog *dialog;

		dialog = new QQuailAddBuddyDialog(this, "", true);
		dialog->setScreenName(purple_conversation_get_name(conv));
		dialog->setAccount(purple_conversation_get_account(conv));

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
    PurpleConversation *conv = purple_conv_window_get_active_conversation(win);
	PurpleAccount *account = purple_conversation_get_account(conv);

	serv_get_info(purple_account_get_connection(account),
				  purple_conversation_get_name(conv));
}

void
QQuailConvWindow::send()
{
    PurpleConversation *conv;
	QQuailConversation *qconv;

	conv = purple_conv_window_get_active_conversation(win);

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
    PurpleConversation *conv;
	QQuailConvChat *qchat;

	conv = purple_conv_window_get_active_conversation(win);

	if (purple_conversation_get_type(conv) != GAIM_CONV_CHAT)
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
                    QIcon(QPixmap(":/data/images/actions/close_conv.png")),
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
    button->setIcon(QIcon(QPixmap(":/data/images/actions/user.png")));
	button->setEnabled(false);

    userMenu = new QMenu(button);
	button->setPopup(userMenu);
	button->setPopupDelay(0);

	/* Warn */
	a = new QAction(tr("Warn"),
                    QIcon(QPixmap(":/data/images/actions/warn.png")),
					QString::null, 0, this, 0);
	warnButton = a;
	a->addTo(userMenu);
	a->setEnabled(false);

	/* Block */
	a = new QAction(tr("Block"),
                    QIcon(QPixmap(":/data/images/actions/block.png")),
					QString::null, 0, this, 0);
	blockButton = a;
	a->addTo(userMenu);
	a->setEnabled(false);

	/* Add */
	a = new QAction(tr("Add"),
                    QIcon(QPixmap(":/data/images/actions/add.png")),
					QString::null, 0, this, 0);
	addRemoveButton = a;
	a->addTo(userMenu);

	connect(addRemoveButton, SIGNAL(activated()),
			this, SLOT(addRemoveBuddySlot()));

	/* Info */
	a = new QAction(tr("Get Information"),
                    QIcon(QPixmap(":/data/images/actions/info.png")),
					QString::null, 0, this, 0);
	infoButton = a;
	a->addTo(userMenu);
	a->setEnabled(false);

	connect(infoButton, SIGNAL(activated()),
			this, SLOT(userInfoSlot()));

	/* Formatting */
	a = new QAction(tr("Formatting"),
                    QIcon(QPixmap(":/data/images/actions/formatting.png")),
					QString::null, 0, this, 0);
	a->addTo(toolbar);
	a->setEnabled(false);

	/* Separator */
	toolbar->addSeparator();

	/* Send */
	a = new QAction(tr("Send"),
                    QIcon(QPixmap(":/data/images/actions/send-im.png")),
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
        QIcon(QPixmap(":/data/images/actions/userlist.png")),
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
                    QIcon(QPixmap(":/data/images/actions/blist.png")),
					QString::null, 0, this, 0);
	a->addTo(toolbar);

	connect(a, SIGNAL(activated()),
			this, SLOT(showBlist()));

	/* Accounts */
	a = new QAction(tr("Accounts"),
                    QIcon(QPixmap(":/data/images/actions/accounts.png")),
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
qQuailConvDestroy(PurpleConversation *conv)
{
	QQuailConversation *qconv = (QQuailConversation *)conv->ui_data;

	if (qconv == NULL)
		return;

	conv->ui_data = NULL;

	delete qconv;
}

static void
qQuailConvWriteChat(PurpleConversation *conv, const char *who,
                   const char *message, PurpleMessageFlags flags, time_t mtime)
{
	QQuailConvChat *qchat = (QQuailConvChat *)conv->ui_data;

	qchat->write(who, message, flags, mtime);
}

static void
qQuailConvWriteIm(PurpleConversation *conv, const char *who,
                 const char *message, PurpleMessageFlags flags, time_t mtime)
{
	QQuailConvIm *qim = (QQuailConvIm *)conv->ui_data;

	qim->write(who, message, flags, mtime);
}

static void
qQuailConvWriteConv(PurpleConversation *conv, const char *who,
                   const char *message, PurpleMessageFlags flags, time_t mtime)
{
	QQuailConversation *qconv = (QQuailConversation *)conv->ui_data;

	qconv->write(who, message, flags, mtime);
}

static void
qQuailConvChatAddUser(PurpleConversation *conv, const char *user)
{
	QQuailConvChat *qchat = (QQuailConvChat *)conv->ui_data;

	qchat->addUser(user);
}

static void
qQuailConvChatAddUsers(PurpleConversation *conv, GList *users)
{
	QQuailConvChat *qchat = (QQuailConvChat *)conv->ui_data;

	qchat->addUsers(users);
}

static void
qQuailConvChatRenameUser(PurpleConversation *conv, const char *oldName,
						const char *newName)
{
	QQuailConvChat *qchat = (QQuailConvChat *)conv->ui_data;

	qchat->renameUser(oldName, newName);
}

static void
qQuailConvChatRemoveUser(PurpleConversation *conv, const char *user)
{
	QQuailConvChat *qchat = (QQuailConvChat *)conv->ui_data;

	qchat->removeUser(user);
}

static void
qQuailConvChatRemoveUsers(PurpleConversation *conv, GList *users)
{
	QQuailConvChat *qchat = (QQuailConvChat *)conv->ui_data;

	qchat->removeUsers(users);
}

static void
qQuailConvUpdated(PurpleConversation *conv, PurpleConvUpdateType type)
{
	QQuailConversation *qconv = (QQuailConversation *)conv->ui_data;

	qconv->updated(type);
}

static PurpleConversationUiOps convOps =
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

static PurpleConversationUiOps *
qQuailConvWindowGetConvUiOps()
{
	return &convOps;
}

/**************************************************************************
 * Window UI ops
 **************************************************************************/
static void
qQuailConvWindowNew(QQuailConvWindow *win)
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
qQuailConvWindowDestroy(QQuailConvWindow *win)
{
	QQuailConvWindow *qwin = (QQuailConvWindow *)win->ui_data;

	qQuailGetMainWindow()->removeConversationWindow(qwin);

	win->ui_data = NULL;
	delete qwin;
}

static void
qQuailConvWindowShow(QQuailConvWindow *win)
{
	QQuailConvWindow *qwin = (QQuailConvWindow *)win->ui_data;

	qQuailGetMainWindow()->setLastActiveConvWindow(win);

	qwin->show();
}

static void
qQuailConvWindowHide(QQuailConvWindow *win)
{
	QQuailConvWindow *qwin = (QQuailConvWindow *)win->ui_data;

	qwin->hide();
}

static void
qQuailConvWindowRaise(QQuailConvWindow *win)
{
	QQuailConvWindow *qwin = (QQuailConvWindow *)win->ui_data;

	qQuailGetMainWindow()->setLastActiveConvWindow(win);

	qQuailGetMainWindow()->getWidgetStack()->raiseWidget(qwin->getId());
}

static void
qQuailConvWindowSwitchConv(QQuailConvWindow *win, unsigned int index)
{
	QQuailConvWindow *qwin = (QQuailConvWindow *)win->ui_data;

	qwin->switchConversation(index);
}

static void
qQuailConvWindowAddConv(QQuailConvWindow *win, PurpleConversation *conv)
{
	QQuailConvWindow *qwin = (QQuailConvWindow *)win->ui_data;

	qwin->addConversation(conv);
}

static void
qQuailConvWindowRemoveConv(QQuailConvWindow *win, PurpleConversation *conv)
{
	QQuailConvWindow *qwin = (QQuailConvWindow *)win->ui_data;

	qwin->removeConversation(conv);
}

static void
qQuailConvWindowMoveConv(QQuailConvWindow *win, PurpleConversation *conv,
					unsigned int newIndex)
{
	QQuailConvWindow *qwin = (QQuailConvWindow *)win->ui_data;

	qwin->moveConversation(conv, newIndex);
}

static int
qQuailConvWindowGetActiveIndex(const QQuailConvWindow *win)
{
	QQuailConvWindow *qwin = (QQuailConvWindow *)win->ui_data;

	return qwin->getActiveIndex();
}

static PurpleConversationUiOps winOps =
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

PurpleConversationUiOps *
qQuailGetConvWindowUiOps()
{
	return &winOps;
}

