#ifndef _QGAIM_CONNECTION_METER_H_
#define _QGAIM_CONNECTION_METER_H_

#include <qlist.h>
#include <qprogressbar.h>
#include <qvariant.h>
#include <qwidget.h>

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

class QGaimConnectionMeter : public QWidget
{
	Q_OBJECT

	public:
		QGaimConnectionMeter(GaimConnection *gc, QWidget *parent = NULL,
							 const char *name = NULL, WFlags fl = 0);
		virtual ~QGaimConnectionMeter();

		void setText(QString &str);
		void setTotalSteps(int totalSteps);
		void setProgress(int progress);

		GaimConnection *getConnection() const;

	private:
		QGaimConnectionProgressBar *progressBar;
		GaimConnection *gc;
		QString str;
		int currentProgress;
		int totalSteps;
};

class QGaimConnectionMeters : public QWidget
{
	Q_OBJECT

	public:
		QGaimConnectionMeters(QWidget *parent = NULL,
							  const char *name = NULL, WFlags fl = 0);
		virtual ~QGaimConnectionMeters();

		void addMeter(QGaimConnectionMeter *meter);
		void addConnection(GaimConnection *gc);
		QGaimConnectionMeter *findMeter(GaimConnection *gc);

	public:
		QVBox *vbox;
		QList<QGaimConnectionMeter> meters;
};

#endif /* _QGAIM_CONNECTION_METER_H_ */
