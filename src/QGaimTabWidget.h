#ifndef _QGAIM_TAB_WIDGET_H_
#define _QGAIM_TAB_WIDGET_H_

#include <qtabwidget.h>

class QGaimTabWidget : public QTabWidget
{
	Q_OBJECT

	public:
		QGaimTabWidget(QWidget *parent = NULL, const char *name = NULL);

		void setCurrentIndex(int index);
		int getCurrentIndex() const;

		void setTabColor(int index, const QColor &color);
};

#endif /* _QGAIM_TAB_WIDGET_H_ */
