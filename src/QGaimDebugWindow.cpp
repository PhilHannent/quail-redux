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
