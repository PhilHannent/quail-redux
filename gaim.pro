TEMPLATE = app
TARGET   = gaim
VERSION  = 0.1
CONFIG   = qt warn_on debug

HEADERS = \
	src/QGaimAccountsWindow.h \
	src/QGaimBListWindow.h \
	src/QGaimConvWindow.h \
	src/QGaimDebugWindow.h \
	src/QGaim.h \
	src/base.h

SOURCES = \
	src/QGaimAccountsWindow.cpp \
	src/QGaimBListWindow.cpp \
	src/QGaimConvWindow.cpp \
	src/QGaimDebugWindow.cpp \
	src/main.cpp

DISTFILES = \
	data/images/protocols/small/aim.png \
	data/images/protocols/small/gadu-gadu.png \
	data/images/protocols/small/icq.png \
	data/images/protocols/small/irc.png \
	data/images/protocols/small/jabber.png \
	data/images/protocols/small/msn.png \
	data/images/protocols/small/trepia.png \
	data/images/accounts.png \
	data/images/away.png \
	data/images/blist.png \
	data/images/connect.png \
	data/images/conversations.png \
	data/images/disconnect.png \
	data/images/edit.png \
	data/images/send-im.png \
	ChangeLog \
	README \
	mkstuff.conf \
	mkipk.sh \
	mktarball.sh

DEFINES += LOCAL_COMPILE

INCLUDEPATH += \
	$(QPEDIR)/include \
	/usr/include/glib-2.0 \
	/usr/lib/glib-2.0/include

DEPENDPATH  += $(QPEDIR)/include
DEFINES     += LOCAL_COMPILE
LIBS        += \
	-lqpe \
	-lgaim \
	-lglib-2.0 \
	-lgmodule-2.0

OBJECTS_DIR = obj
MOC_DIR     = moc
