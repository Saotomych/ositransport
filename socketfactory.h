/*
 * socketfactory.h
 *
 *  Created on: 31 июля 2015 г.
 *      Author: alexey
 */

#ifndef SOCKETFACTORY_H_
#define SOCKETFACTORY_H_

#include "ositransport_global.h"

/*
 *  QSocketFactory creates CTcpEasySockets
 *  Singleton
 */
class CSocketFactory
{
private:
	CSocketFactory():connect_tout(30000){}
	CSocketFactory(const CSocketFactory& that){}
	CSocketFactory& operator=(const CSocketFactory& that){ return *this;}

	static const CSocketFactory* inst;

	static QMutex mut;

	int connect_tout;

public:
	static const CSocketFactory& GetSocketFactory()
	{
		mut.lock();

		if (inst == nullptr)
		{
			inst = new CSocketFactory();
		}

		mut.unlock();

		return *inst;
	}

	void SetConnectTimeout(int tout);

	CTcpEasySocket CreateSocket();
	CTcpEasySocket CreateSocket(QString host, qint16 port);
	CTcpEasySocket CreateSocket(QHostAddress host, qint16 port);
	CTcpEasySocket CreateSocket(QString host, qint16 port, QHostAddress localHost, qint16 localPort);
	CTcpEasySocket CreateSocket(QHostAddress host, qint16 port, QHostAddress localHost, qint16 localPort);
};



#endif /* SOCKETFACTORY_H_ */
