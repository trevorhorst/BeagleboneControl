QT += core network websockets script
QT -= gui widget

# QMAKE_CXXFLAGS += -fpermissive
# INCLUDEPATH += /usr/local/include/QHttpEngine
# INCLUDEPATH += /usr/include/x86_64-linux-gnu/qt5/QtNetwork

LIBS += -lmicrohttpd
LIBS += -lreadline

TARGET = HTTPSServer
CONFIG += c++11
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
    server/httpserver.cpp \
    client/httpclient.cpp \
    server/request.cpp \
    hwlib/register.cpp \
    hwlib/gpio.cpp \
    hwlib/hwlib.cpp \
    hwlib/clockmodule.cpp \
    hwlib/led.cpp \
    server/cmdled.cpp \
    server/command.cpp \
    server/commandhandler.cpp \
    server/cmdgpio.cpp \
    hwlib/i2c.cpp \
    hwlib/ssd1306.cpp

HEADERS += \
    # ndr354server.h \
    # httpserver.h \
    common/console.h \
    common/debug.h \
    common/script.h \
    server/httpserver.h \
    client/httpclient.h \
    server/request.h \
    hwlib/register.h \
    hwlib/gpio.h \
    hwlib/hwlib.h \
    hwlib/clockmodule.h \
    hwlib/led.h \
    server/cmdled.h \
    server/command.h \
    server/commandhandler.h \
    server/cmdgpio.h \
    hwlib/i2c.h \
    hwlib/ssd1306.h \
    common/glcdfont.h \
    common/hi_logo.h
    # httpclient.h \
    # server/httpserver.h \

RESOURCES += \
    resources.qrc \

DISTFILES += \
    default_alt/components/AppMain.jsx \
    default_alt/components/SubComponent.jsx \
    default_alt/components/Index.jsx

