TEMPLATE = app
TARGET   = gaim
VERSION  = 0.4
CONFIG   = qt warn_on debug

HEADERS = \
	src/QGaimAccountBox.h \
	src/QGaimAccountEditor.h \
	src/QGaimAccountsWindow.h \
	src/QGaimAction.h \
	src/QGaimBListWindow.h \
	src/QGaimBuddyList.h \
	src/QGaimConnectionMeter.h \
	src/QGaimConvButton.h \
	src/QGaimConvWindow.h \
	src/QGaimDebugWindow.h \
	src/QGaimDialogs.h \
	src/QGaimImageUtils.h \
	src/QGaimInputDialog.h \
	src/QGaimMainWindow.h \
	src/QGaimMultiLineEdit.h \
	src/QGaimNotify.h \
	src/QGaimPrefsDialog.h \
	src/QGaimProtocolBox.h \
	src/QGaimProtocolUtils.h \
	src/QGaimRequest.h \
	src/QGaimTabBar.h \
	src/QGaimTabWidget.h \
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
	src/QGaimMainWindow.cpp \
	src/QGaimMultiLineEdit.cpp \
	src/QGaimNotify.cpp \
	src/QGaimPrefsDialog.cpp \
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
	data/images/16x16/accounts.png \
	data/images/16x16/add.png \
	data/images/16x16/alias.png \
	data/images/16x16/away.png \
	data/images/16x16/blist.png \
	data/images/16x16/block.png \
	data/images/16x16/chat.png \
	data/images/16x16/close_conv.png \
	data/images/16x16/collapse.png \
	data/images/16x16/connect.png \
	data/images/16x16/conversations.png \
	data/images/16x16/delete.png \
	data/images/16x16/disconnect.png \
	data/images/16x16/edit.png \
	data/images/16x16/expand.png \
	data/images/16x16/formatting.png \
	data/images/16x16/info.png \
	data/images/16x16/join_chat.png \
	data/images/16x16/network.png \
	data/images/16x16/new-chat.png \
	data/images/16x16/new-group.png \
	data/images/16x16/new-im.png \
	data/images/16x16/new.png \
	data/images/16x16/offline_buddies.png \
	data/images/16x16/prefs.png \
	data/images/16x16/remove.png \
	data/images/16x16/send-im.png \
	data/images/16x16/settings.png \
	data/images/16x16/smileys.png \
	data/images/16x16/user.png \
	data/images/16x16/userlist.png \
	data/images/16x16/warn.png \
	data/images/32x32/accounts.png \
	data/images/32x32/add.png \
	data/images/32x32/alias.png \
	data/images/32x32/away.png \
	data/images/32x32/blist.png \
	data/images/32x32/block.png \
	data/images/32x32/chat.png \
	data/images/32x32/close_conv.png \
	data/images/32x32/collapse.png \
	data/images/32x32/connect.png \
	data/images/32x32/conversations.png \
	data/images/32x32/delete.png \
	data/images/32x32/disconnect.png \
	data/images/32x32/edit.png \
	data/images/32x32/expand.png \
	data/images/32x32/formatting.png \
	data/images/32x32/info.png \
	data/images/32x32/join_chat.png \
	data/images/32x32/network.png \
	data/images/32x32/new-chat.png \
	data/images/32x32/new-group.png \
	data/images/32x32/new-im.png \
	data/images/32x32/new.png \
	data/images/32x32/offline_buddies.png \
	data/images/32x32/prefs.png \
	data/images/32x32/remove.png \
	data/images/32x32/send-im.png \
	data/images/32x32/settings.png \
	data/images/32x32/smileys.png \
	data/images/32x32/user.png \
	data/images/32x32/userlist.png \
	data/images/32x32/warn.png \
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
	-lopie \
	-lgaim \
	-lglib-2.0 \
	-lgmodule-2.0

OBJECTS_DIR = obj
MOC_DIR     = moc
