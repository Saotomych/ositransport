#include "ositransport.h"

const char OsiTransportTest::testData[] = {
						(char) 0x00, (char) 0x01, (char) 0x02, (char) 0x03, (char) 0x04, (char) 0x05, (char) 0x06,
						(char) 0x07, (char) 0x08, (char) 0x09, (char) 0x0a, (char) 0x0b, (char) 0x0c, (char) 0x0d, (char) 0x0e,
						(char) 0x0f, (char) 0x00, (char) 0x01, (char) 0x02, (char) 0x03, (char) 0x04, (char) 0x05, (char) 0x06,
						(char) 0x07, (char) 0x08, (char) 0x09, (char) 0x0a, (char) 0x0b, (char) 0x0c, (char) 0x0d, (char) 0x0e,
						(char) 0x0f, (char) 0x00, (char) 0x01, (char) 0x02, (char) 0x03, (char) 0x04, (char) 0x05, (char) 0x06,
						(char) 0x07, (char) 0x08, (char) 0x09, (char) 0x0a, (char) 0x0b, (char) 0x0c, (char) 0x0d, (char) 0x0e,
						(char) 0x0f, (char) 0x00, (char) 0x01, (char) 0x02, (char) 0x03, (char) 0x04, (char) 0x05, (char) 0x06,
						(char) 0x07, (char) 0x08, (char) 0x09, (char) 0x0a, (char) 0x0b, (char) 0x0c, (char) 0x0d, (char) 0x0e,
						(char) 0x0f, (char) 0x00, (char) 0x01, (char) 0x02, (char) 0x03, (char) 0x04, (char) 0x05, (char) 0x06,
						(char) 0x07, (char) 0x08, (char) 0x09, (char) 0x0a, (char) 0x0b, (char) 0x0c, (char) 0x0d, (char) 0x0e,
						(char) 0x0f, (char) 0x00, (char) 0x01, (char) 0x02, (char) 0x03, (char) 0x04, (char) 0x05, (char) 0x06,
						(char) 0x07, (char) 0x08, (char) 0x09, (char) 0x0a, (char) 0x0b, (char) 0x0c, (char) 0x0d, (char) 0x0e,
						(char) 0x0f, (char) 0x00, (char) 0x01, (char) 0x02, (char) 0x03, (char) 0x04, (char) 0x05, (char) 0x06,
						(char) 0x07, (char) 0x08, (char) 0x09, (char) 0x0a, (char) 0x0b, (char) 0x0c, (char) 0x0d, (char) 0x0e,
						(char) 0x0f, (char) 0x00, (char) 0x01, (char) 0x02, (char) 0x03, (char) 0x04, (char) 0x05, (char) 0x06,
						(char) 0x07, (char) 0x08, (char) 0x09, (char) 0x0a, (char) 0x0b, (char) 0x0c, (char) 0x0d, (char) 0x0e,
						(char) 0x0f, (char) 0x00, (char) 0x01, (char) 0x02, (char) 0x03, (char) 0x04, (char) 0x05, (char) 0x06,
						(char) 0x07, (char) 0x08, (char) 0x09, (char) 0x0a, (char) 0x0b, (char) 0x0c, (char) 0x0d, (char) 0x0e,
						(char) 0x0f, (char) 0x00, (char) 0x01, (char) 0x02, (char) 0x03, (char) 0x04, (char) 0x05, (char) 0x06,
						(char) 0x07, (char) 0x08, (char) 0x09, (char) 0x0a, (char) 0x0b, (char) 0x0c, (char) 0x0d, (char) 0x0e,
						(char) 0x0f, (char) 0x00, (char) 0x01, (char) 0x02, (char) 0x03, (char) 0x04, (char) 0x05, (char) 0x06,
						(char) 0x07, (char) 0x08, (char) 0x09, (char) 0x0a, (char) 0x0b, (char) 0x0c, (char) 0x0d, (char) 0x0e,
						(char) 0x0f, (char) 0x00, (char) 0x01, (char) 0x02, (char) 0x03, (char) 0x04, (char) 0x05, (char) 0x06,
						(char) 0x07, (char) 0x08, (char) 0x09, (char) 0x0a, (char) 0x0b, (char) 0x0c, (char) 0x0d, (char) 0x0e,
						(char) 0x0f, (char) 0x00, (char) 0x01, (char) 0x02, (char) 0x03, (char) 0x04, (char) 0x05, (char) 0x06,
						(char) 0x07, (char) 0x08, (char) 0x09, (char) 0x0a, (char) 0x0b, (char) 0x0c, (char) 0x0d, (char) 0x0e,
						(char) 0x0f, (char) 0x00, (char) 0x01, (char) 0x02, (char) 0x03, (char) 0x04, (char) 0x05, (char) 0x06,
						(char) 0x07, (char) 0x08, (char) 0x09, (char) 0x0a, (char) 0x0b, (char) 0x0c, (char) 0x0d, (char) 0x0e,
						(char) 0x0f, (char) 0x00, (char) 0x01, (char) 0x02, (char) 0x03, (char) 0x04, (char) 0x05, (char) 0x06,
						(char) 0x07, (char) 0x08, (char) 0x09, (char) 0x0a, (char) 0x0b, (char) 0x0c, (char) 0x0d, (char) 0x0e,
						(char) 0x0f, (char) 0x00, (char) 0x01, (char) 0x02, (char) 0x03, (char) 0x04, (char) 0x05, (char) 0x06,
						(char) 0x07, (char) 0x08, (char) 0x09, (char) 0x0a, (char) 0x0b, (char) 0x0c, (char) 0x0d, (char) 0x0e,
						(char) 0x0f, (char) 0x00, (char) 0x01, (char) 0x02, (char) 0x03, (char) 0x04, (char) 0x05, (char) 0x06,
						(char) 0x07, (char) 0x08, (char) 0x09, (char) 0x0a, (char) 0x0b, (char) 0x0c, (char) 0x0d, (char) 0x0e,
						(char) 0x0f, (char) 0x00, (char) 0x01, (char) 0x02, (char) 0x03, (char) 0x04, (char) 0x05, (char) 0x06,
						(char) 0x07, (char) 0x08, (char) 0x09, (char) 0x0a, (char) 0x0b, (char) 0x0c, (char) 0x0d, (char) 0x0e,
						(char) 0x0f
};

void OsiTransportTest::Test1::prepareTest()
{
	OsiTransportTest* pTest = OsiTransportTest::getMainTest();

	pTest->checkClientConnected = false;
	pTest->checkClientErrorConnected = false;
	pTest->checkServerConnected = false;
	pTest->checkServerErrorConnected = false;
	pTest->checkIllegalArg = false;
	pTest->checkIllegalClassMbr = false;

	// Create server and start listening
	qint32 port = 18982;
	pTest->pServer = new CServerTSAP(port);
	pTest->pConnectionListener = pTest->pServer->createServer();

	// server slots
	pTest->connect(pTest->pConnectionListener, SIGNAL(signalConnected), pTest, SLOT(slotClientConnected()));
	pTest->connect(pTest->pConnectionListener, SIGNAL(signalDisconnected), pTest, SLOT(slotClientDisconnected()));
	pTest->connect(pTest->pConnectionListener, SIGNAL(signalTSduReady), pTest, SLOT(slotServerRcvTSdu()));
	pTest->connect(pTest->pConnectionListener, SIGNAL(signalCRReady), pTest, SLOT(slotServerCRReady()));
	pTest->connect(pTest->pConnectionListener, SIGNAL(signalIOError), pTest, SLOT(slotServerIOError()));

	pTest->pServer->startListening();

	// Create client and start connection
	pTest->pClient = new CClientTSAP(*CSocketFactory::getSocketFactory());

	// client's error slots
	pTest->connect(pTest->pClient, SIGNAL(signalIllegalArgument), pTest, SLOT(slotIllegalArgument));
	pTest->connect(pTest->pClient, SIGNAL(signalConnectError), pTest, SLOT(slotConnectError));
	pTest->connect(pTest->pClient, SIGNAL(signalIllegalClassMember), pTest, SLOT(slotIllegalClassMember));

	pTest->pClient->setMaxTPDUSizeParam(7);

	QHostAddress address("127.0.0.1");
	pTest->pConnection = pTest->pClient->connectTo(address, port);

	// connection slots
	pTest->connect(pTest->pConnection, SIGNAL(signalConnectionReady), pTest, SLOT(slotConnectionReady));
	pTest->connect(pTest->pConnection, SIGNAL(signalConnectionClosed), pTest, SLOT(slotConnectionClosed));
	pTest->connect(pTest->pConnection, SIGNAL(signalTSduReady), pTest, SLOT(slotTSduReady));
	pTest->connect(pTest->pConnection, SIGNAL(signalCRReady), pTest, SLOT(slotCRReady));
	pTest->connect(pTest->pConnection, SIGNAL(signalIOError), pTest, SLOT(slotIOError));

	pTest->pConnection->startConnection();
}

void OsiTransportTest::Test1::runTest()
{
	OsiTransportTest* pTest = OsiTransportTest::getMainTest();

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Test1: checkClientConnected wrong", true, pTest->checkClientConnected);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Test1: checkClientErrorConnected wrong", false, pTest->checkClientErrorConnected);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Test1: checkServerConnected wrong", true, pTest->checkServerConnected);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Test1: checkServerErrorConnected wrong", false, pTest->checkServerErrorConnected);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Test1: checkIllegalArg wrong", false, pTest->checkIllegalArg);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Test1: checkIllegalClassMbr wrong", false, pTest->checkIllegalClassMbr);
}

void OsiTransportTest::Test2::runTest()
{
	OsiTransportTest* pTest = OsiTransportTest::getMainTest();

}

void OsiTransportTest::Test3::runTest()
{
	OsiTransportTest* pTest = OsiTransportTest::getMainTest();

}

// server slots
void OsiTransportTest::slotClientConnected(const CConnection* that)
{
	checkServerConnected = true;
}

void OsiTransportTest::slotClientDisconnected(const CConnection* that)
{
	checkServerConnected = false;
}

void OsiTransportTest::slotServerRcvTSdu(const CConnection* that)
{

}

void OsiTransportTest::slotServerCRReady(const CConnection* that)
{

}

void OsiTransportTest::slotServerIOError(QString str)
{
	checkClientErrorConnected = true;
}

// client slots
void OsiTransportTest::slotConnectionReady(const CConnection* that)
{

	checkClientConnected = true;

}

void OsiTransportTest::slotConnectionClosed(const CConnection* that)
{
	checkClientConnected = false;
}

void OsiTransportTest::slotTSReady(const CConnection* that)
{

}

void OsiTransportTest::slotCRReady(const CConnection* that)
{

}

void OsiTransportTest::slotIOError(QString str)
{
	checkClientErrorConnected = true;
}

// Client Errors
void OsiTransportTest::slotIllegalArgument(QString strErr)
{
	checkIllegalArg = true;
}

void OsiTransportTest::slotConnectError(QString strErr)
{
	checkClientErrorConnected = true;
}

void OsiTransportTest::slotIllegalClassMember(QString strErr)
{
	checkIllegalClassMbr = true;
}

int main()
{
	OsiTransportTest* ptest = OsiTransportTest::getMainTest();

	OsiTransportTest::Test1* test1 = new OsiTransportTest::Test1("Connection test");
	test1->prepareTest();

	CppUnit::TextTestRunner runner;
	runner.addTest(test1);
	runner.run();

	std::ofstream outFile("testResult.xml");
	CppUnit::XmlOutputter outputer(&runner.result(), outFile);
	outputer.write();

	return 0;
}
