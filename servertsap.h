#ifndef CSERVERTSAP_H
#define CSERVERTSAP_H

#include "ositransport_global.h"
#include "connectionlistener.h"
#include "socketfactory.h"
#include "connectionserver.h"

/**
 * @class CServerTSAP
 * 			Is the TSAP server for multiuser connections.
 */

class OSITRANSPORTSHARED_EXPORT CServerTSAP
{
	qint32 m_localPort;
	qint32 m_backlog;
	QHostAddress m_bindAddr;
	CSocketFactory* m_pServerSocketFactory;
	CConnectionListener* m_pConListener;
	CConnectionServer* m_pServerThread;

	quint32 m_maxTPduSizeParam;
	quint32 m_maxConnection;
	quint32 m_messageTimeout;
	quint32 m_messageFragmentTimeout;

public:

    /**
	 * @brief This constructor to create a server TSAP that can listen on a port.
	 *
	 * @param port
	 *            the TCP port that the ServerSocket will connect to. Should be
	 *            between 1 and 65535.
	 */
    CServerTSAP(qint32 port);

	/**
	 * @brief This constructor to create a server TSAP that can listen on a port.
	 *
	 * @param port
	 *            the TCP port that the ServerSocket will connect to. Should be
	 *            between 1 and 65535.
	 * @param backlog
	 *            is passed to the java.net.ServerSocket
	 * @param bindAddr
	 *            the IP address to bind to. It is passed to
	 *            java.net.ServerSocket
	 */
    CServerTSAP(qint32 port, qint32 backlog, QHostAddress bindAddr);

	/**
	 * @brief This constructor to create a server TSAP that can listen on a port,
	 * with a specified ServerSocketFactory.
	 *
	 * @param port
	 *            the TCP port that the ServerSocket will connect to. Should be
	 *            between 1 and 65535.
	 * @param backlog
	 *            is passed to the java.net.ServerSocket
	 * @param bindAddr
	 *            the IP address to bind to. It is passed to
	 *            java.net.ServerSocket
	 * @param serverSocketFactory
	 *            The ServerSocketFactory to be used to create the ServerSocket
	 */
    CServerTSAP(qint32 port, qint32 backlog, QHostAddress bindAddr, CSocketFactory* serverSocketFactory);

	/**
	 * @brief Starts a new thread that listens on the configured port. This method is
	 * non-blocking.
	 *			Prerequisite: call createServer and connect to signals from ConnectionListener
	 *
	 * @throws IOException
	 */
    void startListening();

	/**
	 *
	 * @brief Create environment for a new thread that listens on the configured port. This method is
	 * non-blocking.
	 *
	 * @throw std::bad_alloc
	 * 			when 'new CConnectionServer' failed
	 * @return CConnectionListener pointer for connect/disconnect signals receiving
	 */
	CConnectionListener* createServer();

	/**
	 * @brief Stop listing on the port. Stops the server thread.
	 */
	void stopListening();

	/**
	 * @brief Set the maxTPDUSize. The default maxTPDUSize is 65531 (see RFC 1006).
	 * 			Only use this function if you want to change this before server creating.
	 * 			This function doesn't influence for already created server.
	 *
	 * @param maxTPDUSizeParam
	 *          The maximum length is equal to 2^(maxTPDUSizeParam) octets.
	 *          Note that the actual TSDU size that can be transfered is equal
	 *          to TPDUSize-3. Default is 65531 octets (see RFC 1006), 7 <=
	 *          maxTPDUSizeParam <= 16, needs to be set before listening or
	 *          connecting
	 */
	void setMaxTPDUSizeParam(int maxTPDUSizeParam);

	/**
	 * @brief Set the maximum number of connections that are allowed in parallel by the Server SAP.
	 * 			Only use this function if you want to change this before server creating.
	 * 			This function doesn't influence for already created server.
	 *
	 * @param maxConnections
	 *            the number of connections allowed (default is 100)
	 */
	void setMaxConnections(int maxConnections);

	/**
	 * @brief Set the TConnection timeout for waiting for the first byte of a new message.
	 *
	 * @param messageTimeout. Default is 0 (unlimited).
	 *            in milliseconds
	 */
	void setMessageTimeout(int messageTimeout);

	/**
	 * @brief Set the TConnection timeout for receiving data once the beginning of a message has been received.
	 *
	 * @param messageFragmentTimeout. Default is 60000 (60 seconds).
	 *            in milliseconds
	 */
	void setMessageFragmentTimeout(int messageFragmentTimeout);

	/**
	 * @brief Get the maxTPDUSize Parameter to be used by this TSAP
	 */
	quint32 getMaxTPDUSizeParam();
};

#endif // CSERVERTSAP_H
