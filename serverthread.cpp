#include "serverthread.h"

quint32 CServerThreadPool::s_socketCounter = 0;

CServerThreadPool::	CServerThreadPool(
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

CConnection* CServerThreadPool::createNewConnection()
{
	try
	{
		CConnection* pconn = new CConnection(m_pTcpSocket, m_maxTPduSizeParam, m_messageTimeout, m_messageFragmentTimeout);
		QObject::connect(pconn, SIGNAL(signalIOError), this, SLOT(slotConnectionClosed));
		QObject::connect(pconn, SIGNAL(signalCRReady), this, SLOT(slotConnectionReady));

		m_setConnection.insert(pconn);

		return pconn;
	}

	catch (std::bad_alloc& ex)
	{
		qDebug() << ex.what();
		throw;
	}
}

void CServerThreadPool::startConnectionThread(CConnection* pconn)
{
	if (s_socketCounter < getMaxConnections())
	{
		try
		{
			CServerThreadPool::CConnectionThread* pst = new CServerThreadPool::CConnectionThread(pconn);
			pst->setAutoDelete(true);
			if (addThread2ServerPool(pst)) s_socketCounter++;
		}

		catch(std::bad_alloc& ex)
		{
			qDebug() << ex.what();
		}
	}
}

void CServerThreadPool::CConnectionThread::run()
{
	m_pConnection->listenForCR();
}

void CServerThreadPool::slotConnectionClosed(const CConnection* that)
{
	Q_CHECK_PTR(that);

	if (s_socketCounter)
	{
		s_socketCounter--;

		m_setConnection.remove(const_cast<CConnection*>(that));
		delete that;

		emit signalUserDisconnected();

		startConnectionThread(createNewConnection());
	}
}

void CServerThreadPool::slotConnectionReady(const CConnection* that)
{
	Q_CHECK_PTR(that);
	signalUserConnected();
}

void CServerThreadPool::slotTryNewConnection(const CConnection* that)
{

}

quint32 CServerThreadPool::getMaxConnections()
{
	return m_maxConnections;
}

void CServerThreadPool::setMaxConnections(quint32 maxConnections)
{
	m_maxConnections = maxConnections;
}

bool CServerThreadPool::addThread2ServerPool(QRunnable* obj)
{
	Q_CHECK_PTR(obj);
	return m_srvThreadPool.tryStart(obj);
}

void CServerThreadPool::stopServer()
{
	m_srvThreadPool.clear();
	m_srvThreadPool.waitForDone();
}
