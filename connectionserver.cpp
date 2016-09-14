#include "connectionserver.h"

CConnectionServer::CConnectionServer(
		QObject *parent,
		quint32 localPort,
		quint32 maxTPduSizeParam,
		quint32 msgTimeout,
		quint32 msgFragmentTimeout,
		quint32 maxConnections,
		CConnectionListener* listener):
		QObject(parent),
		m_localPort(localPort),
		m_maxTPduSizeParam(maxTPduSizeParam),
		m_messageTimeout(msgTimeout),
		m_messageFragmentTimeout(msgFragmentTimeout),
		m_pConnListener(listener)
{
	m_pTcpServer = new QTcpServer(this);
	connect(m_pTcpServer, SIGNAL(newConnection()),this, SLOT(slotServerAcceptConnection()));

	m_pTcpServer->setMaxPendingConnections(maxConnections);

}

CConnectionServer::~CConnectionServer()
{
	delete m_pTcpServer;
}

CConnection* CConnectionServer::createNewConnection(CTcpEasySocket* tcpSocket)
{
	try
	{
		CConnection* pconn = new CConnection(tcpSocket, m_maxTPduSizeParam, m_messageTimeout, m_messageFragmentTimeout);

		// signals from Connection to ServerThread
		connect(pconn, SIGNAL(signalConnectionClosed(const CConnection*)), this, SLOT(slotServerConnectionClosed(const CConnection*)));

		// signal from server to ConnectionListener
		connect(this, SIGNAL(signalClientConnected(const CConnection*)), m_pConnListener, SLOT(slotClientConnected(const CConnection*)));
		connect(this, SIGNAL(signalClientDisconnected(const CConnection*)), m_pConnListener, SLOT(slotClientDisconnected(const CConnection*)));

		// signals from Connection to ConnectionListener
		connect(pconn, SIGNAL(signalCRReady(const CConnection*)), m_pConnListener, SLOT(slotCRReady(const CConnection*)));
		connect(pconn, SIGNAL(signalTSduReady(const CConnection*)), m_pConnListener, SLOT(slotTSduReady(const CConnection*)));
		connect(pconn, SIGNAL(signalIOError(QString)), m_pConnListener, SLOT(slotIOError(QString)));

		pconn->asyncReadWriteInit();

		return pconn;
	}

	catch (std::bad_alloc& ex)
	{
		qDebug() << "CConnectionServer::createNewAssociation: " << ex.what();
		throw;
	}

	catch (std::exception& ex)
	{
		qDebug() << "CConnectionServer::createNewAssociation: " << ex.what();
		throw;
	}

}

void CConnectionServer::startServer()
{
	if (m_pTcpServer->isListening() == false)
	{
		qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");

		m_pTcpServer->listen(QHostAddress::AnyIPv4, m_localPort);
	}
}

quint32 CConnectionServer::getMaxConnections()
{
	return m_pTcpServer->maxPendingConnections();
}

void CConnectionServer::setMaxConnections(quint32 maxConnections)
{
	m_pTcpServer->setMaxPendingConnections(maxConnections);
}

void CConnectionServer::stopServer()
{
	m_pTcpServer->close();
}

/*** Slot Section ***/
void CConnectionServer::slotServerAcceptConnection()
{
	qDebug() << "CConnectionServer::slotServerAcceptConnection";

	QTcpSocket* qsock = m_pTcpServer->nextPendingConnection();
	connect(qsock, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(slotServerError(QAbstractSocket::SocketError)));

	CTcpEasySocket* mysock = new CTcpEasySocket(qsock);
	CConnection* pconn = createNewConnection(mysock);

	emit signalClientConnected(pconn);
}

void CConnectionServer::slotServerConnectionClosed(const CConnection* that)
{
	qDebug() << "CConnectionServer::slotServerConnectionClosed";

	Q_CHECK_PTR(that);

	emit signalClientDisconnected(that);

	if (m_pTcpServer->isListening() == false)
		m_pTcpServer->listen(QHostAddress::AnyIPv4, m_localPort);
}

void CConnectionServer::slotServerError(QAbstractSocket::SocketError socketError)
{
	qDebug() << QString("CConnectionServer::slotServerError %1").arg(socketError);

}
