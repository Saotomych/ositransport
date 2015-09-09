/*
 * socketfactory.cpp
 *
 *  Created on: 31 июля 2015 г.
 *      Author: alexey
 */

#include "socketfactory.h"

CSocketFactory* CSocketFactory::inst = nullptr;

/*
 * Creates an unconnected socket.
 */
CTcpEasySocket CSocketFactory::createSocket()
{
	CTcpEasySocket ts();

	return ts;
}

/*
 * Creates a socket and connects it to the specified remote host at the specified remote port.
 * This socket is configured using the socket options established for this factory.
 */
CTcpEasySocket CSocketFactory::createSocket(QString host, quint16 port)
{
	CTcpEasySocket ts;
	ts.connectToHost(host, port);
	ts.waitForConnected(connect_tout);

	return ts;
}

/*
 * Creates a socket and connects it to the specified port number at the specified address.
 * This socket is configured using the socket options established for this factory.
 */
CTcpEasySocket CSocketFactory::createSocket(QHostAddress host, quint16 port)
{
	CTcpEasySocket ts;
	ts.connectToHost(host, port);
	ts.waitForConnected(connect_tout);

	return ts;
}

/*
 * Creates a socket and connects it to the specified remote host on the specified remote port.
 * The socket will also be bound to the local address and port supplied.
 * This socket is configured using the socket options established for this factory.
 */
CTcpEasySocket CSocketFactory::createSocket(QString host, quint16 port, QHostAddress localHost, quint16 localPort)
{
	CTcpEasySocket ts;
	ts.setLocalHostPort(localHost, localPort);
	ts.connectToHost(host, port);
	ts.waitForConnected(connect_tout);

	return ts;
}

/*
 * Creates a socket and connect it to the specified remote address on the specified remote port.
 * The socket will also be bound to the local address and port supplied.
 * The socket is configured using the socket options established for this factory.
 */
CTcpEasySocket CSocketFactory::createSocket(QHostAddress host, quint16 port, QHostAddress localHost, quint16 localPort)
{
	CTcpEasySocket ts;
	ts.setLocalHostPort(localHost, localPort);
	ts.connectToHost(host, port);
	ts.waitForConnected(connect_tout);

	return ts;
}

void CSocketFactory::setConnectTimeout(int tout)
{
	connect_tout = tout;
}

