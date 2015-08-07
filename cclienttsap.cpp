#include "cclienttsap.h"

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
	if (maxTPduSizeParam < 7 || maxTPduSizeParam > 16) throw new OSIExceptions::CExIllegalArgument("setMaxTPDUSizeParam: Is out of bound");

}

int CClientTSAP::getMaxTPDUSizeParam()
{
	return m_maxTPDUSizeParam;
}

int CClientTSAP::getMaxTPDUSize(int maxTPDUSizeParam)
{
	if (maxTPDUSizeParam < 7 || maxTPDUSizeParam > 16) throw new OSIExceptions::CExIllegalArgument("setMaxTPDUSizeParam: Is out of bound");

	if (maxTPDUSizeParam == 16)
		return 65531;
	else
		return pow(2, maxTPDUSizeParam);
}

QSharedPointer<CConnection> CClientTSAP::connectTo(QHostAddress address, quint16 port)
{
	QHostAddress lA((quint32) 0);
	return connectTo(address, port, lA, -1);
}

QSharedPointer<CConnection> CClientTSAP::connectTo(QHostAddress address, quint16 port, QHostAddress localAddr, quint16 localPort)
{
	if (m_pSocketFactory == nullptr)
	{
		qDebug() << "Pointer to Socket Factory is NULL";
		throw OSIExceptions::CExIllegalClassMember("m_pSocketFactory is NULL!");
	}

	CTcpEasySocket socket;
	if (localAddr.isNull())
		socket = m_pSocketFactory->CreateSocket(address, port);
	else
		socket = m_pSocketFactory->CreateSocket(address, port, localAddr, localPort);

	QSharedPointer<CConnection> pconnection(new CConnection(&socket, m_maxTPDUSizeParam, m_messageTimeout, m_messageFragmentTimeout, nullptr));
	pconnection->setSelRemote(m_tSelRemote);
	pconnection->setSelLocal(m_tSelLocal);
	pconnection->startConnection();

	return pconnection;
}

void CClientTSAP::setSocketFactory(CSocketFactory& socketFactory)
{
	m_pSocketFactory = &socketFactory;
}
