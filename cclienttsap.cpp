#include "cclienttsap.h"
#include "cconnection.h"

CClientTSAP::CClientTSAP():
		m_pSocketFactory(nullptr),
		m_messageTimeout(0),
		m_messageFragmentTimeout(60000),
		m_maxTPDUSizeParam(65531)
{
}

CClientTSAP::CClientTSAP(CSocketFactory& socketFactory):
		m_pSocketFactory(&socketFactory),
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
	if (maxTPduSizeParam < 7 || maxTPduSizeParam > 16) { emit signalIllegalArgument("CClientTSAP::setMaxTPDUSizeParam: parameter Is out of bound"); return; }

	m_maxTPDUSizeParam = maxTPduSizeParam;
}

int CClientTSAP::getMaxTPDUSizeParam()
{
	return m_maxTPDUSizeParam;
}

int CClientTSAP::getMaxTPDUSize(int maxTPDUSizeParam)
{
	if (maxTPDUSizeParam < 7 || maxTPDUSizeParam > 16) {
		std::invalid_argument("CClientTSAP::getMaxTPDUSize: maxTPDUSizeParam is wrong.");
	}

	if (maxTPDUSizeParam == 16)
		return 65531;
	else
		return pow(2, maxTPDUSizeParam);
}

void CClientTSAP::connectTo(QHostAddress address, quint16 port)
{
	QHostAddress lA((quint32) 0);
	connectTo(address, port, lA, -1);
}

void CClientTSAP::connectTo(QHostAddress address, quint16 port, QHostAddress localAddr, quint16 localPort)
{
	if (m_pSocketFactory == nullptr)
	{
		qDebug() << "Pointer to Socket Factory is NULL";
		emit signalIllegalClassMember("CClientTSAP::connectTo: m_pSocketFactory is NULL!");
		return;
	}

	CTcpEasySocket socket;
	if (localAddr.isNull())
		socket = m_pSocketFactory->CreateSocket(address, port);
	else
		socket = m_pSocketFactory->CreateSocket(address, port, localAddr, localPort);

	CConnection connection(&socket, m_maxTPDUSizeParam, m_messageTimeout, m_messageFragmentTimeout);
	connection.setSelRemote(m_tSelRemote);
	connection.setSelLocal(m_tSelLocal);
	connection.startConnection();

	emit signalConnectionReady(connection);
}

void CClientTSAP::setSocketFactory(CSocketFactory& socketFactory)
{
	m_pSocketFactory = &socketFactory;
}
