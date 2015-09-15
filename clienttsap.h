#ifndef CCLIENTTSAP_H
#define CCLIENTTSAP_H

#include "ositransport_global.h"
#include "socketfactory.h"
#include "connection.h"
#include <cmath>
#include <stdexcept>

class OSITRANSPORTSHARED_EXPORT CClientTSAP: public QObject
{
	Q_OBJECT

private:
	CSocketFactory* m_pSocketFactory;
	quint32 m_messageTimeout;
	quint32 m_messageFragmentTimeout;
	quint32 m_maxTPDUSizeParam;

	QVector<char> m_tSelRemote;
	QVector<char> m_tSelLocal;

    CClientTSAP();

public:

    /*
	 * This constructor to create a client TSAP that will start connections to remote TSAPs.
	 * TODO: SSLSocketFactory to enable SSL.
	 *
	 * @param socketFactory
	 *            the socket factory to create the underlying socket
     */
    CClientTSAP(CSocketFactory& socketFactory);

	/**
	 * Set the TConnection timeout for waiting for the first byte of a new message. Default is 0 (unlimited)
	 *
	 * @param messageTimeoutmessageFragmentTimeout
	 *            in milliseconds
	 */
    void setMessageTimeout(int messageTimeout);

	/**
	 * Set the TConnection timeout for receiving data once the beginning of a message has been received. Default is
	 * 60000 (60seconds)
	 *
	 * @param messageFragmentTimeout
	 *            in milliseconds
	 */
	void setMessageFragmentTimeout(int messageFragmentTimeout);

	/**
	 * Set the maxTPDUSize. The default maxTPduSize is 65531 (see RFC 1006).
	 * Needs to be set before listening or connecting.
	 *
	 * @param maxTPduSizeParam
	 *            The maximum length is equal to 2^(maxTPduSizeParam) octets. Note that the actual TSDU size that can be
	 *            transfered is equal to TPduSize-3. Default is 65531 octets (see RFC 1006),
	 *            7 >= maxTPduSizeParam <= 16.
	 * @throws CExIllegalArgument
	 *             is argument illegal
	 */
	void setMaxTPDUSizeParam(int maxTPduSizeParam);

	/**m_maxTPduSizeParam
	 * Get the maximum TPDU size parameter to be used by this TSAP
	 *
	 * @return the maximum TPDU size parameter
	 */
	int getMaxTPDUSizeParam();

	/**
	 * Calculates and returns the maximum TPDUSize. This is equal to 2^(maxTPDUSizeParam)
	 *
	 * @param maxTPDUSizeParam
	 *            the size parameter
	 * @return the maximum TPDU size
	 * @throws CExIllegalArgument as std::exception
	 *             is argument illegal
	 */
	static int getMaxTPDUSize(int maxTPDUSizeParam);

	/**
	 * Connect to a remote TSAP that is listening at the destination address.
	 *
	 * @param address
	 *            remote IP
	 * @param port
	 *            remote port
	 * @return the CConnection Object with signalConnectionReady
	 */
	CConnection* createConnection(QHostAddress address, quint16 port);

	/**
	 * Connect to a remote TSAP that is listening at the destination address.
	 *
	 * @param address
	 *            remote IP
	 * @param port
	 *            remote port
	 * @param localAddr
	 *            local IP
	 * @param localPort
	 *            local port
	 * @return the CConnection Object with signalConnectionReady
	 */
	CConnection* createConnection(QHostAddress address, quint16 port, QHostAddress localAddr, quint16 localPort);

	/**
	 * Set created socketFactory to Client
	 *
	 * @param socketFactory
	 * 				created socket factory
	 */
	void setSocketFactory(CSocketFactory& socketFactory);

signals:

	// Error signals
	void signalIllegalArgument(QString strErr);
	void signalConnectError(QString strErr);
	void signalIllegalClassMember(QString strErr);


};

#endif // CCLIENTTSAP_H
