#ifndef _QGAIM_H_
#define _QGAIM_H_

#include <qpe/qpeapplication.h>
#include <qvariant.h>

#include "QGaimBListWindow.h"

class QGaim : QObject
{
	Q_OBJECT
	
	public:
		QGaim(int argc, char **argv);
		~QGaim();

		void init();
		int exec();

		QGaimBListWindow *getBlistWindow() const;
		QGaimAccountsWindow *getAccountsWindow() const;

	public slots:
		void showBlistWindow();
		void showAccountsWindow();

	private slots:
		void doGlibLoop();

	private:
		QPEApplication *app;
		QGaimBListWindow *blistWin;
		QGaimAccountsWindow *accountsWin;
};

QGaim *qGaimGetHandle();

#endif /* _QGAIM_H_ */
