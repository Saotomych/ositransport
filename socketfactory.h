/*
 * socketfactory.h
 *
 *  Created on: 31 июля 2015 г.
 *      Author: alexey
 */

#ifndef SOCKETFACTORY_H_
#define SOCKETFACTORY_H_

#include "ositransport_global.h"
#include "tcpeasysocket.h"
#include "connection.h"

/**
 *  @class QSocketFactory creates CTcpEasySockets. (Singleton)
 */

class OSITRANSPORTSHARED_EXPORT CSocketFactory
{
private:
	CSocketFactory(): connect_tout(30000) {}
	CSocketFactory(const CSocketFactory& that): connect_tout(that.connect_tout) { *this = that; }
	CSocketFactory& operator=(const CSocketFactory& that){ *this = that; return *this;}

	static CSocketFactory* s_pInst;
	static QMutex s_mut;

	quint32 connect_tout;

public:

/**
 * @brief creates SocketFactory as singleton
 * @return pointer socket factory
 */
	static CSocketFactory* getSocketFactory()
	{
		s_mut.lock();

		if (s_pInst == nullptr)
		{
			try
			{
				s_pInst = new CSocketFactory();
			}

			catch (std::bad_alloc& ex)
			{
				qDebug() << ex.what();
				throw;
			}
		}

		s_mut.unlock();

		return s_pInst;
	}

/**
 * @brief sets timeout for the tcp/ip connection waiting
 * @param tout
 * 			timeout in milliseconds
 */
	void setConnectTimeout(int tout);

/**
 * @brief all createSocket functions returns new socket for connection
 * @return new socket for connection
 */
	CTcpEasySocket* createSocket();
	CTcpEasySocket* createSocket(QString host, quint16 port);
	CTcpEasySocket* createSocket(QHostAddress host, quint16 port);
	CTcpEasySocket* createSocket(QString host, quint16 port, QHostAddress localHost, quint16 localPort);
	CTcpEasySocket* createSocket(QHostAddress host, quint16 port, QHostAddress localHost, quint16 localPort);

	void printConnectResult(bool bResult, CTcpEasySocket* ts, QString host, quint16 port);
};

#endif /* SOCKETFACTORY_H_ */
