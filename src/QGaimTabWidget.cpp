#include "QGaimTabWidget.h"
#include "QGaimTabBar.h"

QGaimTabWidget::QGaimTabWidget(QWidget *parent, const char *name)
	: QTabWidget(parent, name)
{
	setTabBar(new QGaimTabBar(this, "tab control"));
}

void
QGaimTabWidget::setCurrentIndex(int index)
{
	((QGaimTabBar *)tabBar())->setCurrentIndex(index);
}

int
QGaimTabWidget::getCurrentIndex() const
{
	return ((QGaimTabBar *)tabBar())->getCurrentIndex();
}

void
QGaimTabWidget::setTabColor(int index, const QColor &color)
{
	((QGaimTabBar *)tabBar())->setTabColor(index, color);
}

int
QGaimTabWidget::getLastId() const
{
	return ((QGaimTabBar *)tabBar())->getLastId();
}
