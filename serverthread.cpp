#include "serverthread.h"

quint32 CServerThread::s_socketCounter = 0;

CServerThread::CServerThread(
		CTcpEasySocket sock,
		quint32 maxTPduSizeParam,
		quint32 msgTimeout,
		quint32 msgFragmentTimeout,
		quint32 maxConnections,
		CConnectionListener* listener):
		m_TcpSocket(sock),
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
		CConnection* pconn = new CConnection(m_TcpSocket, m_maxTPduSizeParam, m_messageTimeout, m_messageFragmentTimeout);

		// signals from Connection to ServerThread
		connect(pconn, SIGNAL(signalIOError), this, SLOT(slotIOError));
		connect(pconn, SIGNAL(signalCRReady), this, SLOT(slotConnectionReady));
		connect(pconn, SIGNAL(signalConnectionClosed), this, SLOT(slotConnectionClosed));

		// signals from Connection to ConnectionListener
		connect(pconn, SIGNAL(signalConnectionReady), m_pConnListener, SLOT(slotConnectionReady));
		connect(pconn, SIGNAL(signalConnectionClosed), m_pConnListener, SLOT(slotConnectionClosed));
		connect(pconn, SIGNAL(signalCRReady), m_pConnListener, SLOT(slotCRReady));
		connect(pconn, SIGNAL(signalTSduReady), m_pConnListener, SLOT(slotTSduReady));
		connect(pconn, SIGNAL(signalIOError), m_pConnListener, SLOT(slotIOError));
		connect(pconn, SIGNAL(signalConnectionStateChanged()), m_pConnListener, SLOT(slotConnectionStateChanged()));

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
	Q_CHECK_PTR(pconn);

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
	m_pConnection->setListenSocket();
	m_pConnection->listenForCR();
}

void CServerThread::slotConnectionClosed(const CConnection* that)
{
	Q_CHECK_PTR(that);

	if (s_socketCounter)
	{
		s_socketCounter--;

		delete that;

		startConnectionThread(createNewConnection());
	}
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
