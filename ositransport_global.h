#ifndef OSITRANSPORT_GLOBAL_H
#define OSITRANSPORT_GLOBAL_H

#include <QtCore/qglobal.h>
#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QMutex>
#include <QDebug>
#include <QHostInfo>

#if defined(OSITRANSPORT_LIBRARY)
#  define OSITRANSPORTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define OSITRANSPORTSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // OSITRANSPORT_GLOBAL_H
