#ifndef _QGAIM_TAB_BAR_H_
#define _QGAIM_TAB_BAR_H_

#include <qcolor.h>
#include <qlist.h>
#include <qmap.h>
#include <qtabbar.h>

class QGaimTabBar : public QTabBar
{
	Q_OBJECT

	public:
		QGaimTabBar(QWidget *parent = NULL, const char *name = NULL);

		void setTabColor(int id, const QColor &color);
		const QColor &getTabColor(int id) const;

		void setCurrentIndex(int index);
		int getCurrentIndex();

		int getLastId() const;

		virtual int insertTab(QTab *tab, int index = -1);

	protected:
		virtual void paintLabel(QPainter *painter, const QRect &rect,
								QTab *tab, bool hasFocus) const;

	protected:
		QMap<int, QColor> colors;
		int lastId;
//		QList<QTab *> tabs;
};

#endif /* _QGAIM_TAB_BAR_H_ */
