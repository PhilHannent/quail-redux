﻿/**
 * @file QQuailConvWindow.cpp Conversation windows
 *
 * @Copyright (C) 2003-2004 Christian Hammond.
 * @Copyright (C) 2013      Phil Hannent.
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
//#include "QuailBuddyList.h"
#include "QuailBlistItem.h"
#include "QuailConvWindow.h"
#include "QuailConvButton.h"
#include "QuailConvDisplay.h"
#include "QuailDialogs.h"
#include "QuailMultiLineEdit.h"
#include "QuailNotify.h"
#include "QuailProtocolUtils.h"
#include "QuailTabBar.h"
#include "QuailTabWidget.h"
#include "QuailMainWindow.h"

#include <libpurple/debug.h>
#include <libpurple/prefs.h>

#include <QAction>
#include <QDebug>
#include <QLabel>
#include <QLayout>
#include <QListWidget>
#include <QMessageBox>
#include <QMenu>
#include <QPushButton>
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
quail_conversation::quail_conversation(PurpleConversation *conv,
                                     quail_tab_widget *parent)
    : QWidget(parent), conv(conv), textDisplay(NULL), notifying(false)
{
    qDebug() << "QQuailConversation::QQuailConversation()";
    connect(this, SIGNAL(signalSendEnabled(bool)),
            parent, SIGNAL(signalSendEnabled(bool)));
}

quail_conversation::~quail_conversation()
{
}

void
quail_conversation::setConversation(PurpleConversation *conv)
{
	this->conv = conv;
}

PurpleConversation *
quail_conversation::getConversation() const
{
	return conv;
}

void
quail_conversation::write(const char *who, const char *message,
                         PurpleMessageFlags flags, time_t)
{
    qDebug() << "QQuailConversation::write()";
    if (textDisplay == NULL)
		return;

	size_t length = strlen(message);

	QString txt;

    if (flags & PURPLE_MESSAGE_SYSTEM)
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

        if (flags & PURPLE_MESSAGE_WHISPER)
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
                if (flags & PURPLE_MESSAGE_AUTO_RESP)
					nick = "&lt;AUTO-REPLY&gt; ***";
				else
					nick = "***";

				nick += who;

                if (flags & PURPLE_MESSAGE_NICK)
					color = "#AF7F00";
				else
					color = "#062585";
			}
			else
			{
				nick = who;

                if (flags & PURPLE_MESSAGE_AUTO_RESP)
					nick += "&lt;AUTO-REPLY&gt; :";
				else
					nick += ":";

                if (flags & PURPLE_MESSAGE_NICK)
					color = "#AF7F00";
                else if (flags & PURPLE_MESSAGE_RECV)
					color = "#A82F2F";
                else if (flags & PURPLE_MESSAGE_SEND)
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

    textDisplay->slotAddMessage(true, txt);
    //text->verticalScrollBar()->setValue(text->verticalScrollBar()->maxValue());

    if (flags & PURPLE_MESSAGE_RECV)
	{
//        PurpleConversation *activeConv;
//        QQuailConversation *win;

//		win = purple_conversation_get_window(conv);
//		activeConv = purple_conv_window_get_active_conversation(win);

//		if (conv != activeConv ||
//			win != qQuailGetMainWindow()->getLastActiveConvWindow())
		{
			const char *prefName;

            if (purple_conversation_get_type(conv) == PURPLE_CONV_TYPE_CHAT)
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
quail_conversation::setTitle(const char */*newTitle*/)
{
    qDebug() << "QQuailConversation::setTitle()";
    //title = NULL;
}

void
quail_conversation::updated(PurpleConvUpdateType type)
{
    qDebug() << "QQuailConversation::updated()";
    if (type == PURPLE_CONV_UPDATE_ACCOUNT)
	{
		purple_conversation_autoset_title(conv);
//TODO: Update the buddy icon
//        if (purple_conversation_get_type(conv) == PURPLE_CONV_TYPE_IM) {
//			; /* Update buddy icon. */
//        }

		updateTabIcon();
	}
    else if (type == PURPLE_CONV_ACCOUNT_ONLINE ||
             type == PURPLE_CONV_ACCOUNT_OFFLINE)
	{
		updateTabIcon();
	}
    else if (type == PURPLE_CONV_UPDATE_AWAY)
	{
		updateTabIcon();
	}
    else if (type == PURPLE_CONV_UPDATE_ADD ||
             type == PURPLE_CONV_UPDATE_REMOVE)
	{
        quail_conv_window *qwin = (quail_conv_window *)conv->ui_data;
        qwin->updateAddRemoveButton();
	}
}

void
quail_conversation::setTabId(int id)
{
	tabId = id;
}

int
quail_conversation::getTabId() const
{
	return tabId;
}

bool
quail_conversation::meify(char *message, int len)
{
    qDebug() << "QQuailConversation::meify()";
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
quail_conversation::stripFontFace(const QString &str)
{
	QString newStr = str;

//	newStr.replace(QRegExp(" FACE=\"?[^\"> ]+\"?", false), "");

	return newStr;
}

void
quail_conversation::updateTabIcon()
{
    qDebug() << "QQuailConversation::updateTabIcon()";
    quail_conv_window *qwin;
	PurpleAccount *account;
	PurpleBuddy *b;

    qwin = (quail_conv_window *)conv->ui_data;
	account = purple_conversation_get_account(conv);

	b = purple_find_buddy(account, purple_conversation_get_name(conv));

	if (b == NULL)
	{
        qwin->getTabs()->setTabIcon(this->getTabId(),
                                    quail_protocol_utils::getProtocolIcon(account));
	}
	else
	{
        qwin->getTabs()->setTabIcon(this->getTabId(),
                                    quail_blist_item::getBuddyStatusIcon((PurpleBlistNode *)b));
	}
    qwin->getTabs()->setTabText(this->getTabId(),
                                purple_conversation_get_title(conv));

}

/**************************************************************************
 * QQuailConvChat
 **************************************************************************/
quail_conv_chat::quail_conv_chat(PurpleConversation *conv, quail_tab_widget *parent)
    : quail_conversation(conv, parent), chat(PURPLE_CONV_CHAT(conv))
{
    qDebug() << "QQuailConvChat::QQuailConvChat()";
	buildInterface();
}

quail_conv_chat::~quail_conv_chat()
{
}

void
quail_conv_chat::write(const char *who, const char *message,
                     PurpleMessageFlags flags, time_t mtime)
{
    qDebug() << "QQuailConvChat::write()";
	purple_conversation_write(conv, who, message, flags, mtime);
}

void
quail_conv_chat::addUser(PurpleConvChatBuddy *user)
{
    qDebug() << "QQuailConvChat::addUser()";
    QListWidgetItem *item = new QListWidgetItem();
    //TODO: Fix this, its just printing out rubbish
    item->setText(user->alias);

    //TODO: Set the chat status icon
//	if (purple_conv_chat_is_user_ignored(chat, user))
//        item->setText("X"); /* XXX */
//	else
//        item->setText(" ");
    //item->setIcon(QIcon());


    userList->addItem(item);

}

void
quail_conv_chat::addUsers(GList *users)
{
    qDebug() << "QQuailConvChat::addUsers()";
	for (GList *l = users; l != NULL; l = l->next)
        addUser((PurpleConvChatBuddy *)l->data);
}

void
quail_conv_chat::renameUser(const char *oldName, const char *newName)
{
    qDebug() << "QQuailConvChat::renameUser()";
    foreach (QListWidgetItem *item , userList->findItems("*", Qt::MatchWildcard))
    {
        if (item->text() == oldName)
        {
            item->setText(newName);
            break;
        }
    }
}

void
quail_conv_chat::removeUser(const char *user)
{
    qDebug() << "QQuailConvChat::removeUser()";
    foreach (QListWidgetItem *item , userList->findItems("*", Qt::MatchWildcard))
    {
        if (item->text() == user)
        {
            delete item;
            break;
        }
    }
}

void
quail_conv_chat::removeUsers(GList *users)
{
    qDebug() << "QQuailConvChat::removeUsers()";
	for (GList *l = users; l != NULL; l = l->next)
		removeUser((const char *)l->data);
}

void
quail_conv_chat::buildInterface()
{
    qDebug() << "QQuailConvChat::buildInterface()";
    QHBoxLayout *hbox = new QHBoxLayout(this);
    QVBoxLayout *vbox = new QVBoxLayout(this);

    textDisplay  = new quail_conv_display(this);

    entry = new QQuailMultiLineEdit(this);
    entry->setHistoryEnabled(true);
    QFontMetrics m (entry->font()) ;
    int RowHeight = m.lineSpacing() ;
    entry->setFixedHeight(2 * RowHeight) ;

    /* For chats */
    userList = new QListWidget(this);
    userList->setSortingEnabled(true);

    vbox->addWidget(textDisplay);
    vbox->addWidget(entry);
    hbox->addLayout(vbox);
    hbox->addWidget(userList);
    this->setLayout(hbox);
	connect(entry, SIGNAL(returnPressed()),
			this, SLOT(returnPressed()));
	connect(entry, SIGNAL(textChanged()),
			this, SLOT(updateTyping()));

	entry->setFocus();
}

void
quail_conv_chat::focusInEvent(QFocusEvent *)
{
	entry->setFocus();
//	text->verticalScrollBar()->setValue(text->verticalScrollBar()->maxValue());
}

void
quail_conv_chat::setShowUserList(bool show)
{
    qDebug() << "QQuailConvChat::setShowUserList()";
	if (show)
		userList->show();
	else
		userList->hide();

//	text->verticalScrollBar()->setValue(text->verticalScrollBar()->maxValue());
}

bool
quail_conv_chat::getShowUserList() const
{
	return userList->isVisible();
}

void
quail_conv_chat::send()
{
    qDebug() << "QQuailConvChat::send()";
    QString text = entry->toPlainText();
    if (!text.isEmpty())
    {
        if (text[text.length() - 1] == '\n')
            text.remove(text.length() - 1, 1);

        purple_conv_chat_send(PURPLE_CONV_CHAT(conv), text.toStdString().c_str());
    }
    entry->setText("");
}

void
quail_conv_chat::returnPressed()
{
    qDebug() << "QQuailConvChat::returnPressed()";
	send();
}

void
quail_conv_chat::updateTyping()
{
    qDebug() << "QQuailConvChat::updateTyping()";
//    QQuailConvWindow *win = purple_conversation_get_window(conv);
    //QQuailConvWindow *qwin = (QQuailConvWindow *)conv->ui_data;

    //qwin->setSendEnabled(entry->toPlainText().length() > 1);
    emit signalSendEnabled(entry->toPlainText().length() > 1);

}

void
quail_conv_chat::updated(PurpleConvUpdateType type)
{
    qDebug() << "QQuailConvChat::updated()";
    if (type == PURPLE_CONV_UPDATE_UNSEEN)
	{
//		QQuailConvWindow *qwin;
//        QQuailConvWindow *win;
//		QColor color;

//        color = Qt::black;

//		win = purple_conversation_get_window(conv);
//		qwin = (QQuailConvWindow *)win->ui_data;

//        if (purple_conversation_get_unseen(conv) == QUAIL_UNSEEN_NICK)
//        {
//            color.setRgb(0x31, 0x4E, 0x6C);
//        }
//        else if (purple_conversation_get_unseen(conv) == QUAIL_UNSEEN_EVENT)
//        {
//            color.setRgb(0x86, 0x82, 0x72);
//        }
//        else if (purple_conversation_get_unseen(conv) == QUAIL_UNSEEN_TEXT)
//        {
//			color.setRgb(0xDF, 0x42, 0x1E);

//            if (purple_prefs_get_bool("/quail/notify/incoming_chat"))
//			{
//				qQuailNotifyUser();
//				notifying = true;
//			}
//		}
//        else
        {
            if (notifying)
            {
                qQuailNotifyUserStop();
                notifying = false;
            }
        }

//		qwin->getTabs()->setTabColor(getTabId(), color);
	}

    quail_conversation::updated(type);
}

/**************************************************************************
 * QQuailConvIm
 **************************************************************************/
quail_conv_im::quail_conv_im(PurpleConversation *conv, quail_tab_widget *parent)
    : quail_conversation(conv, parent), im(PURPLE_CONV_IM(conv))
{
    qDebug() << "QQuailConvIm::QQuailConvIm()";
	buildInterface();
}

quail_conv_im::~quail_conv_im()
{
}

void
quail_conv_im::write(const char *who, const char *message,
               PurpleMessageFlags flags, time_t mtime)
{
    qDebug() << "QQuailConvIm::write()";
	purple_conversation_write(conv, who, message, flags, mtime);
}

void
quail_conv_im::updated(PurpleConvUpdateType type)
{
    qDebug() << "QQuailConvIm::updated()";
    if (type == PURPLE_CONV_UPDATE_TYPING ||
        type == PURPLE_CONV_UPDATE_UNSEEN)
	{
//		QQuailConvWindow *qwin;
//        QQuailConvWindow *win;
		QColor color;
		/* XXX GaimTypingState typingState; */
		int typingState;
        QuailUnseenState unseenState = QUAIL_UNSEEN_NONE;

        color = Qt::black;
        typingState = purple_conv_im_get_typing_state(PURPLE_CONV_IM(conv));
        //unseenState = purple_conversation_get_unseen(conv);

        if (typingState == PURPLE_TYPING)
		{
			color.setRgb(0x46, 0xA0, 0x46);
		}
        else if (typingState == PURPLE_TYPED)
		{
			color.setRgb(0xD1, 0x94, 0x0C);
		}
        else if (unseenState == QUAIL_UNSEEN_NICK)
		{
			color.setRgb(0x31, 0x4E, 0x6C);
		}
        else if (unseenState == QUAIL_UNSEEN_EVENT)
		{
			color.setRgb(0x86, 0x82, 0x72);
		}
        else if (unseenState == QUAIL_UNSEEN_TEXT)
		{
			color.setRgb(0xDF, 0x42, 0x1E);
		}
		else if (notifying)
		{
			qQuailNotifyUserStop();
			notifying = false;
		}

//		win = purple_conversation_get_window(conv);
//		qwin = (QQuailConvWindow *)win->ui_data;

//		qwin->getTabs()->setTabColor(getTabId(), color);
	}

    quail_conversation::updated(type);
}

void
quail_conv_im::buildInterface()
{
    qDebug() << "QQuailConvIm::buildInterface()";
    QVBoxLayout *l = new QVBoxLayout(this);
    setWindowIcon(QIcon(":/data/images/logo.png"));

    textDisplay  = new quail_conv_display(this);
    entry = new QQuailMultiLineEdit(this);
    QFontMetrics m (entry->font()) ;
    int RowHeight = m.lineSpacing() ;
    entry->setFixedHeight(2 * RowHeight) ;

    l->addWidget(textDisplay);
    l->addWidget(entry);

	connect(entry, SIGNAL(returnPressed()),
			this, SLOT(returnPressed()));
	connect(entry, SIGNAL(textChanged()),
			this, SLOT(updateTyping()));

	entry->setFocus();
    this->setLayout(l);
}

void
quail_conv_im::focusInEvent(QFocusEvent *)
{
    qDebug() << "QQuailConvIm::focusInEvent()";
	entry->setFocus();
//	text->verticalScrollBar()->setValue(text->verticalScrollBar()->maxValue());
}

void
quail_conv_im::send()
{
    qDebug() << "QQuailConvIm::send()";
    QString text = entry->toPlainText();
    if (!text.isEmpty())
    {
        if (text[text.length() - 1] == '\n')
            text.remove(text.length() - 1, 1);

        purple_conv_im_send(PURPLE_CONV_IM(conv), text.toStdString().c_str());
    }
    entry->setText("");
	updateTyping();
}

void
quail_conv_im::returnPressed()
{
	send();
}

void
quail_conv_im::updateTyping()
{
    qDebug() << "QQuailConvIm::updateTyping()";
    //QQuailConvWindow *win = purple_conversation_get_window(conv);
    //QQuailConvIm *qwin = (QQuailConvIm *)conv->ui_data;
    size_t length = entry->toPlainText().length();

    //qwin->setSendEnabled(length > 0);
    emit signalSendEnabled(length > 1);

//	if (!purple_prefs_get_bool("/core/conversations/im/send_typing"))
//		return;

//	if (purple_conv_im_get_type_again_timeout(im))
//		purple_conv_im_stop_type_again_timeout(im);

	if (length == 0)
	{
		/* We deleted all of it, so we'll keep typing off. */
		serv_send_typing(purple_conversation_get_gc(conv),
						 purple_conversation_get_name(conv),
                         PURPLE_NOT_TYPING);
	}
	else
	{
//		purple_conv_im_start_type_again_timeout(im);

		if (length == 1 ||
			 (purple_conv_im_get_type_again(im) != 0 &&
			  time(NULL) > purple_conv_im_get_type_again(im)))
		{
			int timeout = serv_send_typing(purple_conversation_get_gc(conv),
										   purple_conversation_get_name(conv),
                                           PURPLE_TYPING);

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
quail_conv_window::quail_conv_window(QMainWindow *parent)
    : QMainWindow(), parentMainWindow(parent)/*, convWinId(0)*/
{
    qDebug() << "QQuailConvWindow::QQuailConvWindow()";
	buildInterface();
}

quail_conv_window::~quail_conv_window()
{
	delete userMenu;
}

//void
//QQuailConvWindow::setConvWindow(QQuailConversation *win)
//{
//	this->win = win;
//}

//QQuailConversation *
//QQuailConvWindow::getConvWindow() const
//{
//	return win;
//}

void
quail_conv_window::switchConversation(unsigned int index)
{
	if (index == (unsigned int)tabs->getCurrentIndex())
		return;

	tabs->setCurrentIndex(index);
}

void
quail_conv_window::addConversation(PurpleConversation *conv)
{
    qDebug() << "QQuailConvWindow::addConversation()";
    quail_conversation *qconv = NULL;
    PurpleAccount *account;
	PurpleBuddy *b;
    PurpleConversationType type = purple_conversation_get_type(conv);
    if (type == PURPLE_CONV_TYPE_IM)
    {
        qDebug() << "QQuailConvWindow::addConversation().1";
        qconv = new quail_conv_im(conv, tabs);
    }
    else if (type == PURPLE_CONV_TYPE_CHAT)
    {
        qDebug() << "QQuailConvWindow::addConversation().2";
        qconv = new quail_conv_chat(conv, tabs);
    }
	else
    {
        qDebug() << "QQuailConvWindow::addConversation().UNKNOWN CONV TYPE";
		return;
    }

	conv->ui_data = qconv;

	account = purple_conversation_get_account(conv);

	b = purple_find_buddy(account, purple_conversation_get_name(conv));

	if (b == NULL)
	{
        qDebug() << "QQuailConvWindow::addConversation().3";
        tabs->addTab(qconv,
            quail_protocol_utils::getProtocolIcon(account),
			purple_conversation_get_title(conv));
	}
	else
	{
        qDebug() << "QQuailConvWindow::addConversation().4";
        tabs->addTab(qconv,
            quail_blist_item::getBuddyStatusIcon((PurpleBlistNode *)b),
			purple_conversation_get_title(conv));
	}

	qconv->setTabId(tabs->getLastId());

//	if (purple_conv_window_get_conversation_count(win) == 1)
//	{
//        qQuailGetMainWindow()->setWindowTitle(purple_conversation_get_title(conv));
//		tabs->setCurrentPage(0);
//	}
}

void
quail_conv_window::removeConversation(PurpleConversation *conv)
{
    qDebug() << "QQuailConvWindow::removeConversation()";
	/* NOTE: This deletes conv->ui_data. Find out what to do here. */
    quail_conversation *win = (quail_conversation *)conv->ui_data;
    tabs->removeTab(win->getTabId());
    win->deleteLater();
    //conv->ui_data = NULL;
}

void
quail_conv_window::moveConversation(PurpleConversation */*conv*/,
                                  unsigned int /*newIndex*/)
{
    qDebug() << "QQuailConvWindow::moveConversation()";
    //conv = NULL;
    //newIndex = 0;
}

int
quail_conv_window::getActiveIndex() const
{
    qDebug() << "QQuailConvWindow::getActiveIndex()";
//    int currentId = tabs->currentIndex();
	int currentIndex = -1;
//	GList *l;

//	for (l = purple_conv_window_get_conversations(win); l != NULL; l = l->next)
//	{
//        PurpleConversation *conv   = (PurpleConversation *)l->data;
//		QQuailConversation *qconv = (QQuailConversation *)conv->ui_data;

//		if (qconv->getTabId() == currentId)
//			return purple_conversation_get_index(conv);
//	}

	return currentIndex;
}

void
quail_conv_window::updateAddRemoveButton()
{
    qDebug() << "QQuailConvWindow::updateAddRemoveButton()";
//    PurpleConversation *conv = purple_conv_window_get_active_conversation(win);

//	if (purple_find_buddy(purple_conversation_get_account(conv),
//						purple_conversation_get_name(conv)) == NULL)
//	{
//		addRemoveButton->setText(tr("Add"));
//        addRemoveButton->setIcon(
//            QIcon(QPixmap(":/data/images/actions/add.png")));
//	}
//	else
	{
		addRemoveButton->setText(tr("Remove"));
        addRemoveButton->setIcon(
            QIcon(QPixmap(":/data/images/actions/remove.png")));
	}
}

void
quail_conv_window::slotSendEnabled(bool enabled)
{
    qDebug() << "QQuailConvWindow::setSendEnabled()";
	sendButton->setEnabled(enabled);
}

quail_tab_widget *
quail_conv_window::getTabs() const
{
	return tabs;
}

void
quail_conv_window::tabChanged(int widgetId)
{
    qDebug() << "QQuailConvWindow::tabChanged";
    quail_conversation *qconv = (quail_conversation *)this->getTabs()->widget(widgetId);
    PurpleConversation *conv = qconv->getConversation();
	PurpleAccount *account = purple_conversation_get_account(conv);
	PurplePlugin *prpl;
	PurplePluginProtocolInfo *prplInfo = NULL;

    //purple_conversation_set_unseen(conv, QUAIL_UNSEEN_NONE);

	if (conv != NULL)
        qQuailGetMainWindow()->setWindowTitle(purple_conversation_get_title(conv));

	prpl = purple_plugins_find_with_id(purple_account_get_protocol_id(account));

	if (prpl != NULL)
        prplInfo = PURPLE_PLUGIN_PROTOCOL_INFO(prpl);

    if (purple_conversation_get_type(conv) == PURPLE_CONV_TYPE_CHAT)
	{
        quail_conv_chat *qchat = (quail_conv_chat *)qconv;

		userMenuButton->setEnabled(false);
		userListToggle->setEnabled(true);
		infoButton->setEnabled(false);
        userListToggle->setChecked(qchat->getShowUserList());
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
quail_conv_window::destroy(bool destroyWindow, bool destroySubWindows)
{
    qDebug() << "QQuailConvWindow::destroy()";
//	purple_conv_window_destroy(win);

	QMainWindow::destroy(destroyWindow, destroySubWindows);

//    if (qQuailGetMainWindow()->getLastActiveConvWindow() == this)
//		qQuailGetMainWindow()->setLastActiveConvWindow(NULL);
}

void
quail_conv_window::closeEvent(QCloseEvent *)
{
    qDebug() << "QQuailConvWindow::closeEvent()";
//	purple_conv_window_destroy(win);
}

void
quail_conv_window::closeConv()
{
    qDebug() << "QQuailConvWindow::closeConv()";
//	purple_conversation_destroy(purple_conv_window_get_active_conversation(win));
}

//static void
//removeBuddyCb(PurpleBuddy *buddy)
//{
////	PurpleGroup *group;
////    PurpleConversation *conv;
////	QString name;

//	if (buddy == NULL)
//		return;

////	group = purple_find_buddys_group(buddy);
////	name = buddy->name;

////	serv_remove_buddy(purple_account_get_connection(buddy->account), name,
////					  group->name);
////	purple_blist_remove_buddy(buddy);
////	purple_blist_save();

////	conv = purple_find_conversation(name);

////	if (conv != NULL)
////        purple_conversation_update(conv, PURPLE_CONV_UPDATE_REMOVE);
//}

void
quail_conv_window::addRemoveBuddySlot()
{
    qDebug() << "QQuailConvWindow::addRemoveBuddySlot";
//    PurpleConversation *conv = purple_conv_window_get_active_conversation(win);
//    PurpleAccount *account = purple_conversation_get_account(conv);
//    const char *name = purple_conversation_get_name(conv);
//    PurpleBuddy *buddy;

//    if ((buddy = purple_find_buddy(account, name)) == NULL)
//    {
//        QQuailAddBuddyDialog *dialog;

//        dialog = new QQuailAddBuddyDialog(this, "", true);
//        dialog->setScreenName(purple_conversation_get_name(conv));
//        dialog->setAccount(purple_conversation_get_account(conv));

//        dialog->show();
//    }
//    else
//    {
//		g_return_if_fail(buddy != NULL);

//		int result = QMessageBox::information(this,
//				tr("Remove Buddy"),
//				tr("<p>You are about to remove %1 from your buddy list.</p>\n"
//				   "<p>Do you want to continue?</p>").arg(name),
//				tr("&Remove Buddy"), tr("&Cancel"),
//				QString::null, 1, 1);

//		if (result == 0)
//			removeBuddyCb(buddy);
//	}
}

void
quail_conv_window::userInfoSlot()
{
    qDebug() << "QQuailConvWindow::userInfoSlot()";
//    PurpleConversation *conv = purple_conv_window_get_active_conversation(win);
//	PurpleAccount *account = purple_conversation_get_account(conv);

//	serv_get_info(purple_account_get_connection(account),
//				  purple_conversation_get_name(conv));
}

void
quail_conv_window::send()
{
    qDebug() << "QQuailConvWindow::send()";
    emit signalSendMessage();
//    PurpleConversation *conv;
//	QQuailConversation *qconv;

////	conv = purple_conv_window_get_active_conversation(win);

//	qconv = (QQuailConversation *)conv->ui_data;

//	qconv->send();
}

void
quail_conv_window::buildInterface()
{
    qDebug() << "QQuailConvWindow::buildInterface()";
	setupToolbar();

    tabs = new quail_tab_widget(this);
    connect(tabs, SIGNAL(currentChanged(int)),
            this, SLOT(tabChanged(int)));
    connect(this, SIGNAL(signalSendMessage()),
            tabs, SLOT(slotSendMessage()));
    connect(tabs, SIGNAL(signalSendEnabled(bool)),
            this, SLOT(slotSendEnabled(bool)));
    connect(this, SIGNAL(signalToggleUserList()),
            tabs, SLOT(slotToggleUserList()));

	setCentralWidget(tabs);
}

void
quail_conv_window::setupToolbar()
{
    qDebug() << "QQuailConvWindow::setupToolbar()";
	QToolButton *button;
	QAction *a;

//	setToolBarsMovable(false);

	toolbar = new QToolBar(this);

    /* Buddy List */
    a = new QAction(QIcon(QPixmap(":/data/images/actions/blist.png")),
                    tr("Buddy List"),
                    this);
    toolbar->addAction(a);

    connect(a, SIGNAL(triggered(bool)),
            parentMainWindow, SLOT(showBlistWindow()));

    /* Accounts */
    a = new QAction(QIcon(QPixmap(":/data/images/actions/accounts.png")),
                    tr("Accounts"),
                    this);
    toolbar->addAction(a);

    connect(a, SIGNAL(triggered(bool)),
            parentMainWindow, SLOT(showAccountsWindow()));

    /* Conversations */
    a = new QAction(QIcon(QPixmap(":/data/images/actions/conversations.png")),
                    tr("Conversations"),
                    this);
    toolbar->addAction(a);
    connect(a, SIGNAL(triggered(bool)),
            parentMainWindow, SLOT(showConvWindow()));

    /* Now we're going to construct the toolbar on the right. */
    toolbar->addSeparator();

	/* Close Conversation */
    a = new QAction(QIcon(QPixmap(":/data/images/actions/close_conv.png")),
                    tr("Close"),
                    this);
    toolbar->addAction(a);

    connect(a, SIGNAL(triggered(bool)),
			this, SLOT(closeConv()));

	/* Separator */
	toolbar->addSeparator();

	/* Person actions */
    button = new QToolButton(toolbar);
    button->setPopupMode(QToolButton::InstantPopup);
    toolbar->addWidget(button);
    userMenuButton = button;
	button->setAutoRaise(true);
    button->setIcon(QIcon(QPixmap(":/data/images/actions/user.png")));
    button->setEnabled(true);

    userMenu = new QMenu(button);
    button->setMenu(userMenu);

	/* Warn */
    a = new QAction(QIcon(QPixmap(":/data/images/actions/warn.png")),
                    tr("Warn"),
                    this);
	warnButton = a;
    userMenu->addAction(a);
    a->setEnabled(true);

	/* Block */
    a = new QAction(QIcon(QPixmap(":/data/images/actions/block.png")),
                    tr("Block"),
                    this);
	blockButton = a;
    userMenu->addAction(a);
    a->setEnabled(true);

	/* Add */
    a = new QAction(QIcon(QPixmap(":/data/images/actions/add.png")),
                    tr("Add"),
                    this);
	addRemoveButton = a;
    userMenu->addAction(a);

    connect(addRemoveButton, SIGNAL(triggered(bool)),
			this, SLOT(addRemoveBuddySlot()));

	/* Info */
    a = new QAction(QIcon(QPixmap(":/data/images/actions/info.png")),
                    tr("Get Information"),
                    this);
	infoButton = a;
    userMenu->addAction(a);
    a->setEnabled(true);

    connect(infoButton, SIGNAL(triggered(bool)),
			this, SLOT(userInfoSlot()));

	/* Formatting */
    a = new QAction(QIcon(QPixmap(":/data/images/actions/formatting.png")),
                    tr("Formatting"),
                    this);
    userMenu->addAction(a);
    a->setEnabled(true);

	/* Separator */
	toolbar->addSeparator();

	/* Send */
    a = new QAction(QIcon(QPixmap(":/data/images/actions/send-im.png")),
                    tr("Send"),
                    this);
	sendButton = a;
    toolbar->addAction(a);
	a->setEnabled(false);

    connect(a, SIGNAL(triggered(bool)),
			this, SLOT(send()));

	/* Separator */
	toolbar->addSeparator();

	/* User List toggle */
    userListToggle = new QAction(QIcon(QPixmap(":/data/images/actions/userlist.png")),
                                 tr("Show User List"),
                                 this);
    toolbar->addAction(userListToggle);
	userListToggle->setEnabled(false);

    connect(userListToggle, SIGNAL(triggered()),
            this, SIGNAL(signalToggleUserList()));

    this->addToolBar(toolbar);
}

/**************************************************************************
 * Conversation UI ops
 **************************************************************************/
static void
qQuailConvDestroy(PurpleConversation *conv)
{
    qDebug() << "QQuailConvWindow::qQuailConvDestroy()";
    quail_conversation *qconv = (quail_conversation *)conv->ui_data;

	if (qconv == NULL)
		return;

	conv->ui_data = NULL;

	delete qconv;
}

static void
qQuailConvWriteChat(PurpleConversation *conv, const char *who,
                    const char *message, PurpleMessageFlags flags,
                    time_t mtime)
{
    qDebug() << "QQuailConvWindow::qQuailConvWriteChat()";
    quail_conv_chat *qchat = (quail_conv_chat *)conv->ui_data;

	qchat->write(who, message, flags, mtime);
}

static void
qQuailConvWriteIm(PurpleConversation *conv, const char *who,
                  const char *message, PurpleMessageFlags flags,
                  time_t mtime)
{
    qDebug() << "QQuailConvWindow::qQuailConvWriteIm()";
    quail_conv_im *qim = (quail_conv_im *)conv->ui_data;

	qim->write(who, message, flags, mtime);
}

static void
qQuailConvWriteConv(PurpleConversation *conv,
                    const char *name,
                    const char */*alias*/,
                    const char *message,
                    PurpleMessageFlags flags,
                    time_t mtime)
{
    qDebug() << "QQuailConvWindow::qQuailConvWriteConv()";
    quail_conversation *qconv = (quail_conversation *)conv->ui_data;

    qconv->write(name, message, flags, mtime);
}

static void
qQuailConvChatAddUsers(PurpleConversation *conv,
                       GList *cbuddies,
                       gboolean /*new_arrivals*/)
{
    qDebug() << "QQuailConvWindow::qQuailConvChatAddUsers()";
    quail_conv_chat *qchat = (quail_conv_chat *)conv->ui_data;

    qchat->addUsers(cbuddies);
}

static void
qQuailConvChatRenameUser(PurpleConversation *conv, const char *old_name,
                         const char *new_name, const char */*new_alias*/)
{
    qDebug() << "QQuailConvWindow::qQuailConvChatRenameUser()";
    quail_conv_chat *qchat = (quail_conv_chat *)conv->ui_data;

    qchat->renameUser(old_name, new_name);
}

static void
qQuailConvChatRemoveUsers(PurpleConversation *conv, GList *users)
{
    qDebug() << "QQuailConvWindow::qQuailConvChatRemoveUsers()";
    quail_conv_chat *qchat = (quail_conv_chat *)conv->ui_data;

	qchat->removeUsers(users);
}

static void
qQuailConvUpdated(PurpleConversation */*conv*/, const char */*user*/)
{
    qDebug() << "QQuailConvWindow::qQuailConvUpdated()";
    //QQuailConversation *qconv = (QQuailConversation *)conv->ui_data;

    //qconv->updated(type);
}

static void
qQuailConvWindowNew(PurpleConversation *conv)
{
    qDebug() << "QQuailConvWindow::qQuailConvWindowNew()";
//    QQuailConversation *qwin;
    quail_main_window *mainWin = qQuailGetMainWindow();

    mainWin->addConversationWindow(conv);

//    mainWin->getWidgetStack()->addWidget(qwin);
    //mainWin->getWidgetStack()->setCurrentWidget(qwin);

//    conv->ui_data = qwin;
}

static PurpleConversationUiOps convOps =
{
    qQuailConvWindowNew, /* create_conversation */
    qQuailConvDestroy, /* destroy_conversation */
    qQuailConvWriteChat, /* write_chat */
    qQuailConvWriteIm, /* write_im */
    qQuailConvWriteConv, /* write_conv */
    qQuailConvChatAddUsers, /* chat_add_users */
    qQuailConvChatRenameUser, /* chat_rename_user */
    qQuailConvChatRemoveUsers, /* chat_remove_users */
    qQuailConvUpdated, /* chat_update_user */
    NULL, /* present */
    NULL, /* has_focus */
    NULL, /* custom_smiley_add */
    NULL, /* custom_smiley_write */
    NULL, /* custom_smiley_close */
    NULL, /* send_confirm */
    NULL,
    NULL,
    NULL,
    NULL,
};

PurpleConversationUiOps *
qQuailGetConvWindowUiOps()
{
    return &convOps;
}
