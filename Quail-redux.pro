TEMPLATE = app
TARGET   = Quail

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
android-g++ {
    QT += widgets
    INCLUDEPATH += $(LIBPURPLE_ROOT)/../glib/glib
    INCLUDEPATH += $(LIBPURPLE_ROOT)/../glib
    INCLUDEPATH += $(LIBPURPLE_ROOT)/../glib/android
    INCLUDEPATH += $(LIBPURPLE_ROOT)/../glib/gobject/android/gobject
    INCLUDEPATH += $(LIBPURPLE_ROOT)/../glib/gobject
    #INCLUDEPATH += $(LIBPURPLE_ROOT)/../win32-dev/gtk_2_0-2.14/include/glib-2.0/include
    #INCLUDEPATH += $(LIBPURPLE_ROOT)/../win32-dev/gtk_2_0-2.14/lib/glib-2.0/include
    INCLUDEPATH += $(LIBPURPLE_ROOT)
} else {
    QT += webkit
    greaterThan(QT_MAJOR_VERSION, 4) {
        message("Using QT5")
        DEFINES += USE_QT5
        QT += widgets
        QT += webkitwidgets
    }
}

DISTFILES = \
    AUTHORS \
    ChangeLog \
    NEWS \
    README \
    LICENSE

linux-g++* {
    message("Using Linux")
    CONFIG += debug_and_release
    CONFIG += link_pkgconfig
    PKGCONFIG += purple glib-2.0 gmodule-2.0 libxslt
    INCLUDEPATH += "$(LIBPURPLE_ROOT)"
    LIBS += -L"$(LIBPURPLE_ROOT)/libpurple"
}

win32-g++* {
    message("Using win32")

    RC_FILE = resource.rc
    INCLUDEPATH += "$(DEV_ROOT)/win32-dev/gtk_2_0-2.14/include/glib-2.0"
    INCLUDEPATH += "$(DEV_ROOT)/win32-dev/gtk_2_0-2.14/lib/glib-2.0/include"
    INCLUDEPATH += "$(LIBPURPLE_ROOT)"
    INCLUDEPATH += $(QTDIR)/include
    LIBS += -L"$(DEV_ROOT)/win32-dev/gtk-2.24.10/lib" -lglib-2.0 -lgmodule-2.0
    LIBS += -L"$(LIBPURPLE_ROOT)/libpurple" -llibpurple
    LIBS += -L"$(DEV_ROOT)/win32-dev/gtk_2_0-2.14/lib"
    #LIBS += -L"$(QTDIR)/lib"
    LIBS += -lws2_32
    TARGET = bin/Quail
    CONFIG += console
    #DEFINES += QT_NO_DEBUG_OUTPUT
}

HEADERS += \
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
    src/QuailConvDisplay.h \
    src/QuailStatusSelector.h \
    src/QuailBlistItem.h \
    version.h

SOURCES += \
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
    src/main.cpp \
    src/QuailConvDisplay.cpp \
    src/QuailStatusSelector.cpp \
    src/QuailBlistItem.cpp

QMAKE_CFLAGS += -fstack-protector -fstack-protector-all

RESOURCES += \
    quail.qrc

OTHER_FILES += \
    android/AndroidManifest.xml \
    android/res/layout/splash.xml \
    android/res/values/libs.xml \
    android/res/values/strings.xml \
    android/res/values-de/strings.xml \
    android/res/values-el/strings.xml \
    android/res/values-es/strings.xml \
    android/res/values-et/strings.xml \
    android/res/values-fa/strings.xml \
    android/res/values-fr/strings.xml \
    android/res/values-id/strings.xml \
    android/res/values-it/strings.xml \
    android/res/values-ja/strings.xml \
    android/res/values-ms/strings.xml \
    android/res/values-nb/strings.xml \
    android/res/values-nl/strings.xml \
    android/res/values-pl/strings.xml \
    android/res/values-pt-rBR/strings.xml \
    android/res/values-ro/strings.xml \
    android/res/values-rs/strings.xml \
    android/res/values-ru/strings.xml \
    android/res/values-zh-rCN/strings.xml \
    android/res/values-zh-rTW/strings.xml \
    android/src/org/kde/necessitas/ministro/IMinistro.aidl \
    android/src/org/kde/necessitas/ministro/IMinistroCallback.aidl \
    android/src/org/qtproject/qt5/android/bindings/QtActivity.java \
    android/src/org/qtproject/qt5/android/bindings/QtApplication.java \
    android/version.xml \
    TODO.txt
