/**
 * @file QGaimMultiLineEdit.cpp Multi-line edit widget with history
 *                              functionality
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
#include "QGaimMultiLineEdit.h"

QGaimMultiLineEdit::QGaimMultiLineEdit(QWidget *parent, const char *name)
	: QMultiLineEdit(parent, name), historyEnabled(false)
{
	index = -1;
	installEventFilter(this);
}

void
QGaimMultiLineEdit::setHistoryEnabled(bool enabled)
{
	historyEnabled = enabled;
}

bool
QGaimMultiLineEdit::isHistoryEnabled() const
{
	return historyEnabled;
}

bool
QGaimMultiLineEdit::eventFilter(QObject *object, QEvent *event)
{
	if (event->type() == QEvent::KeyPress)
	{
		QKeyEvent *k = (QKeyEvent *)event;

		if (k->key() == Key_Tab)
		{
			/* Tab */
			return TRUE;
		}
	}

	return QMultiLineEdit::eventFilter(object, event);
}

void
QGaimMultiLineEdit::keyPressEvent(QKeyEvent *event)
{
	int key = event->key();

	if (historyEnabled)
	{
		if (key == Key_Up)
		{
			if (history.count() > 0 &&
				index < (signed int)history.count() - 1)
			{
				index++;
				setText(history[index]);
			}
		}
		else if (key == Key_Down)
		{
			if (history.count() > 0 && index > 0)
			{
				index--;
				setText(history[index]);
			}

			if (index == 0)
			{
				index = -1;
				setText("");
			}
		}
		else if (key == Key_Return)
		{
			history.prepend(text());
			index = -1;
		}
		else if (key == Key_Tab)
		{
			/* TODO Tab completion */
			return;
		}
	}

	QMultiLineEdit::keyPressEvent(event);
}
