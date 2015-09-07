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
		CSocketFactory serverSocketFactory) {
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
		cst = new CServerThread(serverSocketFactory.CreateSocket(bindAddr, localPort), 7, 1000, 1000, 10, cl);
	}

	catch(std::bad_alloc& ex)
	{
		qDebug() << ex.what();
		throw;
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

}

void CServerTSAP::setMaxConnections(int maxConnections)
{

}

void CServerTSAP::setMessageTimeout(int messageTimeout)
{

}

void CServerTSAP::setMessageFragmentTimeout(int messageFragmentTimeout)
{

}

int CServerTSAP::getMaxTPDUSizeParam()
{

}

static int CServerTSAP::getMaxTPDUSize(int maxTPDUSizeParam)
{

}
