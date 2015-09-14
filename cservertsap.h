#ifndef CSERVERTSAP_H
#define CSERVERTSAP_H

#include "ositransport_global.h"
#include "cconnectionlistener.h"
#include "socketfactory.h"
#include "serverthread.h"
#include "socketfactory.h"

class OSITRANSPORTSHARED_EXPORT CServerTSAP : public QObject
{
    Q_OBJECT

	qint32 localPort;
	qint32 backlog;
	QHostAddress bindAddr;
	CSocketFactory* pServerSocketFactory;
	CConnectionListener* conListener;
	CServerThread* serverThread;

	quint32 maxTPduSizeParam;
	quint32 maxConnection;
	quint32 messageTimeout;
	quint32 messageFragmentTimeout;

	explicit CServerTSAP() {}

public:

    /**
	 * Use this constructor to create a server TSAP that can listen on a port.
	 *
	 * @param port
	 *            the TCP port that the ServerSocket will connect to. Should be
	 *            between 1 and 65535.
	 */
    CServerTSAP(qint32 port);

	/**
	 * Use this constructor to create a server TSAP that can listen on a port.
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
	 * Use this constructor to create a server TSAP that can listen on a port,
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
	 * Starts a new thread that listens on the configured port. This method is
	 * non-blocking.
	 *
	 * Prerequest: call createServer and connect to signals from ConnectionListener
	 *
	 * @throws IOException
	 */
    void startListening();

	/**
	 * Create environment for a new thread that listens on the configured port. This method is
	 * non-blocking.
	 *
	 * @throws IOException
	 */
	CConnectionListener* createServer();

	/**
	 * Stop listing on the port. Stops the server thread.
	 */
	void stopListening();

	/**
	 * Set the maxTPDUSize. The default maxTPDUSize is 65531 (see RFC 1006).
	 * Only use this function if you want to change this.
	 *
	 * @param maxTPDUSizeParam
	 *            The maximum length is equal to 2^(maxTPDUSizeParam) octets.
	 *            Note that the actual TSDU size that can be transfered is equal
	 *            to TPDUSize-3. Default is 65531 octets (see RFC 1006), 7 <=
	 *            maxTPDUSizeParam <= 16, needs to be set before listening or
	 *            connecting
	 */
	void setMaxTPDUSizeParam(int maxTPDUSizeParam);

	/**
	 * Set the maximum number of connections that are allowed in parallel by the
	 * Server SAP.
	 *
	 * @param maxConnections
	 *            the number of connections allowed (default is 100)
	 */
	void setMaxConnections(int maxConnections);

	/**
	 * Set the TConnection timeout for waiting for the first byte of a new
	 * message. Default is 0 (unlimited)
	 *
	 * @param messageTimeout
	 *            in milliseconds
	 * @throws SocketException
	 */
	void setMessageTimeout(int messageTimeout);

	/**
	 * Set the TConnection timeout for receiving data once the beginning of a
	 * message has been received. Default is 60000 (60 seconds)
	 *
	 * @param messageFragmentTimeout
	 *            in milliseconds
	 * @throws SocketException
	 */
	void setMessageFragmentTimeout(int messageFragmentTimeout);

	/**
	 * Get the maxTPDUSize Parameter to be used by this TSAP
	 */
	quint32 getMaxTPDUSizeParam();

	/**
	 * Get the maximum TPDUSize. This is equal to 2^(maxTPDUSizeParam)
	 *
	 * @param maxTPDUSizeParam
	 * @return the maximum TPDU size
	 * @throws IOException
	 */
	static quint32 getMaxTPDUSize(int maxTPDUSizeParam);


signals:

public slots:

};

#endif // CSERVERTSAP_H
