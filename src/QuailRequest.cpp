/**
 * @file QQuailRequest.cpp Request UI
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
                   const char *secondary, const char *default_value,
                   gboolean multiline, gboolean masked, gchar */*hint*/,
                   const char *ok_text, GCallback ok_cb,
                   const char *cancel_text, GCallback cancel_cb,
                   PurpleAccount */*account*/, const char */*who*/,
                   PurpleConversation */*conv*/, void *user_data)
{
	QString message;
	QString newTitle;
	quail_input_dialog *dialog;

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

    dialog = new quail_input_dialog(multiline, qQuailGetMainWindow());
    dialog->setWindowTitle(newTitle);
	dialog->setInfoText(message);

    if (default_value != NULL)
        dialog->setDefaultValue(default_value);

	if (masked)
		dialog->setMasked();

    dialog->addButtons(cancel_text, ok_text);

	/* Execute the dialog. */
	int result = dialog->exec();

    purple_debug(PURPLE_DEBUG_MISC, "requestInput",
			   "result = %d, '%s'\n", result,
               (const char *)dialog->getText().toStdString().c_str());

	if (result == 1)
	{
        if (ok_cb != NULL)
            ((PurpleRequestInputCb)ok_cb)(user_data, dialog->getText().toStdString().c_str());
	}
    else if (cancel_cb != NULL)
        ((PurpleRequestInputCb)cancel_cb)(user_data, NULL);

	delete dialog;

	return NULL;
}

static void *
qQuailRequestAction(const char *title, const char *primary,
                    const char *secondary, int /*default_action*/,
                    PurpleAccount */*account*/, const char */*who*/,
                    PurpleConversation */*conv*/, void *user_data,
                    size_t action_count, va_list actions)
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
        newTitle = QString(primary).trimmed();
	else
		newTitle = title;

	msgbox = new QMessageBox();
    msgbox->setWindowTitle(newTitle);
	msgbox->setText(message);

    cbs = new GCallback[action_count];

#if 0
    if (action_count > 2)
	{
		for (size_t i = 0; i < 2; i++)
		{
			(void)va_arg(actions, char *);

			cbs[i] = va_arg(actions, GCallback);
		}

        for (size_t i = 2; i < action_count; i++)
		{
			const char *text = va_arg(actions, char *);
			GCallback cb     = va_arg(actions, GCallback);

			msgbox->setButtonText(i, text);
			cbs[i] = cb;
		}
	}
	else
	{
        for (size_t i = 0; i < action_count; i++)
		{
			(void)va_arg(actions, char *);

			cbs[i] = va_arg(actions, GCallback);
		}
	}
#endif

    for (size_t i = 0; i < action_count; i++)
	{
		const char *text = va_arg(actions, char *);
		GCallback cb     = va_arg(actions, GCallback);

		msgbox->setButtonText(10 + i, text);
		cbs[i] = cb;
	}

	int result = msgbox->exec();
    purple_debug(PURPLE_DEBUG_MISC, "QQuailRequest", "result = %d\n", result);

    result = action_count - result - 1;

	if (cbs[result] != NULL)
        ((PurpleRequestActionCb)cbs[result])(user_data, result);

    delete cbs;

	return NULL;
}

static PurpleRequestUiOps requestOps =
{
	qQuailRequestInput,
    NULL, /* request_choice */
	qQuailRequestAction,
    NULL, /* request_fields */
    NULL, /* request_file */
    NULL, /* close_request */
    NULL, /* request_folder */
    NULL, /* request_action_with_icon */
    NULL,
    NULL,
    NULL
};

PurpleRequestUiOps *
qQuailGetRequestUiOps()
{
	return &requestOps;
}
