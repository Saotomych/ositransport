#-------------------------------------------------
#
# Project created by QtCreator 2015-07-30T16:52:21
#
#-------------------------------------------------

QT       += network testlib

QT       -= gui

TEMPLATE = lib
CONFIG += staticlib

DESTDIR = bin
VERSION = 0.0.0.0
CONFIG += build_all
CONFIG += c++11

DEFINES += OSITRANSPORT_LIBRARY

SOURCES += clienttsap.cpp \
	connection.cpp \
    connectionlistener.cpp \
    servertsap.cpp \
    connectionserver.cpp \
    socketfactory.cpp \
    tcpeasysocket.cpp

HEADERS += ositransport_global.h \
    clienttsap.h \
    connection.h \
    connectionlistener.h \
    servertsap.h \
    connectionserver.h \
    socketfactory.h \
    tcpeasysocket.h
    
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
    DEFINES += DEBUG
} else {
    OBJECTS_DIR = build/release
}

