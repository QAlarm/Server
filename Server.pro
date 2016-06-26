QT += core websockets
QT -= gui

CONFIG += c++11

TARGET = server
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app
DESTDIR = bin
UI_DIR= tmp/ui
MOC_DIR= tmp/moc
OBJECTS_DIR= tmp/obj
RCC_DIR= tmp/rcc

QMAKE_LFLAGS += -fuse-ld=gold

SOURCES += Quellen/Start.cpp \
    Quellen/Steuerung.cpp \
    Quellen/Konfiguration.cpp \
    Quellen/WebsocketServer.cpp

HEADERS += Quellen/Parameter.h \
    Quellen/Steuerung.h \
    Quellen/Konfiguration.h \
    Quellen/WebsocketServer.h

DISTFILES += \
    server.ini
