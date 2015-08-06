#ifndef CCONNECTION_H
#define CCONNECTION_H

#include "ositransport_global.h"
#include "tcpeasysocket.h"

class OSITRANSPORTSHARED_EXPORT CConnection
{
private:
	CTcpEasySocket m_socket;
	CDataOutputStream* m_os;
	CDataInputStream* m_is;

	static qint32 s_connectionCounter = 0;

	QVector<quint8> m_tSelRemote;
	QVector<quint8> m_tSelLocal;

	qint32 m_srcRef;
	qint32 m_dstRef;
	qint32 m_maxTPduSizeParam;
	qint32 m_maxTPduSize;
	qint32 m_messagetimeout;
	qint32 m_messageFragmentTimeout;

	bool m_closed;

	CServerThread serverThread;

	explicit CConnection(){}
	explicit CConnection(CConnection& that){}
	CConnection operator=(CConnection& r){return r;}

public:

	CConnection(CTcpEasySocket socket, quint32 maxTPduSizeParam, qint32 messagetimeout,
				qint32 m_messageTimeout, CServerThread serverThread);

	/**
	 * This function is called once a client has connected to the server. It listens for a Connection Request (CR). If
	 * this is successful it replies afterwards with a Connection Confirm (CC). According to the norm a syntax error in
	 * the CR should be followed by an ER. This implementation does not send an ER because it seems unnecessary.
	 *
	 * @throws CExConnectError
	 */
	void listenForCR();

	/**
	 * Starts a connection, sends a CR, waits for a CC and throws an IOException if not successful
	 *
	 * @throws CExConnectError
	 */
	void startConnection();

	/**
	 * Send data to connected host
	 *
	 * @param tsdus
	 * 			List of buffers to send
	 * @param offsets
	 * 			Offsets in buffers
	 * @param lengths
	 * 			Lengths of buffers
	 */
	void send(QLinkedList<QVector<qint8> > tsdus, QLinkedList<qint32> offsets, QLinkedList<qint32> lengths);
	void send(QVector<qint8> tsdu, qint32 offset, qint32 length);

	/**
	 * @return messageTimeout
	 */
	quint32 getMessageTimeout();

	/**
	* Set the TConnection timeout for waiting for the first byte of a new message. Default is 0 (unlimited)
	*
	* @param messageTimeout
	*            in milliseconds
	*/
	void setMessageTimeout(int messageTimeout);

	/**
	 * @return messageTimeout
	 */
	quint32 getMessageFragmentTimeout();

	/**
	 * Set the TConnection timeout for receiving data once the beginning of a message has been received. Default is
	 * 60000 (60 seconds)
	 *
	 * @param messageFragmentTimeout
	 *            in milliseconds
	 */
	void setMessageFragmentTimeout(int messageFragmentTimeout) {
		m_messageFragmentTimeout = messageFragmentTimeout;
	}

	/**
	 * Listens for a new TPDU and writes the extracted TSDU into the passed buffer.
	 *
	 * @param tSduBuffer
	 *            the buffer that is filled with the received TSDU data.
	 * @throws CexEOF
	 *             if a Disconnect Request (DR) was received or the socket was simply closed
	 * @throws CExSocketTimeout
	 *             if a messageFragmentTimeout is thrown by the socket while receiving the remainder of a message
	 * @throws CExIO
	 *             if an ErrorPDU (ER) was received, any syntax error in the received message header was detected or the
	 *             tSduBuffer is too small to hold the complete PDU.
	 * @throws CExTimeout
	 *             this exception is thrown if the first byte of new message is not received within the message timeout.
	 */
	void receive(QVector<quint8> tSduBuffer);

	/**
	 * This function sends a Disconnect Request but does not wait for a Disconnect Confirm.
	 */
	void disconnect();

	/**
	 * Will close the TCP connection if its still open and free any resources of this connection.
	 */
	void close();

};

#endif // CCONNECTION_H
