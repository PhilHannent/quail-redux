﻿/**
 * @file QQuailConvWindow.h Conversation windows
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
#ifndef _QUAIL_CONV_WIN_H_
#define _QUAIL_CONV_WIN_H_

#include <QMainWindow>
#include <QToolButton>
#include <QWidget>

#include <libpurple/conversation.h>

class QAction;
class QQuailMultiLineEdit;
class QQuailTabWidget;
class QListWidget;
class QTabWidget;
class QTextEdit;
/**
 * Unseen text states.
 */
typedef enum
{
    QUAIL_UNSEEN_NONE,   /**< No unseen text in the conversation. */
    QUAIL_UNSEEN_EVENT,  /**< Unseen events in the conversation.  */
    QUAIL_UNSEEN_NO_LOG, /**< Unseen text with NO_LOG flag.       */
    QUAIL_UNSEEN_TEXT,   /**< Unseen text in the conversation.    */
    QUAIL_UNSEEN_NICK    /**< Unseen text and the nick was said.  */
} QuailUnseenState;


class QQuailConversation : public QWidget
{
	Q_OBJECT

	public:
        QQuailConversation(PurpleConversation *conv,
                           QQuailTabWidget *parent = NULL);
		virtual ~QQuailConversation();

        void setConversation(PurpleConversation *conv);
        PurpleConversation *getConversation() const;

		void write(const char *who, const char *message,
                   PurpleMessageFlags flags, time_t mtime);

		void setTitle(const char *title);

        virtual void updated(PurpleConvUpdateType type);

		virtual void send() = 0;

		void setTabId(int id);
		int getTabId() const;

	protected:
		bool meify(char *message, int len);
		QString stripFontFace(const QString &str);

		void updateTabIcon();

    signals:
        void signalSendEnabled(bool bEnabled);

	protected:
        PurpleConversation *conv;
        QTextEdit *text;
		QQuailMultiLineEdit *entry;
		int tabId;
		bool notifying;
};

class QQuailConvChat : public QQuailConversation
{
	Q_OBJECT

	public:
        QQuailConvChat(PurpleConversation *conv, QQuailTabWidget *parent = NULL);
		virtual ~QQuailConvChat();

		void write(const char *who, const char *message,
                   PurpleMessageFlags flags, time_t mtime);

		void addUser(const char *user);
		void addUsers(GList *users);
		void renameUser(const char *oldName, const char *newName);
		void removeUser(const char *user);
		void removeUsers(GList *users);

		void setShowUserList(bool show);
		bool getShowUserList() const;

		void send();

        virtual void updated(PurpleConvUpdateType type);

	protected:
		virtual void buildInterface();

		virtual void focusInEvent(QFocusEvent *event);

	protected slots:
		virtual void returnPressed();
		virtual void updateTyping();

	protected:
        PurpleConvChat *chat;
        QListWidget *userList;
};

class QQuailConvIm : public QQuailConversation
{
	Q_OBJECT

	public:
        QQuailConvIm(PurpleConversation *conv, QQuailTabWidget *parent = NULL);
		virtual ~QQuailConvIm();

		void write(const char *who, const char *message,
                   PurpleMessageFlags flags, time_t mtime);

		void send();

        virtual void updated(PurpleConvUpdateType type);

	protected:
		virtual void buildInterface();

		virtual void focusInEvent(QFocusEvent *event);

	protected slots:
		virtual void returnPressed();
		virtual void updateTyping();

	protected:
        PurpleConvIm *im;
};

class QQuailConvWindow : public QMainWindow
{
	Q_OBJECT

	public:
        QQuailConvWindow(QMainWindow *parent);
		~QQuailConvWindow();

//        void setConvWindow(QQuailConversation *win);
//        QQuailConversation *getConvWindow() const;

		void switchConversation(unsigned int index);
        void addConversation(PurpleConversation *conv);
        void removeConversation(PurpleConversation *conv);
        void moveConversation(PurpleConversation *conv, unsigned int newIndex);
		int getActiveIndex() const;

		void updateAddRemoveButton();

		QQuailTabWidget *getTabs() const;

    public slots:
        void slotSendEnabled(bool enabled);

	protected slots:
        void tabChanged(int widget);

	protected:
		virtual void destroy(bool destroyWindow = TRUE,
							 bool destroySubWindows = TRUE);
		virtual void closeEvent(QCloseEvent *e);

	private slots:
		void closeConv();
		void addRemoveBuddySlot();
		void userInfoSlot();
		void send();
		void userListToggled(bool on);

    signals:
        void signalSendMessage();

	private:
		void buildInterface();
		void setupToolbar();

		QMainWindow *parentMainWindow;

        //QQuailConversation *win;

        //int convWinId;

		QToolBar *toolbar;
		QQuailTabWidget *tabs;

		QToolButton *convsButton;

		QToolButton *userMenuButton;
		QAction *warnButton;
		QAction *blockButton;
		QAction *addRemoveButton;
		QAction *infoButton;
		QAction *sendButton;

        QMenu *userMenu;
        QMenu *formatMenu;

		QAction *userListToggle;
};

PurpleConversationUiOps *qQuailGetConvWindowUiOps();

#endif /* _QUAIL_CONV_WIN_H_ */
