#ifndef CCONNECTION_H
#define CCONNECTION_H

#include "ositransport_global.h"
#include "tcpeasysocket.h"

class OSITRANSPORTSHARED_EXPORT CConnection: public QObject
{
	Q_OBJECT

private:

	Q_DISABLE_COPY(CConnection)

	const quint8 c_CRCDT;
	const quint8 c_CCCDT;

	const quint32 c_maxTSDUBufferSize;

	quint32 c_connectionNum;

	CTcpEasySocket* m_pSocket;
	static qint32 s_connectionCounter;
	static QMutex s_mutexConCounter;

	QByteArray m_tSelRemote;
	QByteArray m_tSelLocal;

	qint32 m_srcRef;
	qint32 m_dstRef;
	qint32 m_maxTPDUSizeParam;
	qint32 m_maxTPDUSize;
	qint32 m_messageTimeout;
	qint32 m_messageFragmentTimeout;
	bool m_closed;

	QScopedPointer<QDataStream> m_pOs;
	QScopedPointer<QDataStream> m_pIs;

	QByteArray m_tSduBuffer;

	explicit CConnection();

	struct TRFC905ServiceHeader
	{
		TRFC905ServiceHeader():
			lengthIndicator(0),
			CRCDT(0),
			dstRef(0),
			srcRef(0),
			option(0)
		{}
		quint8 lengthIndicator;
		quint8 CRCDT;
		quint16 dstRef;
		quint16 srcRef;
		quint8 option;		// class, reason or other in context
	};

	struct TRFC905DataHeader
	{
		TRFC905DataHeader(): lengthIndicator(0), pduCode(0) {}
		quint8 lengthIndicator;
		quint8 pduCode;
	};

public:

	quint16 readRFC1006Header(QDataStream& iStream);	// Call on start of receiving. Return Packet Length.
	TRFC905ServiceHeader readRFC905ServiceHeader(QDataStream& iStream, quint8 cdtCode, quint8 readClass);	// Return RFC905 header as struct.
	TRFC905DataHeader readRFC905DataHeader(QDataStream& iStream);
	quint32 readRFC905VariablePart(QDataStream& iStream, quint32 lengthIndicator, QByteArray& tSel1, QByteArray& tSel2);
	quint32 readUserDataBlock(QDataStream& iStream, QByteArray& tSel);

	quint32 writeRFC1006ServiceHeader(quint32 beginSize);
	quint32 writeRFC1006DataHeader(quint32 size);
	quint32 writeRFC905DataHeader(quint8 cdtCode);
	quint32 writeRFC905ServiceHeader(quint8 cdtCode);
	quint32 writeRFC905Service(QByteArray& tSel1, QByteArray& tSel2);

	CConnection(CTcpEasySocket* socket, quint32 maxTPduSizeParam, qint32 messageTimeout,
				qint32 messageFragmentTimeout);

//	CConnection(const CConnection& other);

	virtual ~CConnection();

	void setSelRemote(QByteArray& tSelRemote);

	void setSelLocal(QByteArray& tSelLocal);

	void asyncReadWriteInit() const;

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
	quint32 send(QLinkedList<QByteArray>& tsdus, QLinkedList<quint32>& offsets, QLinkedList<quint32>& lengths);

	quint32 send(QByteArray& tsdu, quint32 offset, quint32 length);

	/**
	 * @return messageTimeout
	 */
	quint32 getMessageTimeout();

	/**
	* Set the TConnection timeout for waiting for the first byte of a new message. Default is 0 (unlimited)
	*
	* @param messageTimeout#include "socketfactory.h"
	*
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
	 * @return bool
	 * 				is true when TSDU is correct
	 * 				is false when TSDU has errors
	 */
	bool receive(QByteArray& tSduBuffer);

	/**
	 * This function sends a Disconnect Request but does not wait for a Disconnect Confirm.
	 */
	void disconnect();

	/**
	 * Will close the TCP connection if its still open and free any resources of this connection.
	 */
	void close();

	/*
	 * Wait data appearing into input stream
	 *
	 * @return Reference to input stream smart pointer
	 */
	QScopedPointer<QDataStream>& waitData();
	QScopedPointer<QDataStream>& inputStream() { return m_pIs; }

protected:

	class CSender
	{
	private:

		friend CConnection;

		quint32 bytesLeft = 0;
		quint32 tsduOffset = 0;
		quint32 numBytesToWrite = 0;
		quint32 maxTSDUSize;

		QLinkedList<QByteArray> tsdu;
		QLinkedList<quint32> offset;
		QLinkedList<quint32> len;
		QLinkedList<QByteArray>::iterator it_tsdu;
		QLinkedList<quint32>::iterator it_offset;
		QLinkedList<quint32>::iterator it_len;

		CSender(QLinkedList<QByteArray>& tsdus, QLinkedList<quint32>& offsets, QLinkedList<quint32>& lengths, qint32 maxTPDUSize):
			maxTSDUSize(maxTPDUSize - 3),
			tsdu(tsdus),
			offset(offsets),
			len(lengths)
		{
			bytesLeft = 0;
			for (quint32 length: lengths) bytesLeft+=length;

			it_tsdu = tsdu.begin();
			it_offset = offset.begin();
			it_len = len.begin();
		}

		bool sendNextTSDU(CConnection& Conn);
	};

	friend CSender;

	QScopedPointer<CSender> sender;

public slots:
	void slotReadyRead();
	void slotBytesWritten(qint64 bytes);

signals:

	// work signals
	void signalConnectionReady(const CConnection* that);
	void signalConnectionClosed(const CConnection* that);
	void signalTSduReady(const CConnection* that);
	void signalCRReady(const CConnection* that);
	void signalBytesWritten(qint64);

	// Error signals
	void signalIOError(QString strErr);

};

#endif // CCONNECTION_H
