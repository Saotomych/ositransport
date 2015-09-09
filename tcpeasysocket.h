/*
 * tcpeasysocket.h
 *
 *  Created on: 31 июля 2015 г.
 *      Author: alexey
 */

#ifndef TCPEASYSOCKET_H_
#define TCPEASYSOCKET_H_

#include "ositransport_global.h"

#define TCP_EASY_SOCKET_DEFAULT_TIMEOUT	30000

class CTcpEasySocket: public QTcpSocket
{

private:

	const quint32 c_DefaultTimeout;

	QHostAddress host;
	QString strhost;
	qint16 port;
	QHostAddress localHost;
	qint16 localPort;

public:
	CTcpEasySocket();
	CTcpEasySocket(const CTcpEasySocket& that);
	CTcpEasySocket operator=(const CTcpEasySocket& that);

	~CTcpEasySocket();

	bool waitForConnected(quint32 msecs = TCP_EASY_SOCKET_DEFAULT_TIMEOUT);
	bool setLocalHostPort(QHostAddress localHost, qint16 localPort);
	void connectToHost(QHostAddress host, qint16 port);
	void connectToHost(QString host, qint16 port);
	void setListen();
};

#endif /* TCPEASYSOCKET_H_ */
