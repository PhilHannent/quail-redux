#ifndef _QGAIM_MULTI_LINE_EDIT_H_
#define _QGAIM_MULTI_LINE_EDIT_H_

#include <qmultilineedit.h>

class QGaimMultiLineEdit : public QMultiLineEdit
{
	Q_OBJECT

	public:
		QGaimMultiLineEdit(QWidget *parent = NULL, const char *name = NULL);

		void setHistoryEnabled(bool enabled);
		bool isHistoryEnabled() const;

		virtual bool eventFilter(QObject *object, QEvent *event);

	protected:
		void keyPressEvent(QKeyEvent *event);

	protected:
		bool historyEnabled;
		QStringList history;
		int index;
};

#endif /* _QGAIM_MULTI_LINE_EDIT_H_ */
