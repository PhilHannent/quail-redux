/**
 * @file QQuailMultiLineEdit.h Multi-line edit widget with history
 *                            functionality
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
#ifndef _QUAIL_MULTI_LINE_EDIT_H_
#define _QUAIL_MULTI_LINE_EDIT_H_

#include <QTextEdit>

class QQuailMultiLineEdit : public QTextEdit
{
	Q_OBJECT

	public:
		QQuailMultiLineEdit(QWidget *parent = NULL, const char *name = NULL);

		void setHistoryEnabled(bool enabled);
		bool isHistoryEnabled() const;

		virtual bool eventFilter(QObject *object, QEvent *event);

	protected:
		void keyPressEvent(QKeyEvent *event);

	protected:
		bool historyEnabled;
		QStringList history;
		int index;
};

#endif /* _QUAIL_MULTI_LINE_EDIT_H_ */
