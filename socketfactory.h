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
#include "cconnection.h"

/*
 *  QSocketFactory creates CTcpEasySockets
 *  Singleton
 */
class CSocketFactory
{
private:
	CSocketFactory(){}
	CSocketFactory(const CSocketFactory& that){ *this = that; }
	CSocketFactory& operator=(const CSocketFactory& that){ *this = that; return *this;}

	static CSocketFactory* inst;
	static QMutex mut;

	quint32 connect_tout;

public:
	static CSocketFactory* getSocketFactory()
	{
		mut.lock();

		if (inst == nullptr)
		{
			try
			{
				inst = new CSocketFactory();
			}

			catch (std::bad_alloc& ex)
			{
				qDebug() << ex.what();
				throw;
			}
		}

		mut.unlock();

		return inst;
	}

	void setConnectTimeout(int tout);

	CTcpEasySocket createSocket();
	CTcpEasySocket createSocket(QString host, quint16 port);
	CTcpEasySocket createSocket(QHostAddress host, quint16 port);
	CTcpEasySocket createSocket(QString host, quint16 port, QHostAddress localHost, quint16 localPort);
	CTcpEasySocket createSocket(QHostAddress host, quint16 port, QHostAddress localHost, quint16 localPort);
};



#endif /* SOCKETFACTORY_H_ */
