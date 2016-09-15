#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include "ositransport_global.h"
#include "connection.h"
#include "connectionlistener.h"
#include "tcpeasysocket.h"

/**
 * @class CConnectionServer is the QTcpServer observer for connection states and notes about new connections and disconnections
 */

class OSITRANSPORTSHARED_EXPORT CConnectionServer: public QObject
{

	Q_OBJECT

private:

	quint32 m_localPort;
	quint32 m_maxTPduSizeParam;
	quint32 m_messageTimeout;
	quint32 m_messageFragmentTimeout;

	CConnectionListener* m_pConnListener;

	explicit CConnectionServer();

	Q_DISABLE_COPY(CConnectionServer);

	QTcpServer* m_pTcpServer;

public: // API

/**
 * @brief CConnectionServer constructor
 * @param parent
 * 			QObject parent
 * @param localPort
 * 			is the localPort where connections are taken
 * @param maxTPduSizeParam
 * 			is the maximum TPDU size parameter to be used by this TSAP
 * @param msgTimeout
 * 			CConnection timeout for waiting for the first byte of a new message
 * @param msgFragmentTimeout
 * 			CConnection timeout for receiving data once the beginning of a message has been received
 * @param maxConnections
 * 			the number of connections allowed
 * @param listener
 * 			listener for the all connection events
 */
	CConnectionServer(
			QObject *parent,
			quint32 localPort,
			quint32 maxTPduSizeParam,
			quint32 msgTimeout,
			quint32 msgFragmentTimeout,
			quint32 maxConnections,
			CConnectionListener* listener);

	virtual ~CConnectionServer();

/**
 * @brief creates new connection and connect him to the slots
 * @param tcpSocket
 * 			new socket for the connection
 * @return new connection pointer
 */
	CConnection* createNewConnection(CTcpEasySocket* tcpSocket);

/**
 * @brief getter for maximum quantity of the user connections
 * @return maximum quantity of the user connections
 */
    quint32 getMaxConnections();

/**
 * @brief setter for maximum quantity of the user connections
 * @param maxConnections
 * 			is the maximum quantity of the user connections
 */
    void setMaxConnections(quint32 maxConnections);

/**
 * @brief server up
 */
    void startServer();

/**
 * @brief server down
 */
    void stopServer();

private slots:
	void slotServerConnectionClosed(const CConnection* that);
	void slotServerAcceptConnection();
	void slotServerError(QAbstractSocket::SocketError socketError);

signals:
	void signalClientConnected(const CConnection* that);
	void signalClientDisconnected(const CConnection* that);

};

#endif // SERVERTHREAD_H
