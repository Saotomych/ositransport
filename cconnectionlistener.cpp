#include "cconnectionlistener.h"

CConnectionListener::CConnectionListener(QObject *parent) :
    QObject(parent)
{
}

void CConnectionListener::slotConnectionStateChanged(CConnection* pConnection, QAbstractSocket::SocketState socketState)
{
	Q_CHECK_PTR(pConnection);

	if (socketState == QAbstractSocket::SocketState::ConnectedState)
		emit signalConnected(pConnection);

	if (socketState == QAbstractSocket::SocketState::UnconnectedState)
		emit signalDisconnected(pConnection);

}

void CConnectionListener::slotTSduReady(const CConnection* that)
{
	emit signalTSduReady(that);
}

void CConnectionListener::slotCRReady(const CConnection* that)
{
	emit signalCRReady(that);
}

void CConnectionListener::slotIOError(QString strErr)
{
	emit signalIOError(strErr);
}
