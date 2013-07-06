TEMPLATE = app
TARGET   = Quail
VERSION  = 0.9
CONFIG   = qt warn_on debug

DEFINES += APP_NAME=\\\"Quail\\\"
DEFINES += QUAIL_PREFS_ROOT=\\\"/quail\\\"

# Check that we have the repository folder
exists(.hg):DEFINES += BUILDREVISION=\\\"$$system( hg parent --template \"{node}\")\\\"
else:DEFINES += BUILDREVISION=\\\"NOTBUILTFROMSOURCEREPOSITORY\\\"

#win32-g++ {
#    DEFINES += BUILDTIME=\\\"$$system('time/T')\\\"
#    DEFINES += BUILDDATE=\\\"$$system('echo %date%')\\\"
#}
#else {
#    DEFINES += BUILDTIME=\\\"$$system(date '+%H:%M.%s')\\\"
#    DEFINES += BUILDDATE=\\\"$$system(date '+%d/%m/%y')\\\"
#}

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


HEADERS = \
        src/QuailAccountBox.h \
        src/QuailAccountEditor.h \
        src/QuailAccountsWindow.h \
        src/QuailAction.h \
        src/QuailBListWindow.h \
        src/QuailBuddyList.h \
        src/QuailConnectionMeter.h \
        src/QuailConvButton.h \
        src/QuailConvWindow.h \
        src/QuailDebugWindow.h \
        src/QuailEventLoop.h \
        src/QuailDialogs.h \
        src/QuailImageUtils.h \
        src/QuailInputDialog.h \
        src/QuailMainWindow.h \
        src/QuailMultiLineEdit.h \
        src/QuailNotify.h \
        src/QuailPrefsDialog.h \
        src/QuailProtocolBox.h \
        src/QuailProtocolUtils.h \
        src/QuailRequest.h \
        src/QuailTabBar.h \
        src/QuailTabWidget.h \
	src/base.h


SOURCES = \
        src/QuailAccountBox.cpp \
        src/QuailAccountEditor.cpp \
        src/QuailAccountsWindow.cpp \
        src/QuailBListWindow.cpp \
        src/QuailBuddyList.cpp \
        src/QuailConnectionMeter.cpp \
        src/QuailConvButton.cpp \
        src/QuailConvWindow.cpp \
        src/QuailDebugWindow.cpp \
        src/QuailDialogs.cpp \
        src/QuailEventLoop.cpp \
        src/QuailImageUtils.cpp \
        src/QuailInputDialog.cpp \
        src/QuailMainWindow.cpp \
        src/QuailMultiLineEdit.cpp \
        src/QuailNotify.cpp \
        src/QuailPrefsDialog.cpp \
        src/QuailProtocolBox.cpp \
        src/QuailProtocolUtils.cpp \
        src/QuailRequest.cpp \
        src/QuailTabBar.cpp \
        src/QuailTabWidget.cpp \
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

unix {
    message("Using unix")
INCLUDEPATH += \
        ../pidgin-main/libpurple \
	/usr/include/glib-2.0 \
	/usr/lib/glib-2.0/include
}
win32-g++ {
    message("Using win32")
    INCLUDEPATH += /cygdrive/c/dev/win32-dev/gtk_2_0-2.14/include/glib-2.0 \
        /cygdrive/c/dev/win32-dev/gtk_2_0-2.14/include/glib-2.0/include \
        /cygdrive/c/dev/pidgin-main \
        c:/dev/pidgin-main \
        c:/dev/win32-dev/gtk_2_0-2.14/include/glib-2.0 \
        C:/dev/win32-dev/gtk_2_0-2.14/lib/glib-2.0/include
}

LIBS        += \
        -llibpurple \
	-lglib-2.0 \
	-lgmodule-2.0

OBJECTS_DIR = obj
MOC_DIR     = moc
