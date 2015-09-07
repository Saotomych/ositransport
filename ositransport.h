#ifndef OSITRANSPORT_H
#define OSITRANSPORT_H

#include <cppunit/TestCase.h>
#include <cppunit/ui/text/TextTestRunner.h>
#include <cppunit/XmlOutputterHook.h>
#include <cppunit/XmlOutputter.h>

#define UNITTEST
#include "ositransport_global.h"

class OsiTransportTest: public QObject
{

	Q_OBJECT
private:
	void connectConnection(const CConnection* that);
	void connectConnectionListener(const CConnectionListener* that);

public:
	// Test Classes
	/*
	 *  Class tests creation of connection client-server
	 */
	class Test1:public CppUnit::TestCase
	{

	};

	/*
	 *  Class tests data sending from client to server
	 */
	class Test2:public CppUnit::TestCase
	{

	};

	/*
	 *  Class tests data receiving from server to client
	 */
	class Test3:public CppUnit::TestCase
	{

	};

public:
    OsiTransportTest();

public slots:

	// from server slots
	void slotClientConnected(const CConnection* that);
	void slotClientDisconnected(const CConnection* that);
	void slotServerRcvTSdu(const CConnection* that);
	void slotServerCRReady(const CConnection* that);
	void slotServerIOError(const CConnection* that);

	// from client slots
	void slotConnectionReady(const CConnection* that);
	void slotConnectionClosed(const CConnection* that);
	void slotTSReady(const CConnection* that);
	void slotCRReady(const CConnection* that);
	void slotIOError(const CConnection* that);

};

#endif // OSITRANSPORT_H
