/**
 * @file QQuailInputDialog.cpp Input dialog
 *
 * @Copyright (C) 2003-2004 Christian Hammond.
 * @Copyright (C) 2013      Phil Hannent.
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

quail_input_dialog::quail_input_dialog(bool multiline, QWidget *parent)
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
quail_input_dialog::setInfoText(const QString &text)
{
	label->setText(text);
}

void
quail_input_dialog::setMasked(bool masked)
{
	this->masked = masked;

	if (multiline)
	{
//        QTextEdit *mlEntry = (QTextEdit *)entry;

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
quail_input_dialog::isMasked() const
{
	return masked;
}

void
quail_input_dialog::setDefaultValue(const QString &text)
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
quail_input_dialog::getText() const
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
quail_input_dialog::addButtons(const QString &cancel, const QString &ok)
{
	addButton(ok, 1);
	addButton(cancel, 0);
}

void
quail_input_dialog::addButton(const QString &text, int result)
{
    QPushButton *button = new QPushButton(text);
    hbox->addWidget(button);

	mapper->setMapping(button, result);

	connect(button, SIGNAL(clicked()),
			mapper, SLOT(map()));
}


