/**
 * @file QGaimAccountBox.h Account drop-down menu
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
#ifndef _QGAIM_ACCOUNT_BOX_H_
#define _QGAIM_ACCOUNT_BOX_H_

#include <qcombobox.h>

#include <libgaim/account.h>

class QGaimAccountBox : public QComboBox
{
	Q_OBJECT

	public:
		QGaimAccountBox(bool showAll = false, QWidget *parent = NULL,
						const char *name = NULL);
		QGaimAccountBox(GaimAccount *account, bool showAll = false,
						QWidget *parent = NULL, const char *name = NULL);

		void setCurrentAccount(GaimAccount *account);
		GaimAccount *getCurrentAccount() const;

	protected:
		void buildMenu(GaimAccount *account);

	private:
		bool showAll;
};

#endif /* _QGAIM_ACCOUNT_BOX_H_ */