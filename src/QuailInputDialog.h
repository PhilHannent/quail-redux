/**
 * @file QGaimInputDialog.h Input dialog
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
#ifndef _QGAIM_INPUT_DIALOG_H_
#define _QGAIM_INPUT_DIALOG_H_

#include <qdialog.h>

class QHBox;
class QLabel;
class QSignalMapper;

class QGaimInputDialog : public QDialog
{
	Q_OBJECT

	public:
		QGaimInputDialog(bool multiline = false, QWidget *parent = NULL,
						 const char *name = NULL, bool modal = false,
						 WFlags fl = 0);

		void setInfoText(const QString &text);

		void setMasked(bool masked = true);
		bool isMasked() const;

		void setDefaultValue(const QString &text);
		QString getText() const;

		void addButtons(const QString &cancel = QString::null,
						const QString &ok = QString::null);
		void addButton(const QString &text, int result);

	private:
		bool multiline;
		bool masked;
		QSignalMapper *mapper;
		QHBox *hbox;
		QLabel *label;
		QWidget *entry;
};


#endif /* _QGAIM_INPUT_DIALOG_H_ */
