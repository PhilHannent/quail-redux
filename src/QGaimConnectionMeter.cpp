#include "QGaimConnectionMeter.h"

#include <libgaim/debug.h>

#include <qpushbutton.h>
#include <qlabel.h>

/**************************************************************************
 * QGaimConnectionProgressBar
 **************************************************************************/
QGaimConnectionProgressBar::QGaimConnectionProgressBar(QWidget *parent,
													   const char *name,
													   WFlags fl)
	: QProgressBar(parent, name, fl)
{
	setCenterIndicator(true);
}

bool
QGaimConnectionProgressBar::setIndicator(QString &str, int progress,
										 int totalSteps)
{
	return QProgressBar::setIndicator(str, progress, totalSteps);
}


/**************************************************************************
 * QGaimConnectionMeter
 **************************************************************************/
QGaimConnectionMeter::QGaimConnectionMeter(GaimConnection *gc,
										   QWidget *parent,
										   const char *name, WFlags fl)
	: QHBox(parent, name, fl), gc(gc)
{
	QLabel *label;
	GaimAccount *account;

	account = gaim_connection_get_account(gc);

	label = new QLabel(gaim_account_get_username(account), this);
	progressBar = new QGaimConnectionProgressBar(this);
}

QGaimConnectionMeter::~QGaimConnectionMeter()
{
}

void
QGaimConnectionMeter::update(QString str, int progress, int totalSteps)
{
	if (progress == 0)
		progressBar->setTotalSteps(totalSteps);

	progressBar->setProgress(progress);

//	progressBar->setIndicator(str, progress, totalSteps);
}

GaimConnection *
QGaimConnectionMeter::getConnection() const
{
	return gc;
}


/**************************************************************************
 * QGaimConnectionMeters
 **************************************************************************/
QGaimConnectionMeters::QGaimConnectionMeters(QWidget *parent,
											 const char *name, WFlags fl)
	: QVBox(parent, name, fl)
{
	meters.setAutoDelete(true);
}

QGaimConnectionMeters::~QGaimConnectionMeters()
{
}

void
QGaimConnectionMeters::addMeter(QGaimConnectionMeter *meter)
{
	gaim_debug(GAIM_DEBUG_INFO, "QGaimConnectionMeters",
			   "Adding a meter\n");
	meters.append(meter);

	meter->show();
}

void
QGaimConnectionMeters::removeMeter(QGaimConnectionMeter *meter)
{
	gaim_debug(GAIM_DEBUG_INFO, "QGaimConnectionMeters",
			   "Removing a meter\n");
	meters.remove(meter);
}

QGaimConnectionMeter *
QGaimConnectionMeters::addConnection(GaimConnection *gc)
{
	QGaimConnectionMeter *meter;

	meter = new QGaimConnectionMeter(gc, this);

	addMeter(meter);

	return meter;
}

QGaimConnectionMeter *
QGaimConnectionMeters::findMeter(GaimConnection *gc)
{
	QGaimConnectionMeter *meter;

	for (meter = meters.first(); meter != NULL; meter = meters.next())
	{
		if (meter->getConnection() == gc)
			return meter;
	}

	return NULL;
}
