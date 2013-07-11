/**
 * @file QQuailRequest.cpp Request UI
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
#include "QuailRequest.h"
#include "QuailInputDialog.h"
#include "QuailMainWindow.h"

#include <libpurple/debug.h>

#include <QMessageBox>

/**************************************************************************
 * UI operations
 **************************************************************************/
static void *
qQuailRequestInput(const char *title, const char *primary,
				  const char *secondary, const char *defaultValue,
				  gboolean multiline, gboolean masked,
				  const char *okText, GCallback okCb,
				  const char *cancelText, GCallback cancelCb,
				  void *userData)
{
	QString message;
	QString newTitle;
	QQuailInputDialog *dialog;

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
        newTitle = QString(primary).trimmed();
	else
		newTitle = title;

	dialog = new QQuailInputDialog(multiline, qQuailGetMainWindow(),
								  "input request", true);
    dialog->setWindowTitle(newTitle);
	dialog->setInfoText(message);

	if (defaultValue != NULL)
		dialog->setDefaultValue(defaultValue);

	if (masked)
		dialog->setMasked();

	dialog->addButtons(cancelText, okText);

	/* Execute the dialog. */
	int result = dialog->exec();

	purple_debug(GAIM_DEBUG_MISC, "requestInput",
			   "result = %d, '%s'\n", result,
			   (const char *)dialog->getText());

	if (result == 1)
	{
		if (okCb != NULL)
			((GaimRequestInputCb)okCb)(userData, dialog->getText());
	}
	else if (cancelCb != NULL)
		((GaimRequestInputCb)cancelCb)(userData, NULL);

	delete dialog;

	return NULL;
}

static void *
qQuailRequestAction(const char *title, const char *primary,
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
    msgbox->setWindowTitle(newTitle);
	msgbox->setText(message);

	cbs = new GCallback[actionCount];

#if 0
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
#endif

	for (size_t i = 0; i < actionCount; i++)
	{
		const char *text = va_arg(actions, char *);
		GCallback cb     = va_arg(actions, GCallback);

		msgbox->setButtonText(10 + i, text);
		cbs[i] = cb;
	}

	int result = msgbox->exec();
	purple_debug(GAIM_DEBUG_MISC, "QQuailRequest", "result = %d\n", result);

	result = actionCount - result - 1;

	if (cbs[result] != NULL)
		((GaimRequestActionCb)cbs[result])(userData, result);

	delete cbs;

	return NULL;
}

static PurpleRequestUiOps requestOps =
{
	qQuailRequestInput,
	NULL,
	qQuailRequestAction,
	NULL,
	NULL
};

PurpleRequestUiOps *
qQuailGetRequestUiOps()
{
	return &requestOps;
}
