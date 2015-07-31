#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include "ositransport_global.h"

class CServerThread : public QObject
{
    Q_OBJECT
public:
    explicit CServerThread(QObject *parent = 0);

signals:

public slots:

};

#endif // SERVERTHREAD_H
