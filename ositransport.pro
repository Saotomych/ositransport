#-------------------------------------------------
#
# Project created by QtCreator 2015-07-30T16:52:21
#
#-------------------------------------------------

QT       += network testlib

QT       -= gui

TEMPLATE = lib
DESTDIR = bin
VERSION = 0.0.0.0
CONFIG += build_all

DEFINES += OSITRANSPORT_LIBRARY

SOURCES += ositransport.cpp \
    cclienttsap.cpp \
    cservertsap.cpp \
    serverthread.cpp \
    cconnection.cpp \
    cconnectionlistener.cpp

HEADERS += ositransport.h\
        ositransport_global.h \
    cclienttsap.h \
    cservertsap.h \
    serverthread.h \
    cconnection.h \
    cconnectionlistener.h

unix {
    CONFIG (debug, debug|release) {
        TARGET = $$qtLibraryTarget(ositransportd)
    }else{
        TARGET = ositransport
    }
    target.path = /usr/lib
    INSTALLS += target
}else{
    TARGET = $$qtLibraryTarget(ositransport)
}

CONFIG += debug_and_release build_all
INCLUDEPATH += include/ositransport

CONFIG (debug, debug|release){
    OBJECTS_DIR = build/debug
} else {
    OBJECTS_DIR = build/release
}

