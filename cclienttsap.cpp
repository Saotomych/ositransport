#include "cclienttsap.h"

CClientTSAP::CClientTSAP(): m_messageTimeout(0), m_messageFragmentTimeout(60000), m_maxTPDUSizeParam(65531)
{
}

CClientTSAP::CClientTSAP(CSocketFactory& socketFactory):
		m_socketFactory(socketFactory),
		m_messageTimeout(0),
		m_messageFragmentTimeout(60000),
		m_maxTPDUSizeParam(65531)
{
}

void CClientTSAP::setMessageTimeout(int messageTimeout)
{
	m_messageTimeout = messageTimeout;
}

void CClientTSAP::setMessageFragmentTimeout(int messageFragmentTimeout)
{
	m_messageFragmentTimeout = messageFragmentTimeout;
}

void CClientTSAP::setMaxTPDUSizeParam(int maxTPduSizeParam)
{
	if (maxTPduSizeParam < 7 || maxTPduSizeParam > 16) throw new CExIllegalArgument("setMaxTPDUSizeParam: Is out of bound");

}

int CClientTSAP::getMaxTPDUSizeParam()
{
	return m_maxTPDUSizeParam;
}

static int CClientTSAP::getMaxTPDUSize(int maxTPduSizeParam)
{
	if (maxTPduSizeParam < 7 || maxTPduSizeParam > 16) throw new CExIllegalArgument("setMaxTPDUSizeParam: Is out of bound");

	if (maxTPduSizeParam == 16)
		return 65531;
	else
		return pow(2, maxTPduSizeParam);
}

CConnection CClientTSAP::connectTo(QHostAddress address, qint16 port)
{
	QHostAddress lA((quint32) 0);
	return connectTo(address, port, lA, -1);
}

CConnection CClientTSAP::connectTo(QHostAddress address, qint16 port, QHostAddress localAddr, qint16 localPort)
{
	CTcpEasySocket socket;
	if (localAddr.isNull())
		m_socketFactory.CreateSocket(address, port);
	else
		m_socketFactory.CreateSocket(address, port, localAddr, localPort);

	CConnection connection = new CConnection(socket, m_maxTPduSizeParam, m_messageTimeout, m_messageFragmentTimeout, nullptr);
	connection.tSelRemote = tSelRemote;
	connection.tSelLocal = tSelLocal;
	connection.startConnection();

	return connection;
}

void CClientTSAP::setSocketFactory(CSocketFactory& socketFactory)
{
	m_socketFactory = socketFactory;
}
