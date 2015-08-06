#include "cconnection.h"

qint32 CConnection::s_connectionCounter = 0;

CConnection::CConnection(CTcpEasySocket* socket,
						quint32 maxTPduSizeParam,
						qint32 messageTimeout,
						qint32 messageFragmentTimeout,
						CServerThread* pServerThread):
m_pSocket(socket),
m_srcRef(0),
m_dstRef(0),
m_maxTPduSizeParam(maxTPduSizeParam),
m_messageTimeout(messageTimeout),
m_messageFragmentTimeout(messageFragmentTimeout),
m_closed(true),
m_pServerThread(pServerThread)
{
	QScopedPointer<QDataStream> os(new QDataStream(socket));
	QScopedPointer<QDataStream> is(new QDataStream(socket));

	m_pOs.swap(os);
	m_pIs.swap(is);
}

CConnection::CConnection(const CConnection& that)
{
	QScopedPointer<QDataStream> os(new QDataStream(m_pSocket));
	QScopedPointer<QDataStream> is(new QDataStream(m_pSocket));

	m_pOs.swap(os);
	m_pIs.swap(is);
}

void CConnection::setSelRemote(QVector<quint8>& tSelRemote)
{
	m_tSelRemote = tSelRemote;
}

void CConnection::setSelLocal(QVector<quint8>& tSelLocal)
{
	m_tSelLocal = tSelLocal;
}

void CConnection::listenForCR()
{
	m_pSocket->setMessageFragmentTimeout(m_messageFragmentTimeout);

	// start reading rfc 1006 header
	char *buffer(nullptr);
	int len;
//	m_pIs->readBytes(buffer, len);

}

void CConnection::startConnection()
{

}

void CConnection::send(QLinkedList<QVector<qint8> > tsdus, QLinkedList<quint32> offsets, QLinkedList<quint32> lengths)
{

}

void CConnection::send(QVector<qint8> tsdu, quint32 offset, quint32 length)
{

}

quint32 CConnection::getMessageTimeout()
{

}

void CConnection::setMessageTimeout(int messageTimeout)
{

}

quint32 CConnection::getMessageFragmentTimeout()
{

}

void CConnection::setMessageFragmentTimeout(int messageFragmentTimeout) {
	m_messageFragmentTimeout = messageFragmentTimeout;
}

void CConnection::receive(QVector<quint8> tSduBuffer)
{

}

void CConnection::disconnect()
{

}

void CConnection::close()
{

}

