﻿/**
 * @file QQuailDebugWindow.cpp Debug window
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
#include "QuailDebugWindow.h"

#include <libpurple/debug.h>

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>

#include <QApplication>

static void
qQuailDebugPrint(PurpleDebugLevel, const char *category,
                const char *arg_s)
{
	if (category == NULL)
		qDebug("%s", arg_s);
	else
		qDebug("%s: %s", category, arg_s);

}

static PurpleDebugUiOps ops =
{
    qQuailDebugPrint,
    NULL, /* is_enabled */
    NULL,
    NULL,
    NULL,
    NULL,
};

PurpleDebugUiOps *
qQuailGetDebugUiOps(void)
{
	return &ops;
}
