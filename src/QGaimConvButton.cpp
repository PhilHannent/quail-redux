#include "QGaimConvButton.h"
#include "base.h"

#include <libgaim/debug.h>

#include "qpopupmenu.h"

static void
newConvCb(char *, QGaimConvButton *button)
{
	button->setEnabled(true);
}

static void
delConvCb(char *, QGaimConvButton *button)
{
	if (gaim_get_conversations()->next == NULL)
		button->setEnabled(false);
}

QGaimConvButton::QGaimConvButton(QWidget *parent, const char *name)
	: QToolButton(parent, name), convs(NULL)
{
	QPixmap *pixmap = new QPixmap(DATA_PREFIX "images/conversations.png");

	setAutoRaise(true);
	setPixmap(*pixmap);

	delete pixmap;

	menu = new QPopupMenu();

	setPopup(menu);

	connect(menu, SIGNAL(aboutToShow()),
			this, SLOT(generateMenu()));
	connect(menu, SIGNAL(activated(int)),
			this, SLOT(convActivated(int)));

	gaim_signal_connect(this, event_new_conversation,
						(void *)newConvCb, this);
	gaim_signal_connect(this, event_del_conversation,
						(void *)delConvCb, this);

	if (gaim_get_conversations() == NULL)
		setEnabled(false);
}

QGaimConvButton::~QGaimConvButton()
{
	delete menu;

	if (convs != NULL)
		delete convs;

	gaim_signal_disconnect(this, event_new_conversation, (void *)newConvCb);
	gaim_signal_disconnect(this, event_del_conversation, (void *)delConvCb);
}

void
QGaimConvButton::generateMenu()
{
	GaimConversation *conv;
	GList *l;
	size_t size;
	int i;

	gaim_debug(GAIM_DEBUG_INFO, "QGaimConvButton", "About to show\n");

	menu->clear();

	if (convs != NULL)
		delete convs;

	if (gaim_get_conversations() == NULL)
		return;

	size = g_list_length(gaim_get_conversations());

	convs = new GaimConversation*[size];

	for (l = gaim_get_conversations(), i = 0; l != NULL; l = l->next, i++)
	{
		conv = (GaimConversation *)l->data;

		menu->insertItem(gaim_conversation_get_title(conv), i);

		convs[i] = conv;
	}
}

void
QGaimConvButton::convActivated(int id)
{
	GaimConversation *conv;
	GaimWindow *win;

	gaim_debug(GAIM_DEBUG_INFO, "QGaimConvButton", "convActivated\n");

	conv = convs[id];

	if (g_list_find(gaim_get_conversations(), conv) == NULL)
	{
		/*
		 * The conversation was somehow removed since this menu was
		 * created.
		 */
		return;

	}

	gaim_debug(GAIM_DEBUG_INFO, "QGaimConvButton", "Raising window\n");
	win = gaim_conversation_get_window(conv);

	gaim_window_switch_conversation(win, gaim_conversation_get_index(conv));
	gaim_window_raise(win);
}

