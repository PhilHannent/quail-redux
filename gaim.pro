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

DEFINES += LOCAL_COMPILE

INCLUDEPATH += \
	$(QPEDIR)/include \
	/usr/include/glib-2.0 \
	/usr/lib/glib-2.0/include \
	/home/chipx86/cvs/libgaim-root/include

DEPENDPATH  += $(QPEDIR)/include
DEFINES     += LOCAL_COMPILE
LIBS        += \
	-lqpe \
	-L/home/chipx86/cvs/libgaim-root/lib \
	-lgaim \
	-lglib-2.0 \
	-lgmodule-2.0

OBJECTS_DIR = obj
MOC_DIR     = moc
