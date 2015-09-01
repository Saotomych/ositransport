#include "cconnectionlistener.h"

CConnectionListener::CConnectionListener(QObject *parent) :
    QObject(parent)
{
}

void CConnectionListener::slotDisconnect()
{

}

void CConnectionListener::slotCreated(const CConnection* that)
{

}

void CConnectionListener::slotConnected(const CConnection* that)
{

}

void CConnectionListener::slotClosed(const CConnection* that)
{

}

void CConnectionListener::slotConnectionLost(const CConnection* that)
{

}
