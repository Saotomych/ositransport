#-------------------------------------------------
#
# Project created by QtCreator 2015-07-30T16:52:21
#
#-------------------------------------------------

QT       += network

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
	    OBJECTS_DIR = build/debug
	    DEFINES += DEBUG
		LIBS += -lgcov

	    QMAKE_CXXFLAGS_RELEASE -= -O
		QMAKE_CXXFLAGS_RELEASE -= -O1
		QMAKE_CXXFLAGS_RELEASE -= -O2
		QMAKE_CXXFLAGS += -O0 -fprofile-arcs -ftest-coverage
    }else{
        TARGET = $$qtLibraryTarget(ositransport)
        OBJECTS_DIR = build/release
    }
    target.path = /usr/lib
    INSTALLS += target
}else{
    TARGET = $$qtLibraryTarget(ositransport-notunix)
}

CONFIG += debug_and_release build_all
INCLUDEPATH += include/ositransport

