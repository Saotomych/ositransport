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

/**
 * @class CTcpEasySocket is the addition for QTcpSocket. Makes useful functions to the original socket.
 */

class CTcpEasySocket
{

private:

	const quint32 c_DefaultTimeout;

	QHostAddress host;
	QString strhost;
	qint16 port;
	QHostAddress localHost;
	qint16 localPort;

    Q_DISABLE_COPY(CTcpEasySocket)

	QTcpSocket* m_pTcpSocket;

	CTcpEasySocket();

public:

/**
 * @brief CTcpEasySocket constructor
 * @param tcpsock
 * 			is the new QTcpSocket
 */
	CTcpEasySocket(QTcpSocket* tcpsock);

	~CTcpEasySocket();

/**
 * @brief gives full access to original socket
 * @return
 */
	QTcpSocket* getSocket();

/**
 * @brief waits connection and report about it
 * @param msecs
 * 			timeout
 * @return true or false for connection successful
 */
	bool waitForConnected(quint32 msecs = TCP_EASY_SOCKET_DEFAULT_TIMEOUT);

/**
 * @brief sets local host and port
 * @param localHost
 * 			local host
 * @param localPort
 * 			local port
 * @return true or false
 */
	bool setLocalHostPort(QHostAddress localHost, qint16 localPort);

/**
 * @brief creates connection to host port
 * @param host
 * 			remote host as QHostAddress
 * @param port
 * 			remote port
 */
	void connectToHost(QHostAddress host, qint16 port);

/**
 * @brief creates connection to host port
 * @param host
 * 			remote host as QString
 * @param port
 * 			remote port
 */
	void connectToHost(QString host, qint16 port);

/**
 * @brief getter for remote host
 * @return remote host
 */
	QHostAddress getHost();

/**
 * @brief getter for remote port
 * @return remote port
 */
	qint16 getPort();
};

#endif /* TCPEASYSOCKET_H_ */
