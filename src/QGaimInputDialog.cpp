/**
 * @file QGaimInputDialog.cpp Input dialog
 *
 * @Copyright (C) 2003 Christian Hammond.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA  02111-1307  USA
 */
#include "QGaimInputDialog.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qmultilineedit.h>
#include <qpushbutton.h>
#include <qsignalmapper.h>
#include <qhbox.h>

QGaimInputDialog::QGaimInputDialog(bool multiline, QWidget *parent,
								   const char *name, bool modal, WFlags fl)
	: QDialog(parent, name, modal, fl), multiline(multiline), masked(false)
{
	QVBoxLayout *layout = new QVBoxLayout(this, 8);
	layout->setAutoAdd(true);

	label = new QLabel(this);

	mapper = new QSignalMapper(this);

	connect(mapper, SIGNAL(mapped(int)),
			this, SLOT(done(int)));

	if (multiline)
	{
		QMultiLineEdit *mlEntry = new QMultiLineEdit(this);

		mlEntry->setFixedVisibleLines(3);
		mlEntry->setWordWrap(QMultiLineEdit::WidgetWidth);

		entry = mlEntry;
	}
	else
	{
		QLineEdit *slEntry = new QLineEdit(this);

		entry = slEntry;
	}

	hbox = new QHBox(this);
	hbox->setSpacing(6);
}

void
QGaimInputDialog::setInfoText(const QString &text)
{
	label->setText(text);
}

void
QGaimInputDialog::setMasked(bool masked)
{
	this->masked = masked;

	if (multiline)
	{
		QMultiLineEdit *mlEntry = (QMultiLineEdit *)entry;

		mlEntry->setEchoMode(masked
							 ? QMultiLineEdit::Password
							 : QMultiLineEdit::Normal);
	}
	else
	{
		QLineEdit *slEntry = (QLineEdit *)entry;

		slEntry->setEchoMode(masked
							 ? QLineEdit::Password
							 : QLineEdit::Normal);
	}
}

bool
QGaimInputDialog::isMasked() const
{
	return masked;
}

void
QGaimInputDialog::setDefaultValue(const QString &text)
{
	if (multiline)
	{
		QMultiLineEdit *mlEntry = (QMultiLineEdit *)entry;

		mlEntry->setText(text);
	}
	else
	{
		QLineEdit *slEntry = (QLineEdit *)entry;

		slEntry->setText(text);
	}
}

QString
QGaimInputDialog::getText() const
{
	if (multiline)
	{
		QMultiLineEdit *mlEntry = (QMultiLineEdit *)entry;

		return mlEntry->text();
	}
	else
	{
		QLineEdit *slEntry = (QLineEdit *)entry;

		return slEntry->text();
	}
}

void
QGaimInputDialog::addButtons(const QString &cancel = QString::null,
							 const QString &ok = QString::null)
{
	addButton(ok, 1);
	addButton(cancel, 0);
}

void
QGaimInputDialog::addButton(const QString &text, int result)
{
	QPushButton *button = new QPushButton(text, hbox);

	mapper->setMapping(button, result);

	connect(button, SIGNAL(clicked()),
			mapper, SLOT(map()));
}


