TEMPLATE = app
TARGET   = gaim
VERSION  = 0.3
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
	src/QGaimImageUtils.h \
	src/QGaimInputDialog.h \
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
	src/QGaimImageUtils.cpp \
	src/QGaimInputDialog.cpp \
	src/QGaimMultiLineEdit.cpp \
	src/QGaimNotify.cpp \
	src/QGaimProtocolBox.cpp \
	src/QGaimProtocolUtils.cpp \
	src/QGaimRequest.cpp \
	src/QGaimTabBar.cpp \
	src/QGaimTabWidget.cpp \
	src/main.cpp


DISTFILES = \
	data/images/protocols/aim.png \
	data/images/protocols/gadu-gadu.png \
	data/images/protocols/icq.png \
	data/images/protocols/irc.png \
	data/images/protocols/jabber.png \
	data/images/protocols/msn.png \
	data/images/protocols/trepia.png \
	data/images/protocols/yahoo.png \
	data/images/status/activebuddy.png \
	data/images/status/admin.png \
	data/images/status/aol.png \
	data/images/status/away.png \
	data/images/status/dnd.png \
	data/images/status/extendedaway.png \
	data/images/status/female.png \
	data/images/status/freeforchat.png \
	data/images/status/game.png \
	data/images/status/hiptop.png \
	data/images/status/invisible.png \
	data/images/status/login.png \
	data/images/status/logout.png \
	data/images/status/male.png \
	data/images/status/na.png \
	data/images/status/notauthorized.png \
	data/images/status/occupied.png \
	data/images/status/offline.png \
	data/images/status/wireless.png \
	data/images/accounts.png \
	data/images/add.png \
	data/images/alias.png \
	data/images/away.png \
	data/images/blist.png \
	data/images/block.png \
	data/images/chat.png \
	data/images/close_conv.png \
	data/images/collapse.png \
	data/images/connect.png \
	data/images/conversations.png \
	data/images/delete.png \
	data/images/disconnect.png \
	data/images/edit.png \
	data/images/expand.png \
	data/images/formatting.png \
	data/images/info.png \
	data/images/join_chat.png \
	data/images/new.png \
	data/images/new-chat.png \
	data/images/new-im.png \
	data/images/new-group.png \
	data/images/offline_buddies.png \
	data/images/prefs.png \
	data/images/remove.png \
	data/images/send-im.png \
	data/images/settings.png \
	data/images/smileys.png \
	data/images/userlist.png \
	data/images/warn.png \
	AUTHORS \
	ChangeLog \
	NEWS \
	README \
	gaim.png \
	make.sh \
	mkstuff.conf \
	mkipk.sh \
	mktarball.sh

INCLUDEPATH += \
	$(QPEDIR)/include \
	/usr/include/glib-2.0 \
	/usr/lib/glib-2.0/include

DEPENDPATH  += $(QPEDIR)/include
LIBS        += \
	-lqpe \
	-lgaim \
	-lglib-2.0 \
	-lgmodule-2.0

OBJECTS_DIR = obj
MOC_DIR     = moc
