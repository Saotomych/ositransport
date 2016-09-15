#ifndef CCLIENTTSAP_H
#define CCLIENTTSAP_H

#include "ositransport_global.h"
#include "socketfactory.h"
#include "connection.h"
#include <cmath>
#include <stdexcept>

/**
 * @class CClientTSAP
 * 			That is a client for connecting to the TSAP server
 */

class OSITRANSPORTSHARED_EXPORT CClientTSAP: public QObject
{
	Q_OBJECT

private:
	CSocketFactory* m_pSocketFactory;
	quint32 m_messageTimeout;
	quint32 m_messageFragmentTimeout;
	quint32 m_maxTPDUSizeParam;

	QByteArray m_tSelRemote;
	QByteArray m_tSelLocal;

    CClientTSAP();

public:

    /**
	 * @brief This constructor to create a client TSAP that will start connections to remote TSAPs.
	 * TODO: SSLSocketFactory to enable SSL.
	 *
	 * @param socketFactory
	 *            the socket factory to create the underlying socket
     */
    CClientTSAP(CSocketFactory& socketFactory);

	/**
	 * @brief Set the CConnection timeout for waiting for the first byte of a new message.
	 *
	 * @param messageTimeout. Default is 0 (unlimited)
	 *            in milliseconds
	 */
    void setMessageTimeout(int messageTimeout);

	/**
	 * @brief Set the CConnection timeout for receiving data once the beginning of a message has been received.
	 *
	 * @param messageFragmentTimeout. Default is 60000 (60seconds)
	 *            in milliseconds
	 */
	void setMessageFragmentTimeout(int messageFragmentTimeout);

	/**
	 * @brief Set the maxTPDUSize. The default maxTPduSize is 65531 (see RFC 1006).
	 * Needs to be set before listening or connecting.
	 *
	 * @param maxTPduSizeParam
	 *            The maximum length is equal to 2^(maxTPduSizeParam) octets. Note that the actual TSDU size that can be
	 *            transfered is equal to TPduSize-3. Default is 65531 octets (see RFC 1006),
	 *            7 >= maxTPduSizeParam <= 16.
	 * @return emit signalIllegalArgument("CClientTSAP::setMaxTPDUSizeParam: parameter is out of bound")
	 *             is argument illegal
	 */
	void setMaxTPDUSizeParam(int maxTPduSizeParam);

	/**
	 * @brief Get the maximum TPDU size parameter to be used by this TSAP
	 *
	 * @return the maximum TPDU size parameter
	 */
	int getMaxTPDUSizeParam();

	/**
	 * @brief Connect to a remote TSAP that is listening at the destination address.
	 *
	 * @param address
	 *            remote IP
	 * @param port
	 *            remote port
	 * @return the CConnection Object with emit of the signalConnectionReady
	 * 			in happy case
	 * @return emit signalIllegalClassMember("CClientTSAP::connectTo: m_pSocketFactory is NULL! Set default factory.")
	 * 			in 'socket factory is nothing' case
	 */
	CConnection* createConnection(QHostAddress address, quint16 port);

	/**
	 * @brief Connect to a remote TSAP that is listening at the destination address.
	 * 			emit signalIllegalClassMember("CClientTSAP::connectTo: m_pSocketFactory is NULL! Set default factory.")
	 * 			in case socket factory is nothing
	 *
	 * @param address
	 *            remote IP
	 * @param port
	 *            remote port
	 * @param localAddr
	 *            local IP
	 * @param localPort
	 *            local port
	 * @return the CConnection Object with emit of the signalConnectionReady
	 * 			in happy case
	 * @return emit signalIllegalClassMember("CClientTSAP::connectTo: m_pSocketFactory is NULL! Set default factory.")
	 * 			in 'socket factory is nothing' case
	 */
	CConnection* createConnection(QHostAddress address, quint16 port, QHostAddress localAddr, quint16 localPort);

	/**
	 * @brief Set created socketFactory to Client
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
