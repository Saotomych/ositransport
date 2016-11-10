#ifndef CCONNECTION_H
#define CCONNECTION_H

#include "ositransport_global.h"
#include "tcpeasysocket.h"

/**
 * @class CConnection
 * @brief Class supports async read/write to QTcpSocket and wraps message with the RFC1006 & RFC905 headers.
 */

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

public:

	/**
	 * @param socket
	 * 			pointer to Qt Socket, don't have to be nullptr
	 * @param maxTPduSizeParam
	 * 			maximum size of TPDU. Default is 65535.
	 * @param messageTimeout
	 * 			Timeout for receiving of all message
	 * @param messageFragmentTimeout
	 *			Timeout for receiving of fragment
	 * @brief Constructor for CConnection. Connects QTcpSocket to read / written slots.
	 */
	CConnection(CTcpEasySocket* socket, quint32 maxTPduSizeParam, qint32 messageTimeout,
				qint32 messageFragmentTimeout);

	virtual ~CConnection();

	/**
	 * @param tSelRemote
	 * 			transport remote selector
	 * @brief This function sets Transport Remote Selector. It's used in establishing a remote server connection.
	 *
	 * It specifies a byte array, which is expressed as pairs of hexadecimal digits separated by zero or more spaces.
	 * The maximum size is 50 bytes (100 hex chars).
	 * The default setting is "00 01" (unless a value is specified in the SCL file)
	 */
	void setSelRemote(QByteArray& tSelRemote);

	/**
	 * @param tSelLocal
	 * 			transport local selector
	 * @brief This function sets Transport Local Selector. It's used in receiving a server connection from remote.
	 *
	 * It specifies a byte array, which is expressed as pairs of hexadecimal digits separated by zero or more spaces.
	 * The maximum size is 50 bytes (100 hex chars).
	 * The default setting is "00 01" (unless a value is specified in the SCL file)
	 */
	void setSelLocal(QByteArray& tSelLocal);

	/**
	 * @brief This function is called once by server when a client has connected to the server. It parse for a Connection Request (CR).
	 *
	 * If this is successful it replies afterwards with a Connection Confirm (CC). According to the norm a syntax error in
	 * the CR should be followed by an ER. This implementation does not send an ER because it seems unnecessary.
	 *
	 * @return emits signalCRReady(this)
	 */
	void listenForCR();

	/**
	 * @brief Starts a connection, sends a CR, waits for a CC and throws an IOException if not successful
	 */
	void startConnection();

	/**
	 * @brief Send data to connected host
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
	 * @brief getter for message timeout
	 * @return messageTimeout
	 *           in milliseconds
	 */
	quint32 getMessageTimeout();

	/**
	* @brief Set the TConnection timeout for waiting for the first byte of a new message. Default is 0 (unlimited)
	* @param messageTimeout
	*            in milliseconds
	*/
	void setMessageTimeout(int messageTimeout);

	/**
	 * @brief getter for message fragment timeout
	 * @return messageFragmentTimeout
	 *           in milliseconds
	 */
	quint32 getMessageFragmentTimeout();

	/**
	 * @brief Set the TConnection timeout for receiving data once the beginning of a message has been received. Default is
	 * 60000 (60 seconds)
	 *
	 * @param messageFragmentTimeout
	 *           in milliseconds
	 */
	void setMessageFragmentTimeout(int messageFragmentTimeout);

	/**
	 * Listens for a new TPDU and writes the extracted TSDU into the passed buffer.
	 *
	 * @param tSduBuffer
	 *            the buffer that is filled with the received TSDU data.
	 * @return bool
	 * 				is true when TSDU is correct
	 * 				is false when TSDU has errors
	 */
	bool receive(QByteArray& tSduBuffer);

	/**
	 * @brief This function sends a Disconnect Request but does not wait for a Disconnect Confirm.
	 */
	void disconnect();

	/**
	 * @brief Will close the TCP connection if its still open and free any resources of this connection.
	 * @return emits signalConnectionClosed(this)
	 */
	void close();


	/**
	 * @brief This function is called once by client when a client has connected to the server. It parse for a Connection Answer.
	 * @return emits signal signalConnectionReady(this)
	 */
	void parseServerAnswer();

	/**
	 * @brief Calculates and returns the maximum TPDUSize. This is equal to 2^(maxTPDUSizeParam)
	 *
	 * @param maxTPDUSizeParam
	 *            the size parameter
	 * @return the maximum TPDU size
	 * @throws std::invalid_argument("CConnection::getMaxTPDUSize: maxTPDUSizeParam is wrong.")
	 *             is argument illegal
	 */
	static int getMaxTPDUSize(int maxTPDUSizeParam);

        bool isClosed() const;

protected:

	class CTSDUSender
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

		CTSDUSender(QLinkedList<QByteArray>& tsdus, QLinkedList<quint32>& offsets, QLinkedList<quint32>& lengths, qint32 maxTPDUSize):
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

	class CServiceSender
	{
	private:

		friend CConnection;

		quint32 bytesLeft = 0;
		quint32 maxFrameSize;

		quint32 m_beginSize;

		QByteArray m_tSel1,
				   m_tSel2;

		quint8 m_opCode;

		CServiceSender(quint32 beginSize, quint8 opCode, QByteArray& tSel1, QByteArray& tSel2, qint32 FrameSize = 128):
			bytesLeft(14),	// Header size
			maxFrameSize(FrameSize),
			m_beginSize(beginSize),
			m_tSel1(tSel1),
			m_tSel2(tSel2),
			m_opCode(opCode)
		{
		}

		bool sendService(CConnection& Conn);
	};

	friend CTSDUSender;
	friend CServiceSender;

	QScopedPointer<CTSDUSender> TSDUSender;
	QScopedPointer<CServiceSender> ServiceSender;

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
