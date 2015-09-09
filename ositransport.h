#ifndef OSITRANSPORT_H
#define OSITRANSPORT_H

#include <cppunit/TestCase.h>
#include <cppunit/ui/text/TextTestRunner.h>
#include <cppunit/XmlOutputterHook.h>
#include <cppunit/XmlOutputter.h>

#define UNITTEST
#include "ositransport_global.h"
#include "cservertsap.h"
#include "cclienttsap.h"
#include "cconnection.h"
#include "cconnectionlistener.h"
#include "socketfactory.h"

class OsiTransportTest: public QObject
{

	Q_OBJECT

	OsiTransportTest():
		pServer(nullptr),
		pClient(nullptr),
		pConnection(nullptr),
		pConnectionListener(nullptr),
		checkClientConnected(false),
		checkClientErrorConnected(false),
		checkServerConnected(false),
		checkServerErrorConnected(false),
		checkIllegalArg(false),
		checkIllegalClassMbr(false)
	{}

public:

	CServerTSAP* pServer;
	CClientTSAP* pClient;
	CConnection* pConnection;
	CConnectionListener* pConnectionListener;

	bool checkClientConnected;
	bool checkClientErrorConnected;
	bool checkServerConnected;
	bool checkServerErrorConnected;
	bool checkIllegalArg;
	bool checkIllegalClassMbr;

	static const char testData[];

	static OsiTransportTest* getMainTest() {
		static OsiTransportTest* pthis = nullptr;
		if (pthis == nullptr)
		{
			pthis = new OsiTransportTest();
		}
		return pthis;
	}

	// Test Classes
	/*
	 *  Class tests creation of connection client-server
	 */
	class Test1:public CppUnit::TestCase
	{
	public:
		Test1(const std::string str): TestCase(str) {}
		void prepareTest();
		void runTest();
	};

	/*
	 *  Class tests data sending from client to server
	 */
	class Test2:public CppUnit::TestCase
	{
	public:
		Test2(const std::string str): TestCase(str) {}
		void runTest();
	};

	/*
	 *  Class tests data receiving from server to client
	 */
	class Test3:public CppUnit::TestCase
	{
	public:
		Test3(const std::string str): TestCase(str) {}
		void runTest();
	};

public slots:

	// server slots
	void slotClientConnected(const CConnection* that);
	void slotClientDisconnected(const CConnection* that);
	void slotServerRcvTSdu(const CConnection* that);
	void slotServerCRReady(const CConnection* that);
	void slotServerIOError(QString str);

	// client slots
	void slotConnectionReady(const CConnection* that);
	void slotConnectionClosed(const CConnection* that);
	void slotTSReady(const CConnection* that);
	void slotCRReady(const CConnection* that);
	void slotIOError(QString str);

	// Client Errors
	void slotIllegalArgument(QString strErr);
	void slotConnectError(QString strErr);
	void slotIllegalClassMember(QString strErr);

};

#endif // OSITRANSPORT_H
