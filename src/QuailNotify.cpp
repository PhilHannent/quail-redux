/**
 * @file QQuailNotify.cpp Notification UI
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
#include "QuailNotify.h"

#include <libpurple/prefs.h>

#include <QDialog>
#include <QLabel>
#include <QLayout>
#include <QMessageBox>
#include <QTextEdit>

//#include <opie/odevice.h>

//using namespace Opie;

static int notifyActiveCount = 0;

/**************************************************************************
 * UI operations
 **************************************************************************/
static void *qQuailNotifyEmails(size_t count, gboolean detailed,
							   const char **subjects, const char **froms,
							   const char **tos, const char **urls,
							   GCallback cb, void *userData);

static void *
qQuailNotifyMessage(PurpleNotifyMsgType type, const char *title,
                    const char *primary, const char *secondary)
{
	QString message;
	QString newTitle;

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

	switch (type)
	{
        case PURPLE_NOTIFY_MSG_ERROR:
			QMessageBox::critical(NULL, newTitle, message);
			break;

        case PURPLE_NOTIFY_MSG_WARNING:
			QMessageBox::warning(NULL, newTitle, message);
			break;

        case PURPLE_NOTIFY_MSG_INFO:
			QMessageBox::information(NULL, newTitle, message);
			break;

		default:
			return NULL;
	}

	return NULL;
}

static void *
qQuailNotifyEmail(PurpleConnection *gc,
                  const char *subject, const char *from,
                  const char *to, const char *url)
{
//    return qQuailNotifyEmails(gc,
//							 (subject == NULL ? NULL : &subject),
//							 (from    == NULL ? NULL : &from),
//							 (to      == NULL ? NULL : &to),
//                             (url     == NULL ? NULL : &url));
}

static void *
qQuailNotifyEmails(PurpleConnection *gc,
                   size_t count, gboolean detailed,
                   const char **subjects, const char **froms,
                   const char **tos, const char **urls)
{
	QString str, title;

    //title = tr("You have new mail!");
    title = "You have new mail!";

	str = "<p>";

	if (count == 1)
        str += QString("%1 has 1 new e-mail.").arg(*tos);
	else
        str += QString("%1 has %2 new e-mails.").arg(*tos, count);

	str += "</p>";

	if (count == 1)
	{
		if (froms != NULL)
            str += QString("<p>From: %1</p>").arg(*froms);

		if (subjects != NULL)
            str += QString("<p>Subject: %1</p>").arg(*subjects);
	}

	QMessageBox::information(NULL, title, str);

	return NULL;
}

static void *
qQuailNotifyFormatted(const char *title, const char *primary,
                      const char *secondary, const char *text)
{
	QDialog *dialog;
	QVBoxLayout *layout;
	QLabel *label;
    QTextEdit *textview;
	QString message;
	QString newTitle;

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
	{
		if (primary != NULL)
            newTitle = QString(primary).trimmed();
		else
			newTitle = "";
	}
	else
		newTitle = title;


    //dialog = new QDialog(NULL, "notify-formatted");
    dialog = new QDialog();
    dialog->setWindowTitle(newTitle);

    layout = new QVBoxLayout();
    dialog->setLayout(layout);

    //layout->setAutoAdd(true);

	label = new QLabel(dialog);
    layout->addWidget(label);
	label->setText(message);

    textview = new QTextEdit(dialog);
    layout->addWidget(textview);
	textview->setText(text);

	dialog->showMaximized();

	return NULL;
}

static PurpleNotifyUiOps notifyOps =
{
	qQuailNotifyMessage,
	qQuailNotifyEmail,
	qQuailNotifyEmails,
    qQuailNotifyFormatted,

    NULL, /* searchresults */
    NULL, /* searchresults_new_rows */
    NULL, /* userinfo */
    NULL, /* uri */
    NULL, /* close_notify */
    NULL,
    NULL,
    NULL,
    NULL
};

PurpleNotifyUiOps *
qQuailGetNotifyUiOps()
{
	return &notifyOps;
}

void
qQuailNotifyBuzzer(void)
{
    if (!purple_prefs_get_bool("/quail/notify/use_buzzer"))
		return;

    //ODevice::inst()->alarmSound();
}

void
qQuailNotifyLedStart(void)
{
    if (!purple_prefs_get_bool("/quail/notify/use_led"))
		return;

//	ODevice *device = ODevice::inst();

//	if (!device->ledList().isEmpty())
//	{
//		OLed led = (device->ledList().contains(Led_Mail)
//					? Led_Mail : device->ledList()[0]);

//		device->setLedState(led,
//							(device->ledStateList(led).contains(Led_BlinkSlow)
//							 ? Led_BlinkSlow : Led_On));
//	}
}

void
qQuailNotifyLedStop(void)
{
//	ODevice *device = ODevice::inst();

//	if (!device->ledList().isEmpty())
//	{
//		OLed led = (device->ledList().contains(Led_Mail)
//					? Led_Mail : device->ledList()[0]);

//		device->setLedState(led, Led_Off);
//	}
}

void
qQuailNotifySound(void)
{
    if (!purple_prefs_get_bool("/quail/notify/use_sound"))
		return;
}

void
qQuailNotifyUser(void)
{
	qQuailNotifyBuzzer();
	qQuailNotifyLedStart();
	qQuailNotifySound();

	notifyActiveCount++;
}

void
qQuailNotifyUserStop(void)
{
	if (--notifyActiveCount > 0)
		return;

	qQuailNotifyLedStop();
}

void
qQuailNotifyInit(void)
{
    purple_prefs_add_none("/quail/notify");
    purple_prefs_add_bool("/quail/notify/incoming_im",   true);
    purple_prefs_add_bool("/quail/notify/incoming_chat", false);
    purple_prefs_add_bool("/quail/notify/use_buzzer",    false);
    purple_prefs_add_bool("/quail/notify/use_led",       true);
    purple_prefs_add_bool("/quail/notify/use_sound",     false);
}
