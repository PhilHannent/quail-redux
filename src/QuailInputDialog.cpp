/**
 * @file QQuailInputDialog.cpp Input dialog
 *
 * @Copyright (C) 2003-2004 Christian Hammond.
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
#include "QuailInputDialog.h"

#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QSignalMapper>
#include <QHBoxLayout>

QQuailInputDialog::QQuailInputDialog(bool multiline, QWidget *parent,
                                   const char *name, bool modal, Qt::WindowFlags fl)
    : QDialog(parent), multiline(multiline), masked(false)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

	label = new QLabel(this);
    layout->addWidget(label);

	mapper = new QSignalMapper(this);

	connect(mapper, SIGNAL(mapped(int)),
			this, SLOT(done(int)));

	if (multiline)
	{
        QTextEdit *mlEntry = new QTextEdit(this);
        layout->addWidget(mlEntry);

//		mlEntry->setFixedVisibleLines(3);

		entry = mlEntry;
	}
	else
	{
		QLineEdit *slEntry = new QLineEdit(this);

		entry = slEntry;
	}

    hbox = new QHBoxLayout(this);
    hbox->addLayout(layout);
	hbox->setSpacing(6);
}

void
QQuailInputDialog::setInfoText(const QString &text)
{
	label->setText(text);
}

void
QQuailInputDialog::setMasked(bool masked)
{
	this->masked = masked;

	if (multiline)
	{
        QTextEdit *mlEntry = (QTextEdit *)entry;

//		mlEntry->setEchoMode(masked
//                             ? QTextEdit::Password
//                             : QTextEdit::Normal);
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
QQuailInputDialog::isMasked() const
{
	return masked;
}

void
QQuailInputDialog::setDefaultValue(const QString &text)
{
	if (multiline)
	{
        QTextEdit *mlEntry = (QTextEdit *)entry;

		mlEntry->setText(text);
	}
	else
	{
		QLineEdit *slEntry = (QLineEdit *)entry;

		slEntry->setText(text);
	}
}

QString
QQuailInputDialog::getText() const
{
	if (multiline)
	{
        QTextEdit *mlEntry = (QTextEdit *)entry;

        return mlEntry->toPlainText();
	}
	else
	{
		QLineEdit *slEntry = (QLineEdit *)entry;

		return slEntry->text();
	}
}

void
QQuailInputDialog::addButtons(const QString &cancel, const QString &ok)
{
	addButton(ok, 1);
	addButton(cancel, 0);
}

void
QQuailInputDialog::addButton(const QString &text, int result)
{
    QPushButton *button = new QPushButton(text);
    hbox->addWidget(button);

	mapper->setMapping(button, result);

	connect(button, SIGNAL(clicked()),
			mapper, SLOT(map()));
}


