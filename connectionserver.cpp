#include "connectionserver.h"

CConnectionServer::CConnectionServer(
		quint32 localPort,
		quint32 maxTPduSizeParam,
		quint32 msgTimeout,
		quint32 msgFragmentTimeout,
		quint32 maxConnections,
		CConnectionListener* listener):
		m_localPort(localPort),
		m_maxTPduSizeParam(maxTPduSizeParam),
		m_messageTimeout(msgTimeout),
		m_messageFragmentTimeout(msgFragmentTimeout),
		m_pConnListener(listener)
{
	m_TcpServer.setMaxPendingConnections(maxConnections);

	connect(&m_TcpServer, SIGNAL(newConnection()),this, SLOT(slotAcceptConnection()));

}

CConnection* CConnectionServer::createNewConnection(CTcpEasySocket* tcpSocket)
{
	try
	{
		CConnection* pconn = new CConnection(tcpSocket, m_maxTPduSizeParam, m_messageTimeout, m_messageFragmentTimeout);

		// signals from Connection to ServerThread
		connect(pconn, SIGNAL(signalIOError(QString)), this, SLOT(slotIOError(QString)));
		connect(pconn, SIGNAL(signalCRReady(const CConnection*)), this, SLOT(slotConnectionReady(const CConnection*)));
		connect(pconn, SIGNAL(signalConnectionClosed(const CConnection*)), this, SLOT(slotConnectionClosed(const CConnection*)));

		// signal from server to ConnectionListener
		connect(this, SIGNAL(signalClientConnected(const CConnection*)), m_pConnListener, SLOT(slotClientConnected(const CConnection*)));
		connect(this, SIGNAL(signalClientDisconnected(const CConnection*)), m_pConnListener, SLOT(slotClientDisconnected(const CConnection*)));

		// signals from Connection to ConnectionListener
		connect(pconn, SIGNAL(signalCRReady(const CConnection*)), m_pConnListener, SLOT(slotCRReady(const CConnection*)));
		connect(pconn, SIGNAL(signalTSduReady(const CConnection*)), m_pConnListener, SLOT(slotTSduReady(const CConnection*)));
		connect(pconn, SIGNAL(signalIOError(QString)), m_pConnListener, SLOT(slotIOError(QString)));

		return pconn;
	}

	catch (std::bad_alloc& ex)
	{
		qDebug() << ex.what();
		throw;
	}
}

void CConnectionServer::startServer()
{
	if (m_TcpServer.isListening() == false)
		m_TcpServer.listen(QHostAddress::AnyIPv4, m_localPort);
}

quint32 CConnectionServer::getMaxConnections()
{
	return m_TcpServer.maxPendingConnections();
}

void CConnectionServer::setMaxConnections(quint32 maxConnections)
{
	m_TcpServer.setMaxPendingConnections(maxConnections);
}

void CConnectionServer::stopServer()
{
	m_TcpServer.close();
}

/*** Slot Section ***/
void CConnectionServer::slotAcceptConnection()
{
	qDebug() << "CServerThread::slotAcceptConnection";

	QTcpSocket* qsock = m_TcpServer.nextPendingConnection();
	CTcpEasySocket* mysock = new CTcpEasySocket(qsock);
	CConnection* pconn = createNewConnection(mysock);

	emit signalClientConnected(pconn);

	pconn->listenForCR();
}

void CConnectionServer::slotConnectionClosed(const CConnection* that)
{
	qDebug() << "CServerThread::slotConnectionClosed";

	Q_CHECK_PTR(that);

	emit signalClientDisconnected(that);

	if (m_TcpServer.isListening() == false)
		m_TcpServer.listen(QHostAddress::AnyIPv4, m_localPort);
}
