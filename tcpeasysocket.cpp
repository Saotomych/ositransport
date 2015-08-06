/*
 * tcpeasysocket.cpp
 *
 *  Created on: 31 июля 2015 г.
 *      Author: alexey
 */

#include "tcpeasysocket.h"

CTcpEasySocket::CTcpEasySocket():
		strhost(QString("")),
		host((unsigned int)0),
		port(0),
		localHost((unsigned int)0),
		localPort(0),
		m_messageTimeout(TCP_EASY_SOCKET_DEFAULT_TIMEOUT),
		m_messageFragmentTimeout(TCP_EASY_SOCKET_DEFAULT_TIMEOUT)
{
}

CTcpEasySocket::CTcpEasySocket(const CTcpEasySocket& that)
{
	strhost = that.strhost;
	host = that.host;
	port = that.port;
	localHost = that.localHost;
	localPort = that.localPort;
	m_messageTimeout = that.m_messageTimeout;
	m_messageFragmentTimeout = that.m_messageFragmentTimeout;
}

CTcpEasySocket CTcpEasySocket::operator=(const CTcpEasySocket& that)
{
	strhost = that.strhost;
	host = that.host;
	port = that.port;
	localHost = that.localHost;
	localPort = that.localPort;
	m_messageTimeout = that.m_messageTimeout;
	m_messageFragmentTimeout = that.m_messageFragmentTimeout;

	return *this;
}

CTcpEasySocket::~CTcpEasySocket() {
}

bool CTcpEasySocket::waitForConnected(int msec)
{
	if (!QTcpSocket::waitForConnected(msec))
	{
		QString str = QString("Connect failed to host %1:%2 after %3 ms.").arg(localAddress().toString(), QString::number((short)localPort()), QString::number(msec));
		QDebug(str);
		return false;
	}

	return true;
}

bool CTcpEasySocket::setLocalHostPort(QHostAddress localHost, qint16 localPort)
{
	if (!bind(localHost, localPort))
	{
		QString str = QString("Bind to local host %1:%2 failed.").arg(localAddress().toString(), QString::number((short)localPort()));
		QDebug(str);
		return false;
	}

	this->localHost = localHost;
	this->localPort = localPort;

	return true;
}

void CTcpEasySocket::connectToHost(QHostAddress host, qint16 port)
{
	this->host = host;
	this->port = port;
	this->strhost = QString("");
	QTcpSocket::connectToHost(host, port);
}

void CTcpEasySocket::connectToHost(QString strhost, qint16 port)
{
	this->strhost = strhost;
	this->host = 0;
	this->port = port;
	QTcpSocket::connectToHost(host, port);
}
