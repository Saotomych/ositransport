#include "cservertsap.h"

CServerTSAP::CServerTSAP(qint32 port, CConnectionListener conListener)
{
	this(port, 0, 0, conListener, CSocketFactory::GetSocketFactory());
}

CServerTSAP::CServerTSAP(qint32 port, qint32 backlog, QHostAddress bindAddr, CConnectionListener conListener)
{
	this(port, backlog, bindAddr, conListener, CSocketFactory::GetSocketFactory());
}

CServerTSAP::CServerTSAP(qint32 port, qint32 backlog, QHostAddress bindAddr, CConnectionListener conListener,
		CSocketFactory serverSocketFactory) {
	if (port < 1 || port > 65535) {
		throw std::exception("port number is out of bound");
	}

	this->localPort = port;
	this->backlog = backlog;
	this->bindAddr = bindAddr;
	this->conListener = conListener;
	this->serverSocketFactory = serverSocketFactory;
}

