#ifndef _QGAIM_ACCOUNT_EDITOR_H_
#define _QGAIM_ACCOUNT_EDITOR_H_

#include <libgaim/account.h>
#include <libgaim/plugin.h>

#include <qvariant.h>
#include <qwidget.h>
#include <qmainwindow.h>

class QComboBox;
class QPixmap;

class QGaimAccountEditor : public QMainWindow
{
	Q_OBJECT

	public:
		QGaimAccountEditor(GaimAccount *account = NULL);
		~QGaimAccountEditor();

	protected:
		virtual void buildInterface();

	private:
		GaimAccount *account;
		QComboBox *protocolList;
};

#endif /* _QGAIM_ACCOUNT_EDITOR_H_ */
