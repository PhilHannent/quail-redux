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
