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

/*
 *  QSocketFactory creates CTcpEasySockets
 *  Singleton
 */
class OSITRANSPORTSHARED_EXPORT CSocketFactory
{
private:
	CSocketFactory(): connect_tout(30000) {}
	CSocketFactory(const CSocketFactory& that): connect_tout(30000) { *this = that; }
	CSocketFactory& operator=(const CSocketFactory& that){ *this = that; return *this;}

	static CSocketFactory* s_inst;
	static QMutex s_mut;

	quint32 connect_tout;

public:
	static CSocketFactory* getSocketFactory()
	{
		s_mut.lock();

		if (s_inst == nullptr)
		{
			try
			{
				s_inst = new CSocketFactory();
			}

			catch (std::bad_alloc& ex)
			{
				qDebug() << ex.what();
				throw;
			}
		}

		s_mut.unlock();

		return s_inst;
	}

	void setConnectTimeout(int tout);

	CTcpEasySocket* createSocket();
	CTcpEasySocket* createSocket(QString host, quint16 port);
	CTcpEasySocket* createSocket(QHostAddress host, quint16 port);
	CTcpEasySocket* createSocket(QString host, quint16 port, QHostAddress localHost, quint16 localPort);
	CTcpEasySocket* createSocket(QHostAddress host, quint16 port, QHostAddress localHost, quint16 localPort);

	void printConnectResult(bool bResult, CTcpEasySocket* ts, QString host, quint16 port);
};

#endif /* SOCKETFACTORY_H_ */
