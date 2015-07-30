#ifndef CCONNECTION_H
#define CCONNECTION_H

#include <QObject>
#include "ositransport_global.h"

class OSITRANSPORTSHARED_EXPORT CConnection : public QObject
{
    Q_OBJECT
public:
    explicit CConnection(QObject *parent = 0);

signals:

public slots:

};

#endif // CCONNECTION_H
