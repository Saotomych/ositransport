#ifndef CCONNECTIONLISTENER_H
#define CCONNECTIONLISTENER_H

#include "ositransport_global.h"

class OSITRANSPORTSHARED_EXPORT CConnectionListener : public QObject
{
    Q_OBJECT
public:
    explicit CConnectionListener(QObject *parent = 0);

signals:

public slots:

};

#endif // CCONNECTIONLISTENER_H
