/*
 * tcpeasysocket.cpp
 *
 *  Created on: 31 июля 2015 г.
 *      Author: alexey
 */

#include "tcpeasysocket.h"

CTcpEasySocket::CTcpEasySocket(QTcpSocket* tcpsock):
		c_DefaultTimeout(TCP_EASY_SOCKET_DEFAULT_TIMEOUT),
		host((unsigned int)0),
		strhost(QString("")),
		port(0),
		localHost((unsigned int)0),
		localPort(0),
		m_pTcpSocket(tcpsock)
{
}

CTcpEasySocket::~CTcpEasySocket() {
}

QTcpSocket* CTcpEasySocket::getSocket()
{
	return m_pTcpSocket;
}

bool CTcpEasySocket::waitForConnected(quint32 msec)
{
	if (!m_pTcpSocket->QTcpSocket::waitForConnected(msec))
	{
		qDebug() << QString("Connect failed to host %1:%2 after %3 ms.").arg(m_pTcpSocket->localAddress().toString(), QString::number((short)localPort), QString::number(msec));
		return false;
	}

	return true;
}

bool CTcpEasySocket::setLocalHostPort(QHostAddress localHost, qint16 localPort)
{
	if (!m_pTcpSocket->bind(localHost, localPort))
	{
		qDebug() << QString("Bind to local host %1:%2 failed.").arg(m_pTcpSocket->localAddress().toString(), QString::number((short)localPort));
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
	m_pTcpSocket->QTcpSocket::connectToHost(host, port);
}

void CTcpEasySocket::connectToHost(QString strhost, qint16 port)
{
	this->strhost = strhost;
	this->host = 0;
	this->port = port;
	m_pTcpSocket->QTcpSocket::connectToHost(host, port);
}

QHostAddress CTcpEasySocket::getHost() { return host; }

qint16 CTcpEasySocket::getPort() { return port; }

