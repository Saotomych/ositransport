#include "servertsap.h"

CServerTSAP::CServerTSAP(qint32 _port):
		m_backlog(0),
		m_bindAddr(QHostAddress::AnyIPv4),
		m_conListener(nullptr),
		m_serverThread(nullptr),
		m_maxTPduSizeParam(16),
		m_maxConnection(100),
		m_messageTimeout(5000),
		m_messageFragmentTimeout(5000)
{
	if (_port < 1 || _port > 65535) {
		throw std::invalid_argument("port number is out of bound");
	}

	m_localPort = _port;

	m_pServerSocketFactory = CSocketFactory::getSocketFactory();
}

CServerTSAP::CServerTSAP(qint32 _port, qint32 _backlog, QHostAddress _bindAddr):
		m_conListener(nullptr),
		m_serverThread(nullptr),
		m_maxTPduSizeParam(16),
		m_maxConnection(100),
		m_messageTimeout(5000),
		m_messageFragmentTimeout(5000)
{
	if (_port < 1 || _port > 65535) {
		throw std::invalid_argument("port number is out of bound");
	}

	m_localPort = _port;
	m_backlog = _backlog;
	m_bindAddr = _bindAddr;
	m_pServerSocketFactory = CSocketFactory::getSocketFactory();

}

CServerTSAP::CServerTSAP(qint32 port, qint32 backlog, QHostAddress bindAddr,
		CSocketFactory* serverSocketFactory):
		m_conListener(nullptr),
		m_serverThread(nullptr),
		m_maxTPduSizeParam(16),
		m_maxConnection(100),
		m_messageTimeout(60000),
		m_messageFragmentTimeout(60000)
{
	if (port < 1 || port > 65535) {
		throw std::invalid_argument("port number is out of bound");
	}

	m_localPort = port;
	m_backlog = backlog;
	m_bindAddr = bindAddr;
	m_pServerSocketFactory = serverSocketFactory;

}

void CServerTSAP::startListening()
{
	if (m_serverThread != nullptr)
		m_serverThread->startServer();
}

CConnectionListener* CServerTSAP::createServer()
{
	CConnectionListener* clistener = nullptr;
	CConnectionServer* cst = nullptr;

	try
	{
		clistener = new CConnectionListener();
		cst = new CConnectionServer(
				nullptr,
				m_localPort,
				m_maxTPduSizeParam,
				m_messageTimeout,
				m_messageFragmentTimeout,
				m_maxConnection,
				clistener );
	}

	catch(std::bad_alloc& ex)
	{
		qDebug() << "CServerTSAP::startListening(): " << ex.what();
		throw;
	}

	catch(std::exception& ex)
	{
		qDebug() << "CServerTSAP::startListening(): " << ex.what();
	}

	catch(...)
	{
		qDebug() << "CServerTSAP::startListening(): Unknown exception";
	}

	m_conListener = clistener;
	m_serverThread = cst;

	return m_conListener;
}

void CServerTSAP::stopListening()
{
	if (m_serverThread != nullptr)
	{
		m_serverThread->stopServer();
		delete m_serverThread;
		m_serverThread = nullptr;
	}

	if (m_conListener != nullptr)
	{
		delete m_conListener;
		m_conListener = nullptr;
	}
}

void CServerTSAP::setMaxTPDUSizeParam(int maxTPDUSizeParam)
{
	this->m_maxTPduSizeParam = maxTPDUSizeParam;
}

void CServerTSAP::setMaxConnections(int maxConnections)
{
	this->m_maxConnection = maxConnections;
}

void CServerTSAP::setMessageTimeout(int messageTimeout)
{
	this->m_messageTimeout = messageTimeout;
}

void CServerTSAP::setMessageFragmentTimeout(int messageFragmentTimeout)
{
	this->m_messageFragmentTimeout = messageFragmentTimeout;
}

quint32 CServerTSAP::getMaxTPDUSizeParam()
{
	return m_maxTPduSizeParam;
}

/*static*/
quint32 CServerTSAP::getMaxTPDUSize(int maxTPDUSizeParam)
{
	if (maxTPDUSizeParam < 7 || maxTPDUSizeParam > 16) {
		std::invalid_argument("CServerTSAP::getMaxTPDUSize: maxTPDUSizeParam is wrong.");
	}

	if (maxTPDUSizeParam == 16)
		return 65531;
	else
		return pow(2, maxTPDUSizeParam);
}