#ifndef _QGAIM_CONNECTION_METER_H_
#define _QGAIM_CONNECTION_METER_H_

#include <qhbox.h>
#include <qlist.h>
#include <qprogressbar.h>
#include <qwidget.h>
#include <qvariant.h>
#include <qvbox.h>

#include <libgaim/connection.h>

class QProgressBar;
class QVBox;

class QGaimConnectionProgressBar : public QProgressBar
{
	Q_OBJECT

	public:
		QGaimConnectionProgressBar(QWidget *parent = NULL,
								   const char *name = NULL, WFlags fl = 0);

		bool setIndicator(QString &str, int progress, int totalSteps);
};

class QGaimConnectionMeter : public QHBox
{
	Q_OBJECT

	public:
		QGaimConnectionMeter(GaimConnection *gc, QWidget *parent = NULL,
							 const char *name = NULL, WFlags fl = 0);
		virtual ~QGaimConnectionMeter();

		void update(QString str, int totalSteps, int progress);

		GaimConnection *getConnection() const;

	private:
		QGaimConnectionProgressBar *progressBar;
		GaimConnection *gc;
};

class QGaimConnectionMeters : public QVBox
{
	Q_OBJECT

	public:
		QGaimConnectionMeters(QWidget *parent = NULL,
							  const char *name = NULL, WFlags fl = 0);
		virtual ~QGaimConnectionMeters();

		void addMeter(QGaimConnectionMeter *meter);
		void removeMeter(QGaimConnectionMeter *meter);

		QGaimConnectionMeter *addConnection(GaimConnection *gc);

		QGaimConnectionMeter *findMeter(GaimConnection *gc);

	public:
		QList<QGaimConnectionMeter> meters;
};

#endif /* _QGAIM_CONNECTION_METER_H_ */
