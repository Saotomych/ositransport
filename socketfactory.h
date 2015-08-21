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

/*
 *  QSocketFactory creates CTcpEasySockets
 *  Singleton
 */
class CSocketFactory
{
private:
	CSocketFactory():connect_tout(30000){}
	CSocketFactory(const CSocketFactory& that){ *this = that; }
	CSocketFactory& operator=(const CSocketFactory& that){ *this = that; return *this;}

	static CSocketFactory* inst;

	static QMutex mut;

	int connect_tout;

public:
	static CSocketFactory* GetSocketFactory()
	{
		mut.lock();

		if (inst == nullptr)
		{
			inst = new CSocketFactory();
		}

		mut.unlock();

		return inst;
	}

	void SetConnectTimeout(int tout);

	CTcpEasySocket CreateSocket();
	CTcpEasySocket CreateSocket(QString host, quint16 port);
	CTcpEasySocket CreateSocket(QHostAddress host, quint16 port);
	CTcpEasySocket CreateSocket(QString host, quint16 port, QHostAddress localHost, quint16 localPort);
	CTcpEasySocket CreateSocket(QHostAddress host, quint16 port, QHostAddress localHost, quint16 localPort);
};



#endif /* SOCKETFACTORY_H_ */
