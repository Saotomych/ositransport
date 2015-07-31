/*
 * tcpeasysocket.h
 *
 *  Created on: 31 июля 2015 г.
 *      Author: alexey
 */

#ifndef TCPEASYSOCKET_H_
#define TCPEASYSOCKET_H_

#include "ositransport_global.h"

class CTcpEasySocket: private QTcpSocket
{
private:
	QHostAddress host;
	QString strhost;
	qint16 port;
	QHostAddress localHost;
	qint16 localPort;

public:
	CTcpEasySocket();
	~CTcpEasySocket();

	bool waitForConnected(int msecs = 30000);
	bool setLocalHostPort(QHostAddress localHost, qint16 localPort);
	void connectToHost(QHostAddress host, qint16 port);
	void connectToHost(QString host, qint16 port);

};

#endif /* TCPEASYSOCKET_H_ */
