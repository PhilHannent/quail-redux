TEMPLATE = app
TARGET   = gaim
VERSION  = 0.1
CONFIG   = qt warn_on debug

HEADERS = \
	src/QGaimAccountBox.h \
	src/QGaimAccountEditor.h \
	src/QGaimAccountsWindow.h \
	src/QGaimBListWindow.h \
	src/QGaimBuddyList.h \
	src/QGaimConnectionMeter.h \
	src/QGaimConvButton.h \
	src/QGaimConvWindow.h \
	src/QGaimDebugWindow.h \
	src/QGaimDialogs.h \
	src/QGaimMultiLineEdit.h \
	src/QGaimNotify.h \
	src/QGaimProtocolBox.h \
	src/QGaimProtocolUtils.h \
	src/QGaimRequest.h \
	src/QGaimTabBar.h \
	src/QGaimTabWidget.h \
	src/QGaim.h \
	src/base.h


SOURCES = \
	src/QGaimAccountBox.cpp \
	src/QGaimAccountEditor.cpp \
	src/QGaimAccountsWindow.cpp \
	src/QGaimBListWindow.cpp \
	src/QGaimBuddyList.cpp \
	src/QGaimConnectionMeter.cpp \
	src/QGaimConvButton.cpp \
	src/QGaimConvWindow.cpp \
	src/QGaimDebugWindow.cpp \
	src/QGaimDialogs.cpp \
	src/QGaimMultiLineEdit.cpp \
	src/QGaimNotify.cpp \
	src/QGaimProtocolBox.cpp \
	src/QGaimProtocolUtils.cpp \
	src/QGaimRequest.cpp \
	src/QGaimTabBar.cpp \
	src/QGaimTabWidget.cpp \
	src/main.cpp


DISTFILES = \
	data/images/protocols/small/aim.png \
	data/images/protocols/small/gadu-gadu.png \
	data/images/protocols/small/icq.png \
	data/images/protocols/small/irc.png \
	data/images/protocols/small/jabber.png \
	data/images/protocols/small/msn.png \
	data/images/protocols/small/trepia.png \
	data/images/protocols/small/yahoo.png \
	data/images/accounts.png \
	data/images/add.png \
	data/images/away.png \
	data/images/blist.png \
	data/images/block.png \
	data/images/close_conv.png \
	data/images/connect.png \
	data/images/conversations.png \
	data/images/delete.png \
	data/images/disconnect.png \
	data/images/edit.png \
	data/images/formatting.png \
	data/images/info.png \
	data/images/new.png \
	data/images/new-group.png \
	data/images/prefs.png \
	data/images/remove.png \
	data/images/send-im.png \
	data/images/smileys.png \
	data/images/userlist.png \
	data/images/warn.png \
	ChangeLog \
	README \
	gaim.png \
	make.sh \
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
