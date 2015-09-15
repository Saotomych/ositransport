#include "servertsap.h"

CServerTSAP::CServerTSAP(qint32 _port):
		backlog(0),
		bindAddr(QHostAddress::AnyIPv4),
		conListener(nullptr),
		serverThread(nullptr),
		maxTPduSizeParam(16),
		maxConnection(100),
		messageTimeout(0),
		messageFragmentTimeout(60000)
{
	if (_port < 1 || _port > 65535) {
		throw std::invalid_argument("port number is out of bound");
	}

	localPort = _port;

	pServerSocketFactory = CSocketFactory::getSocketFactory();
}

CServerTSAP::CServerTSAP(qint32 _port, qint32 _backlog, QHostAddress _bindAddr):
		conListener(nullptr),
		serverThread(nullptr),
		maxTPduSizeParam(16),
		maxConnection(100),
		messageTimeout(60000),
		messageFragmentTimeout(60000)
{
	if (_port < 1 || _port > 65535) {
		throw std::invalid_argument("port number is out of bound");
	}

	localPort = _port;
	backlog = _backlog;
	bindAddr = _bindAddr;
	pServerSocketFactory = CSocketFactory::getSocketFactory();

}

CServerTSAP::CServerTSAP(qint32 _port, qint32 _backlog, QHostAddress _bindAddr,
		CSocketFactory* _serverSocketFactory):
		conListener(nullptr),
		serverThread(nullptr),
		maxTPduSizeParam(16),
		maxConnection(100),
		messageTimeout(60000),
		messageFragmentTimeout(60000)
{
	if (_port < 1 || _port > 65535) {
		throw std::invalid_argument("port number is out of bound");
	}

	localPort = _port;
	backlog = _backlog;
	bindAddr = _bindAddr;
	pServerSocketFactory = _serverSocketFactory;

}

void CServerTSAP::startListening()
{
	if (serverThread != nullptr)
		serverThread->startServer();
}

CConnectionListener* CServerTSAP::createServer()
{
	CConnectionListener* clistener = nullptr;
	CConnectionServer* cst = nullptr;

	try
	{
		clistener = new CConnectionListener();

		cst = new CConnectionServer(
				localPort,
				maxTPduSizeParam,
				messageTimeout,
				messageFragmentTimeout,
				maxConnection,
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

	conListener = clistener;
	serverThread = cst;

	return conListener;
}

void CServerTSAP::stopListening()
{
	if (serverThread != nullptr)
	{
		serverThread->stopServer();
		delete serverThread;
		serverThread = nullptr;
	}

	if (conListener != nullptr)
	{
		delete conListener;
		conListener = nullptr;
	}
}

void CServerTSAP::setMaxTPDUSizeParam(int maxTPDUSizeParam)
{
	this->maxTPduSizeParam = maxTPDUSizeParam;
}

void CServerTSAP::setMaxConnections(int maxConnections)
{
	this->maxConnection = maxConnections;
}

void CServerTSAP::setMessageTimeout(int messageTimeout)
{
	this->messageTimeout = messageTimeout;
}

void CServerTSAP::setMessageFragmentTimeout(int messageFragmentTimeout)
{
	this->messageFragmentTimeout = messageFragmentTimeout;
}

quint32 CServerTSAP::getMaxTPDUSizeParam()
{
	return maxTPduSizeParam;
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
