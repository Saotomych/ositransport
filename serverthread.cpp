#include "serverthread.h"

quint32 CServerThread::s_socketCounter = 0;

CServerThread::	CServerThread(
		CTcpEasySocket* sock,
		quint32 maxTPduSizeParam,
		quint32 msgTimeout,
		quint32 msgFragmentTimeout,
		quint32 maxConnections,
		CConnectionListener* listener):
		m_pTcpSocket(sock),
		m_maxTPduSizeParam(maxTPduSizeParam),
		m_messageTimeout(msgTimeout),
		m_messageFragmentTimeout(msgFragmentTimeout),
		m_maxConnections(maxConnections),
		m_pConnListener(listener)
{
}

CConnection* CServerThread::createNewConnection()
{
	try
	{
		CConnection* pconn = new CConnection(m_pTcpSocket, m_maxTPduSizeParam, m_messageTimeout, m_messageFragmentTimeout);

		// signals from Connection to ServerThread
		QObject::connect(pconn, SIGNAL(signalIOError), this, SLOT(slotConnectionClosed));
		QObject::connect(pconn, SIGNAL(signalCRReady), this, SLOT(slotConnectionReady));

		// signals from Connection to ConnectionListener
		QObject::connect(pconn, SIGNAL(signalConnectionReady), m_pConnListener, SLOT(slotConnectionReady));
		QObject::connect(pconn, SIGNAL(signalConnectionClosed), m_pConnListener, SLOT(slotConnectionClosed));
		QObject::connect(pconn, SIGNAL(signalCRReady), m_pConnListener, SLOT(slotCRReady));
		QObject::connect(pconn, SIGNAL(signalTSduReady), m_pConnListener, SLOT(slotTSduReady));
		QObject::connect(pconn, SIGNAL(signalIOError), m_pConnListener, SLOT(slotIOError));

		return pconn;
	}

	catch (std::bad_alloc& ex)
	{
		qDebug() << ex.what();
		throw;
	}
}

void CServerThread::startServer()
{
	startConnectionThread(createNewConnection());
}

void CServerThread::startConnectionThread(CConnection* pconn)
{
	if (s_socketCounter < getMaxConnections())
	{
		try
		{
			CServerThread::CConnectionThread* pst = new CServerThread::CConnectionThread(this, pconn);
			pst->setAutoDelete(true);
			if (addThread2ServerPool(pst)) s_socketCounter++;
		}

		catch(std::bad_alloc& ex)
		{
			qDebug() << ex.what();
		}
	}
}

void CServerThread::CConnectionThread::run()
{
	connect(m_pTcpSocket, SIGNAL(stateChanged()), m_pConnection, SLOT(slotSocketStateChanged()));
	connect(m_pConnection, SIGNAL(signalConnectionStateChanged()), m_pServerThread, SLOT(slotConnectionStateChanged()));

	// TODO: localPort for listening as parameter
	m_pTcpSocket->setListenPort(18899);
	m_pConnection->listenForCR();
}

void CServerThread::slotConnectionClosed(const CConnection* that)
{
	Q_CHECK_PTR(that);

	if (s_socketCounter)
	{
		s_socketCounter--;

		emit signalUserDisconnected(that);

		startConnectionThread(createNewConnection());
	}
}

void CServerThread::slotConnectionReady(const CConnection* that)
{
	Q_CHECK_PTR(that);
	emit signalUserConnected();
}

void CServerThread::slotTryNewConnection(const CConnection* that)
{
	emit signalUserConnected(that);
}

quint32 CServerThread::getMaxConnections()
{
	return m_maxConnections;
}

void CServerThread::setMaxConnections(quint32 maxConnections)
{
	m_maxConnections = maxConnections;
}

bool CServerThread::addThread2ServerPool(QRunnable* obj)
{
	Q_CHECK_PTR(obj);
	return m_srvThreadPool.tryStart(obj);
}

void CServerThread::stopServer()
{
	m_srvThreadPool.clear();
	m_srvThreadPool.waitForDone();
}

// TODO Realize stateChanged of socket in server
void CServerThread::slotConnectionStateChanged(CConnection* pConnection, QAbstractSocket::SocketState socketState)
{

}
