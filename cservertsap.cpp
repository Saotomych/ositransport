#include "cservertsap.h"

CServerTSAP::CServerTSAP(qint32 port)
{
	this(port, 0, 0, CSocketFactory::GetSocketFactory());
}

CServerTSAP::CServerTSAP(qint32 port, qint32 backlog, QHostAddress bindAddr)
{
	this(port, backlog, bindAddr, conListener, CSocketFactory::GetSocketFactory());
}

CServerTSAP::CServerTSAP(qint32 port, qint32 backlog, QHostAddress bindAddr,
		CSocketFactory serverSocketFactory):
		maxTPduSizeParam(16),
		maxConnection(100),
		messageTimeout(0),
		messageFragmentTimeout(60000)
{
	if (port < 1 || port > 65535) {
		throw std::exception("port number is out of bound");
	}

	this->localPort = port;
	this->backlog = backlog;
	this->bindAddr = bindAddr;
	this->serverSocketFactory = serverSocketFactory;

	conListener = nullptr;
	serverThread = nullptr;
}

CConnectionListener& CServerTSAP::startListening()
{
	CConnectionListener* cl;
	CServerThread* cst;

	try
	{
		cl = new CConnectionListener();

		// TODO: FIX Magic numbers
		cst = new CServerThread(serverSocketFactory.CreateSocket(bindAddr, localPort),
				maxTPduSizeParam,
				messageTimeout,
				messageFragmentTimeout,
				maxConnection,
				cl );
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

	conListener = cl;
	serverThread = cst;
	serverThread->startServer();

	return *conListener;
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

static quint32 CServerTSAP::getMaxTPDUSize(int maxTPDUSizeParam)
{
	if (maxTPDUSizeParam < 7 || maxTPDUSizeParam > 16) {
		std::invalid_argument("CServerTSAP::getMaxTPDUSize: maxTPDUSizeParam is wrong.");
	}

	if (maxTPDUSizeParam == 16)
		return 65531;
	else
		return pow(2, maxTPDUSizeParam);
}
