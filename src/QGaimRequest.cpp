/**
 * @file QGaimRequest.cpp Request UI
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
#include "QGaimRequest.h"

#include <libgaim/debug.h>

#include <qinputdialog.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qmultilineedit.h>
#include <qvbox.h>

static void *
qGaimRequestInput(const char *title, const char *primary,
				  const char *secondary, const char *defaultValue,
				  gboolean multiline, gboolean masked,
				  const char *, GCallback okCb,
				  const char *, GCallback cancelCb,
				  void *userData)
{
	QString result;
	QString message;
	QString newTitle;
	QDialog *dialog;
	QWidget *entry;

	message = "";

	if (primary != NULL)
	{
		message += "<p>";
		message += primary;
		message += "</p>";
	}

	if (secondary != NULL)
	{
		message += "<p>";
		message += secondary;
		message += "</p>";
	}

	if (title == NULL)
		newTitle = QString(primary).stripWhiteSpace();
	else
		newTitle = title;

	dialog = new QDialog();
	dialog->setCaption(newTitle);

	QVBoxLayout *layout = new QVBoxLayout(dialog);
	layout->setAutoAdd(true);
	layout->setSpacing(5);

	new QLabel(message, dialog);

	if (multiline)
	{
		QMultiLineEdit *mlEntry = new QMultiLineEdit(dialog);
		mlEntry->setFixedVisibleLines(3);
		mlEntry->setWordWrap(QMultiLineEdit::WidgetWidth);

		if (defaultValue != NULL)
			mlEntry->setText(defaultValue);

		if (masked)
			mlEntry->setEchoMode(QMultiLineEdit::Password);

		entry = mlEntry;
	}
	else
	{
		QLineEdit *slEntry = new QLineEdit(dialog);

		if (defaultValue != NULL)
			slEntry->setText(defaultValue);

		if (masked)
			slEntry->setEchoMode(QLineEdit::Password);

		entry = slEntry;
	}

	result = dialog->exec();

	if (result == 0)
	{
		if (okCb != NULL)
		{
			if (multiline)
				result = ((QMultiLineEdit *)entry)->text();
			else
				result = ((QLineEdit *)entry)->text();

			((GaimRequestInputCb)okCb)(userData, result);
		}
	}
	else if (cancelCb != NULL)
		((GaimRequestInputCb)cancelCb)(userData, NULL);

	return NULL;
}

static void *
qGaimRequestAction(const char *title, const char *primary,
				   const char *secondary, unsigned int,
				   void *userData, size_t actionCount, va_list actions)
{
	QMessageBox *msgbox;
	QString message;
	QString newTitle;
	GCallback *cbs;

	message = "";

	if (primary != NULL)
	{
		message += "<p>";
		message += primary;
		message += "</p>";
	}

	if (secondary != NULL)
	{
		message += "<p>";
		message += secondary;
		message += "</p>";
	}

	if (title == NULL)
		newTitle = QString(primary).stripWhiteSpace();
	else
		newTitle = title;

	msgbox = new QMessageBox();
	msgbox->setCaption(newTitle);
	msgbox->setText(message);

	cbs = new GCallback[actionCount];

	if (actionCount > 2)
	{
		for (size_t i = 0; i < 2; i++)
		{
			(void)va_arg(actions, char *);

			cbs[i] = va_arg(actions, GCallback);
		}

		for (size_t i = 2; i < actionCount; i++)
		{
			const char *text = va_arg(actions, char *);
			GCallback cb     = va_arg(actions, GCallback);

			msgbox->setButtonText(i, text);
			cbs[i] = cb;
		}
	}
	else
	{
		for (size_t i = 0; i < actionCount; i++)
		{
			(void)va_arg(actions, char *);

			cbs[i] = va_arg(actions, GCallback);
		}
	}

	int result = msgbox->exec();
	gaim_debug(GAIM_DEBUG_MISC, "QGaimRequest", "result = %d\n", result);

	result = actionCount - result - 1;

	if (cbs[result] != NULL)
		((GaimRequestActionCb)cbs[result])(userData, result);

	delete cbs;

	return NULL;
}

static GaimRequestUiOps requestOps =
{
	qGaimRequestInput,
	NULL,
	qGaimRequestAction,
	NULL,
	NULL
};

GaimRequestUiOps *
qGaimGetRequestUiOps()
{
	return &requestOps;
}
