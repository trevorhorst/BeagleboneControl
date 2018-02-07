QT += core network websockets script
QT -= gui widget

# QMAKE_CXXFLAGS += -fpermissive
# INCLUDEPATH += /usr/local/include/QHttpEngine
# INCLUDEPATH += /usr/include/x86_64-linux-gnu/qt5/QtNetwork

LIBS += -L/usr/local/lib/
LIBS += -lqhttpengine
LIBS += -lmicrohttpd
LIBS += -lreadline

TARGET = HTTPSServer
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    # ndr354server.cpp \
    # httpserver.cpp \
    common/console.cpp \
    common/debug.cpp \
    common/script.cpp \
    # httpclient.cpp \
    # server/httpserver.cpp
    server/httpserver.cpp

HEADERS += \
    # ndr354server.h \
    # httpserver.h \
    common/console.h \
    common/debug.h \
    common/script.h \
    server/httpserver.h
    # httpclient.h \
    # server/httpserver.h \

RESOURCES += \
    resources.qrc \

DISTFILES +=

