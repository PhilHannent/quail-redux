#ifndef _QGAIM_CONV_WIN_H_
#define _QGAIM_CONV_WIN_H_

#include <qvariant.h>
#include <qwidget.h>
#include <qmainwindow.h>

#include <libgaim/conversation.h>

class QMultiLineEdit;
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

	protected:
		GaimConversation *conv;
		QTextView *text;
		QMultiLineEdit *entry;
};

class QGaimChat : public QGaimConversation
{
	Q_OBJECT

	public:
		QGaimChat(GaimConversation *conv, QWidget *parent = NULL,
				  const char *name = NULL, WFlags fl = 0);
		~QGaimChat();

		void write(const char *who, const char *message, int flags,
				   time_t mtime);

		void addUser(const char *user);
		void renameUser(const char *oldName, const char *newName);
		void removeUser(const char *user);

	protected:
		virtual void buildInterface();
};

class QGaimIm : public QGaimConversation
{
	Q_OBJECT

	public:
		QGaimIm(GaimConversation *conv, QWidget *parent = NULL,
				const char *name = NULL, WFlags fl = 0);
		~QGaimIm();

		void write(const char *who, const char *message,
				   size_t len, int flags, time_t mtime);

	protected:
		virtual void buildInterface();
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

	private:
		void buildInterface();
		QMenuBar *buildMenuBar();

	private:
		QTabWidget *tabs;

		GaimWindow *win;
};

GaimWindowUiOps *qGaimGetConvWindowUiOps();

#endif /* _QGAIM_CONV_WIN_H_ */
