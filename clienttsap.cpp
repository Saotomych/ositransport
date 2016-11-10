#include "clienttsap.h"

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
	if (maxTPduSizeParam < 7 || maxTPduSizeParam > 16) {

		emit signalIllegalArgument("CClientTSAP::setMaxTPDUSizeParam: parameter is out of bound");

		return;
	}

	m_maxTPDUSizeParam = maxTPduSizeParam;
}

int CClientTSAP::getMaxTPDUSizeParam()
{
	return m_maxTPDUSizeParam;
}

CConnection* CClientTSAP::createConnection(QHostAddress address, quint16 port)
{
	QHostAddress lA(QHostAddress::Null);

	return createConnection(address, port, lA, -1);
}

CConnection* CClientTSAP::createConnection(QHostAddress address, quint16 port, QHostAddress localAddr, quint16 localPort)
{
	if (m_pSocketFactory == nullptr)
	{
		qDebug() << "CClientTSAP::connectTo: m_pSocketFactory is NULL!";
		emit signalIllegalClassMember("CClientTSAP::connectTo: m_pSocketFactory is NULL! Set default factory.");

		m_pSocketFactory = CSocketFactory::getSocketFactory();
	}

	CTcpEasySocket* socket = nullptr;
        qint32 tries = 3;

        while (socket == nullptr && tries)
        {
            qDebug() << "Create socket try " << (4-tries);
            if (localAddr.isNull())
                socket = m_pSocketFactory->createSocket(address, port);
            else
                socket = m_pSocketFactory->createSocket(address, port, localAddr, localPort);

            QThread::msleep(500);
            --tries;
        }

	CConnection* pconnection = new CConnection(socket, m_maxTPDUSizeParam, m_messageTimeout, m_messageFragmentTimeout);
	pconnection->setSelRemote(m_tSelRemote);
	pconnection->setSelLocal(m_tSelLocal);

	return pconnection;
}

void CClientTSAP::setSocketFactory(CSocketFactory& socketFactory)
{
	m_pSocketFactory = &socketFactory;
}
