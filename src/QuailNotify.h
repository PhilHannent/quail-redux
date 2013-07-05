/**
 * @file QGaimNotify.h Notification UI
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
#ifndef _QGAIM_NOTIFY_H_
#define _QGAIM_NOTIFY_H_

#include <libgaim/notify.h>

GaimNotifyUiOps *qGaimGetNotifyUiOps();

void qGaimNotifyBuzzer(void);
void qGaimNotifyLedStart(void);
void qGaimNotifyLedStop(void);
void qGaimNotifySound(void);
void qGaimNotifyUser(void);
void qGaimNotifyUserStop(void);

void qGaimNotifyInit(void);

#endif /* _QGAIM_NOTIFY_H_ */