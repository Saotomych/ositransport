/*
 * socketfactory.cpp
 *
 *  Created on: 31 июля 2015 г.
 *      Author: alexey
 */

#include "socketfactory.h"

CSocketFactory* CSocketFactory::s_inst = nullptr;

QMutex CSocketFactory::s_mut;

/*
 * Creates an unconnected socket.
 */
CTcpEasySocket* CSocketFactory::createSocket()
{
	CTcpEasySocket* ts = new CTcpEasySocket(new QTcpSocket());

	return ts;
}

void CSocketFactory::printConnectResult(bool bResult, CTcpEasySocket* ts, QString host, quint16 port)
{
	if (bResult)
	{
		qDebug() << QString("Connected to %1:%2.").arg(host).arg(port);
	}
	else
	{
		QString err = ts->getSocket()->errorString();
		qDebug() << QString("Connection to %1:%2 wrong = %3.").arg(host).arg(port).arg(err);
	}
}

/*
 * Creates a socket and connects it to the specified remote host at the specified remote port.
 * This socket is configured using the socket options established for this factory.
 */
CTcpEasySocket* CSocketFactory::createSocket(QString host, quint16 port)
{
	CTcpEasySocket* ts = new CTcpEasySocket(new QTcpSocket());

	QHostAddress h(host);
	ts->getSocket()->bind(h, port);
	ts->getSocket()->connectToHost(host, port);
	bool bRes = ts->getSocket()->waitForConnected(connect_tout);

	printConnectResult( bRes, ts, host, port );

	return ts;
}

/*
 * Creates a socket and connects it to the specified port number at the specified address.
 * This socket is configured using the socket options established for this factory.
 */
CTcpEasySocket* CSocketFactory::createSocket(QHostAddress host, quint16 port)
{
	CTcpEasySocket* ts = new CTcpEasySocket(new QTcpSocket());

	ts->getSocket()->bind(host, port);
	ts->getSocket()->connectToHost(host, port);
	bool bRes = ts->getSocket()->waitForConnected(connect_tout);

	printConnectResult( bRes, ts, host.toString(), port );

	return ts;
}

/*
 * Creates a socket and connects it to the specified remote host on the specified remote port.
 * The socket will also be bound to the local address and port supplied.
 * This socket is configured using the socket options established for this factory.
 */
CTcpEasySocket* CSocketFactory::createSocket(QString host, quint16 port, QHostAddress localHost, quint16 localPort)
{
	CTcpEasySocket* ts = new CTcpEasySocket(new QTcpSocket());
	ts->setLocalHostPort(localHost, localPort);

	QHostAddress h(host);
	ts->getSocket()->bind(h, port);
	ts->getSocket()->connectToHost(host, port);
	bool bRes = ts->getSocket()->waitForConnected(connect_tout);

	printConnectResult( bRes, ts, host, port );

	return ts;
}

/*
 * Creates a socket and connect it to the specified remote address on the specified remote port.
 * The socket will also be bound to the local address and port supplied.
 * The socket is configured using the socket options established for this factory.
 */
CTcpEasySocket* CSocketFactory::createSocket(QHostAddress host, quint16 port, QHostAddress localHost, quint16 localPort)
{
	CTcpEasySocket* ts = new CTcpEasySocket(new QTcpSocket());
	ts->setLocalHostPort(localHost, localPort);

	ts->getSocket()->bind(host, port);
	ts->getSocket()->connectToHost(host, port);
	bool bRes = ts->getSocket()->waitForConnected(connect_tout);

	printConnectResult( bRes, ts, host.toString(), port );

	return ts;
}

void CSocketFactory::setConnectTimeout(int tout)
{
	connect_tout = tout;
}

