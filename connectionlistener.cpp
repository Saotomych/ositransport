#include "connectionlistener.h"

CConnectionListener::CConnectionListener(QObject *parent) :
    QObject(parent)
{
}

void CConnectionListener::slotTSduReady(const CConnection* that)
{
	qDebug() << "CConnectionListener::slotTSduReady";

	emit signalTSduReady(that);
}

void CConnectionListener::slotCRReady(const CConnection* that)
{
	qDebug() << "CConnectionListener::slotCRReady";

	emit signalCRReady(that);
}

void CConnectionListener::slotIOError(QString strErr)
{
	qDebug() << "CConnectionListener::slotIOError" << strErr;

	emit signalIOError(strErr);
}

void CConnectionListener::slotClientConnected(const CConnection* that)
{
	qDebug() << "CConnectionListener::slotClientConnected";

	emit signalConnected(that);
}

void CConnectionListener::slotConnectAnswer(const CConnection* that)
{
	qDebug() << "CConnectionListener::slotConnectAnswer";

	emit signalConnectAnswer(that);
}

void CConnectionListener::slotClientDisconnected(const CConnection* that)
{
	qDebug() << "CConnectionListener::slotClientDisconnected";

	emit signalDisconnected(that);
}
