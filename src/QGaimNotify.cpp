/**
 * @file QGaimNotify.cpp Notification UI
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
#include "QGaimNotify.h"

#include <qmessagebox.h>

static void *qGaimNotifyEmails(size_t count, gboolean detailed,
							   const char **subjects, const char **froms,
							   const char **tos, const char **urls,
							   GCallback cb, void *userData);

static void *
qGaimNotifyMessage(GaimNotifyMsgType type, const char *title,
				   const char *primary, const char *secondary,
				   GCallback, void *)
{
	QString message;
	QString newTitle;

	message = "";

	if (primary != NULL)
	{
		message += primary;

		if (secondary != NULL)
			message += "\n\n";
	}

	if (secondary != NULL)
		message += secondary;

	if (title == NULL)
		newTitle = QString(primary).stripWhiteSpace();
	else
		newTitle = title;

	switch (type)
	{
		case GAIM_NOTIFY_MSG_ERROR:
			QMessageBox::critical(NULL, newTitle, message);
			break;

		case GAIM_NOTIFY_MSG_WARNING:
			QMessageBox::warning(NULL, newTitle, message);
			break;

		case GAIM_NOTIFY_MSG_INFO:
			QMessageBox::information(NULL, newTitle, message);
			break;

		default:
			return NULL;
	}

	return NULL;
}

static void *
qGaimNotifyEmail(const char *subject, const char *from, const char *to,
				 const char *url, GCallback cb, void *userData)
{
	return qGaimNotifyEmails(1, true,
							 (subject == NULL ? NULL : &subject),
							 (from    == NULL ? NULL : &from),
							 (to      == NULL ? NULL : &to),
							 (url     == NULL ? NULL : &url),
							 cb, userData);
}

static void *
qGaimNotifyEmails(size_t count, gboolean, const char **subjects,
				  const char **froms, const char **tos, const char **,
				  GCallback, void *)
{
	QString str, title;

	title = QObject::tr("You have new mail!");

	str  = *tos;
	str += "has ";
	str += count;

	if (count == 1)
		str += " new e-mail.\n\n";
	else
		str += " new e-mails.\n\n";

	if (count == 1)
	{
		if (froms != NULL)
		{
			str += "From: ";
			str += *froms;
			str += "\n";
		}
		
		if (subjects != NULL)
		{
			str += "Subject: ";
			str += *subjects;
			str += "\n";
		}
	}

	QMessageBox::information(NULL, title, str);

	return NULL;
}

static GaimNotifyUiOps notifyOps =
{
	qGaimNotifyMessage,
	qGaimNotifyEmail,
	qGaimNotifyEmails,
	NULL
};

GaimNotifyUiOps *
qGaimGetNotifyUiOps()
{
	return &notifyOps;
}
