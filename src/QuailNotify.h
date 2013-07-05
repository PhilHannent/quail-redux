/**
 * @file QQuailNotify.h Notification UI
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

#include <libpurple/notify.h>

GaimNotifyUiOps *qQuailGetNotifyUiOps();

void qQuailNotifyBuzzer(void);
void qQuailNotifyLedStart(void);
void qQuailNotifyLedStop(void);
void qQuailNotifySound(void);
void qQuailNotifyUser(void);
void qQuailNotifyUserStop(void);

void qQuailNotifyInit(void);

#endif /* _QGAIM_NOTIFY_H_ */
