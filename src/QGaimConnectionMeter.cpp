#include "QGaimConnectionMeter.h"

#include <qlabel.h>
#include <qhbox.h>
#include <qvbox.h>

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


QGaimConnectionMeter::QGaimConnectionMeter(GaimConnection *gc,
										   QWidget *parent,
										   const char *name, WFlags fl)
	: QWidget(parent, name, fl), gc(gc), currentProgress(0), totalSteps(0)
{
	QHBox *box;
	QLabel *label;
	GaimAccount *account;

	account = gaim_connection_get_account(gc);

	box = new QHBox(this);

	label = new QLabel(gaim_account_get_username(account), box);
	progressBar = new QGaimConnectionProgressBar(box);
}

QGaimConnectionMeter::~QGaimConnectionMeter()
{
}

void
QGaimConnectionMeter::setText(QString &str)
{
	this->str = str;

	progressBar->setIndicator(str, currentProgress, totalSteps);
}

void
QGaimConnectionMeter::setTotalSteps(int totalSteps)
{
	this->totalSteps = totalSteps;

	progressBar->setIndicator(str, currentProgress, totalSteps);
}

void
QGaimConnectionMeter::setProgress(int progress)
{
	this->currentProgress = progress;

	progressBar->setIndicator(str, progress, totalSteps);
}

GaimConnection *
QGaimConnectionMeter::getConnection() const
{
	return gc;
}


QGaimConnectionMeters::QGaimConnectionMeters(QWidget *parent,
											 const char *name, WFlags fl)
	: QWidget(parent, name, fl)
{
}

QGaimConnectionMeters::~QGaimConnectionMeters()
{
}

void
QGaimConnectionMeters::addMeter(QGaimConnectionMeter *meter)
{
	meters.append(meter);
}

void
QGaimConnectionMeters::addConnection(GaimConnection *gc)
{
	addMeter(new QGaimConnectionMeter(gc, this));
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
