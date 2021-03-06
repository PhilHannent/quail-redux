﻿/**
 * @file QQuailMultiLineEdit.cpp Multi-line edit widget with history
 *                              functionality
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
#include "QuailMultiLineEdit.h"

#include <QDebug>
#include <QEvent>
#include <QKeyEvent>

QQuailMultiLineEdit::QQuailMultiLineEdit(QWidget *parent)
    : QTextEdit(parent), historyEnabled(false)
{
	index = -1;
	installEventFilter(this);
}

void
QQuailMultiLineEdit::setHistoryEnabled(bool enabled)
{
	historyEnabled = enabled;
}

bool
QQuailMultiLineEdit::isHistoryEnabled() const
{
	return historyEnabled;
}

bool
QQuailMultiLineEdit::eventFilter(QObject *object, QEvent *event)
{
	if (event->type() == QEvent::KeyPress)
	{
		QKeyEvent *k = (QKeyEvent *)event;

        if (k->key() == Qt::Key_Tab)
		{
			/* Tab */
            return true;
		}
	}

    return QTextEdit::eventFilter(object, event);
}

void
QQuailMultiLineEdit::keyPressEvent(QKeyEvent *event)
{
    qDebug() << "QQuailMultiLineEdit::keyPressEvent";
	int key = event->key();

    if (historyEnabled)
    {
        if (historyEnabled && key == Qt::Key_Up)
        {
            if (history.count() > 0 &&
                index < (signed int)history.count() - 1)
            {
                index++;
                setText(history[index]);
            }
        }
        else if (historyEnabled && key == Qt::Key_Down)
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
        else if (key == Qt::Key_Return)
        {
            history.prepend(this->toPlainText());
            index = -1;
            qDebug() << "QQuailMultiLineEdit::keyPressEvent.1";
            emit returnPressed();
            return;
        }
        else if (key == Qt::Key_Tab)
        {
            /* TODO Tab completion */
            return;
        }
        else if (key == Qt::Key_Return && event->modifiers() == Qt::CTRL)
        {
            emit returnPressed();
        }
    }
    else if (key == Qt::Key_Down || key == Qt::Key_Return)
    {
        emit returnPressed();

    }

    QTextEdit::keyPressEvent(event);
}
