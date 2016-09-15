#ifndef CCONNECTIONLISTENER_H
#define CCONNECTIONLISTENER_H

#include "ositransport_global.h"
#include "connection.h"

/**
 * @class CConnectionListener
 * 			That is main interface for server application to the transport level.
 * 			That is a proxyclass from CConnection object to server applications. The class starts after the creating
 * 			certain CConnection for certain user and notes application about new event for the user connections.
 */

class OSITRANSPORTSHARED_EXPORT CConnectionListener : public QObject
{

	Q_OBJECT

public:
    explicit CConnectionListener(QObject *parent = 0);

signals:
/**
 * @brief signalConnected notes about new connected user
 * @param that
 * 			that is new created connection for certain user
 */
	void signalConnected(const CConnection* that);

/**
 * @brief signalDisconnected notes about disconnect of user
 * @param that
 * 			is old connection pointer for note only.
 * 			calling of any functions with the pointer not recommend because the pointer may be invalid at the moment.
 */
	void signalDisconnected(const CConnection* that);

/**
 * @brief signalTSduReady notes about new received data frame
 * @param that
 * 			is actual connection pointer with new data frame
 */
	void signalTSduReady(const CConnection* that);

/**
 * @brief signalCRReady notes that new client connection frame was parsed successfully
 * @param that
 * 			is actual connection pointer with new data frame
 */
	void signalCRReady(const CConnection* that);

/**
 * @brief Error messages from transport level will be here
 * @param str
 * 			is the error text string
 */
	void signalIOError(QString str);

private slots:
/**
 * @brief this slot is connected to CConnectionServer at the moment of CConnectionServer::createNewConnection
 * @param that
 * 			is the new connection
 */
	void slotClientConnected(const CConnection* that);

/**
 * @brief this slot is connected to CConnectionServer at the moment of CConnectionServer::createNewConnection
 * @param that
 * 			is the new connection
 */
	void slotClientDisconnected(const CConnection* that);

/**
 * @brief this slot is connected to CConnection at the moment of CConnectionServer::createNewConnection
 * @param that
 * 			is the new connection
 */
	void slotTSduReady(const CConnection* that);

/**
 * @brief this slot is connected to CConnection at the moment of CConnectionServer::createNewConnection
 * @param that
 * 			is the new connection
 */
	void slotCRReady(const CConnection* that);

/**
 * @brief this slot is connected to CConnection at the moment of CConnectionServer::createNewConnection
 * @param strErr
 * 			is the error text string
 */
	void slotIOError(QString strErr);

};

#endif // CCONNECTIONLISTENER_H
