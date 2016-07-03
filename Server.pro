QT += core websockets
QT -= gui

isEmpty(LOKAL) {
include(/usr/include/qalarm-lib/Gemeinsam.pri)
INCLUDEPATH += /usr/include/qalarm-lib
}
else {
include(../Lib/Gemeinsam.pri)
INCLUDEPATH += ../Lib/Quellen
LIBS	    += -L../Lib/bin
}

TARGET = qalarm-server
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

LIBS += -lqalarm

SOURCES += Quellen/Start.cpp \
    Quellen/Steuerung.cpp \
    Quellen/WebsocketServer.cpp

HEADERS += Quellen/Parameter.h \
    Quellen/Steuerung.h \
    Quellen/WebsocketServer.h

DISTFILES += \
    server.ini \
    qalarm-server.spec \
    srpm-bauen.sh \
    qalarm.service
