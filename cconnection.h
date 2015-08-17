#ifndef CCONNECTION_H
#define CCONNECTION_H

#include "ositransport_global.h"
#include "tcpeasysocket.h"
#include "serverthread.h"

class OSITRANSPORTSHARED_EXPORT CConnection
{
private:

	const quint8 c_CRCDT;
	const quint8 c_CCCDT;

	CTcpEasySocket* m_pSocket;
	static qint32 s_connectionCounter;
	static QMutex s_mutexConCounter;

	QVector<char> m_tSelRemote;
	QVector<char> m_tSelLocal;

	qint32 m_srcRef;
	qint32 m_dstRef;
	qint32 m_maxTPDUSizeParam;
	qint32 m_maxTPDUSize;
	qint32 m_messageTimeout;
	qint32 m_messageFragmentTimeout;
	bool m_closed;
	CServerThread* m_pServerThread;

	QScopedPointer<QDataStream> m_pOs;
	QScopedPointer<QDataStream> m_pIs;

	explicit CConnection();
	CConnection& operator=(const CConnection& that);
	CConnection(const CConnection& that);

	quint32 readRFC1006Header(quint16& packetLength);
	quint32 readRFC1006CR(QVector<char>& tSel1, QVector<char>& tSel2, quint32 lengthIndicator, qint8 cdtCode);
	quint32 readUserDataBlock(QVector<char>& tSel);

	quint32 writeRFC1006Header();
	quint32 writeRFC1006Header(quint32 size);
	quint32 write8073Header(quint8 lastCode);
	quint32 writeRFC1006CR(QVector<char>& tSel1, QVector<char>& tSel2, qint8 cdtCode);
	quint32 writeBuffer(QVector<char> buffer, quint32 offset, quint32 len);

public:

	CConnection(CTcpEasySocket* socket, quint32 maxTPduSizeParam, qint32 m_messageTimeout,
				qint32 m_messageFragmentTimeout, CServerThread* pServerThread);

	void setSelRemote(QVector<char>& tSelRemote);

	void setSelLocal(QVector<char>& tSelLocal);

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
	void send(QLinkedList<QVector<char> > tsdus, QLinkedList<quint32> offsets, QLinkedList<quint32> lengths);

	void send(QVector<char> tsdu, quint32 offset, quint32 length);

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
	void setMessageFragmentTimeout(int messageFragmentTimeout);

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
	void receive(QByteArray& tSduBuffer);

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
