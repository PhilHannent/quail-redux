#ifndef _QGAIM_H_
#define _QGAIM_H_

#include <libgaim/conversation.h>

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

		void setLastActiveConvWindow(GaimWindow *win);
		GaimWindow *getLastActiveConvWindow() const;

	public slots:
		void showBlistWindow();
		void showAccountsWindow();

	private slots:
		void doMainLoop();

	private:
		QPEApplication *app;
		QGaimBListWindow *blistWin;
		QGaimAccountsWindow *accountsWin;
		GaimWindow *lastConvWin;
};

QGaim *qGaimGetHandle();

#endif /* _QGAIM_H_ */
