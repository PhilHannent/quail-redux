/**
 * @file QGaimDebugWindow.cpp Debug window
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
#include "QGaimDebugWindow.h"

#include <libgaim/debug.h>

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>

static void
qGaimDebugPrint(GaimDebugLevel, const char *category,
				const char *format, va_list args)
{
	gchar *arg_s;

	arg_s = g_strdup_vprintf(format, args);

	if (category == NULL)
		fprintf(stderr, "%s", arg_s);
	else
		fprintf(stderr, "%s: %s", category, arg_s);

	g_free(arg_s);
}

static GaimDebugUiOps ops =
{
	qGaimDebugPrint
};

GaimDebugUiOps *
qGaimGetDebugUiOps(void)
{
	return &ops;
}
