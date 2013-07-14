DEFINES += APP_DISPLAY_VERSION=\\\"0.1.0\\\"
DEFINES += APP_MAJOR_VERSION=\\\"0\\\"

TEMPLATE = app
TARGET   = Quail
VERSION = $${APP_DISPLAY_VERSION}
CONFIG   = qt warn_on debug

DEFINES += APP_NAME=\\\"Quail\\\"
DEFINES += QUAIL_PREFS_ROOT=\\\"/quail\\\"
DEFINES += UI_ID=\\\"quail\\\"
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
        src/QuailTabWidget.h


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
	AUTHORS \
	ChangeLog \
	NEWS \
	README \
	quail.png \
	make.sh \
	mkstuff.conf \
	mkipk.sh \
	mktarball.sh

linux-g++* {
    message("Using unix")
        INCLUDEPATH += \
        ../pidgin-main/libpurple \
	/usr/include/glib-2.0 \
	/usr/lib/glib-2.0/include

	INCLUDEPATH += /usr/include/glib-2.0 \
		/usr/lib/glib-2.0/include \
		/usr/include/libpurple \
		../pidgin-main/libpurple \
		/usr/lib/x86_64-linux-gnu/glib-2.0/include

	LIBS += /usr/lib/libpurple.so

}
win32-g++ {
    message("Using win32")
    INCLUDEPATH += /cygdrive/c/dev/win32-dev/gtk_2_0-2.14/include/glib-2.0 \
        /cygdrive/c/dev/win32-dev/gtk_2_0-2.14/include/glib-2.0/include \
        /cygdrive/c/dev/pidgin-main \
        c:/dev/pidgin-main \
        c:/dev/win32-dev/gtk_2_0-2.14/include/glib-2.0 \
        C:/dev/win32-dev/gtk_2_0-2.14/lib/glib-2.0/include

    LIBS += -L"C:\dev\win32-dev\gtk-2.24.10\lib"
    LIBS += -L"C:\dev\pidgin-main\libpurple"
    LIBS += -L"C:\dev\pidgin-main\libpurple"
    LIBS += -L"C:\Qt\Qt5.1.0-mingw32\5.1.0-beta1\mingw47_32\lib"
    LIBS += -L"C:\Qt\Qt5.1.0-mingw32\5.1.0-beta1\mingw47_32\bin"

    RC_FILE = resource.rc

        LIBS        += -llibpurple
}

LIBS += -lglib-2.0 -lgmodule-2.0


OBJECTS_DIR = obj
MOC_DIR     = moc

RESOURCES += \
    quail.qrc
