/**
 * @file QGaimConvWindow.h Conversation windows
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
#ifndef _QGAIM_CONV_WIN_H_
#define _QGAIM_CONV_WIN_H_

#include <qvariant.h>
#include <qwidget.h>
#include <qmainwindow.h>

#include <libgaim/conversation.h>

class QAction;
class QGaimMultiLineEdit;
class QGaimTabWidget;
class QListView;
class QPopupMenu;
class QTabWidget;
class QTextView;

class QGaimConversation : public QWidget
{
	Q_OBJECT

	public:
		QGaimConversation(GaimConversation *conv, QWidget *parent = NULL,
						  const char *name = NULL, WFlags fl = 0);
		virtual ~QGaimConversation();

		void setGaimConversation(GaimConversation *conv);
		GaimConversation *getGaimConversation() const;

		void write(const char *who, const char *message, size_t length,
				   int flags, time_t mtime);

		void setTitle(const char *title);

		virtual void updated(GaimConvUpdateType type);

		virtual void send() = 0;

		void setTabId(int id);
		int getTabId() const;

	protected:
		bool meify(char *message, int len);
		QString stripFontFace(const QString &str);

		void updateTabIcon();

	protected:
		GaimConversation *conv;
		QTextView *text;
		QGaimMultiLineEdit *entry;
		int tabId;
};

class QGaimChat : public QGaimConversation
{
	Q_OBJECT

	public:
		QGaimChat(GaimConversation *conv, QWidget *parent = NULL,
				  const char *name = NULL, WFlags fl = 0);
		virtual ~QGaimChat();

		void write(const char *who, const char *message, int flags,
				   time_t mtime);

		void addUser(const char *user);
		void addUsers(GList *users);
		void renameUser(const char *oldName, const char *newName);
		void removeUser(const char *user);
		void removeUsers(GList *users);

		void setShowUserList(bool show);
		bool getShowUserList() const;

		void send();

		virtual void updated(GaimConvUpdateType type);

	protected:
		virtual void buildInterface();

		virtual void focusInEvent(QFocusEvent *event);

	protected slots:
		virtual void returnPressed();

	protected:
		GaimChat *chat;
		QListView *userList;
};

class QGaimIm : public QGaimConversation
{
	Q_OBJECT

	public:
		QGaimIm(GaimConversation *conv, QWidget *parent = NULL,
				const char *name = NULL, WFlags fl = 0);
		virtual ~QGaimIm();

		void write(const char *who, const char *message,
				   size_t len, int flags, time_t mtime);

		void send();

		virtual void updated(GaimConvUpdateType type);

	protected:
		virtual void buildInterface();

		virtual void focusInEvent(QFocusEvent *event);

	protected slots:
		virtual void returnPressed();
		virtual void updateTyping();

	protected:
		GaimIm *im;
};

class QGaimConvWindow : public QMainWindow
{
	Q_OBJECT
	
	public:
		QGaimConvWindow(GaimWindow *win);
		~QGaimConvWindow();

		void setGaimWindow(GaimWindow *win);
		GaimWindow *getGaimWindow() const;

		void switchConversation(unsigned int index);
		void addConversation(GaimConversation *conv);
		void removeConversation(GaimConversation *conv);
		void moveConversation(GaimConversation *conv, unsigned int newIndex);
		int getActiveIndex() const;

		QGaimTabWidget *getTabs() const;

		void setId(int id);
		int getId() const;

	protected slots:
		void tabChanged(QWidget *widget);

	protected:
		virtual void destroy(bool destroyWindow = TRUE,
							 bool destroySubWindows = TRUE);
		virtual void closeEvent(QCloseEvent *e);

	private slots:
		void closeConv();
		void send();
		void showAccountsWindow();
		void conversationsToggled(bool state);
		void userListToggled(bool on);
		void showBlist();

	private:
		void buildInterface();
		void setupToolbar();

	private:
		GaimWindow *win;

		int convWinId;

		QToolBar *toolbar;
		QGaimTabWidget *tabs;

		QToolButton *convsButton;

		QAction *warnButton;
		QAction *blockButton;
		QAction *addButton;
		QAction *removeButton;
		QAction *infoButton;

		QPopupMenu *userMenu;
		QPopupMenu *formatMenu;

		QAction *userListToggle;
};

GaimWindowUiOps *qGaimGetConvWindowUiOps();

#endif /* _QGAIM_CONV_WIN_H_ */
