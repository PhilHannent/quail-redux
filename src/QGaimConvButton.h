#ifndef _QGAIM_CONV_BUTTON_H_
#define _QGAIM_CONV_BUTTON_H_

#include <qvariant.h>
#include <qwidget.h>
#include <qtoolbutton.h>

#include <libgaim/conversation.h>

class QPopupMenu;

class QGaimConvButton : public QToolButton
{
	Q_OBJECT

	public:
		QGaimConvButton(QWidget *parent = NULL, const char *name = NULL);
		~QGaimConvButton();

	protected slots:
		void generateMenu();
		void convActivated(int id);

	private:
		QPopupMenu *menu;
		GaimConversation **convs;
};

#endif /* _QGAIM_CONV_BUTTON_H_ */
