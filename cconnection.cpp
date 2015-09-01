#include "cconnection.h"
#include "cclienttsap.h"

qint32 CConnection::s_connectionCounter = 0;

CConnection::CConnection(CTcpEasySocket* socket,
						quint32 maxTPduSizeParam,
						qint32 messageTimeout,
						qint32 messageFragmentTimeout):
c_CRCDT(0xe0),
c_CCCDT(0xd0),
m_pSocket(socket),
m_srcRef(0),
m_dstRef(0),
m_maxTPDUSizeParam(maxTPduSizeParam),
m_maxTPDUSize(0),
m_messageTimeout(messageTimeout),
m_messageFragmentTimeout(messageFragmentTimeout),
m_closed(true)
{
	QScopedPointer<QDataStream> os(new QDataStream(socket));
	m_pOs.swap(os);

	QScopedPointer<QDataStream> is(new QDataStream(socket));
	m_pIs.swap(is);

	m_maxTPDUSize = CClientTSAP::getMaxTPDUSize(m_maxTPDUSizeParam);

	s_mutexConCounter.lock();
	c_connectionNum = s_connectionCounter;
	s_connectionCounter++;
	s_mutexConCounter.unlock();

}

CConnection::CConnection(const CConnection& other):  QObject(), c_CRCDT(0xe0), c_CCCDT(0xd0)
{
	c_connectionNum = other.c_connectionNum;
    m_pSocket = other.m_pSocket;

    m_tSelRemote = other.m_tSelRemote;
	m_tSelLocal = other.m_tSelLocal;

	m_srcRef = other.m_srcRef;
	m_dstRef = other.m_dstRef;
	m_maxTPDUSizeParam = other.m_maxTPDUSizeParam;
	m_maxTPDUSize = other.m_maxTPDUSize;
	m_messageTimeout = other.m_messageTimeout;
	m_messageFragmentTimeout = other.m_messageFragmentTimeout;
	m_closed = other.m_closed;

	QScopedPointer<QDataStream> os(other.m_pOs.data());
	m_pOs.swap(os);

	QScopedPointer<QDataStream> is(other.m_pIs.data());
	m_pIs.swap(is);
}

CConnection::~CConnection()
{
	// Pointers class members created no class
	// so that destructor and copy constructor are public
}

void CConnection::setSelRemote(QVector<char>& tSelRemote)
{
	m_tSelRemote = tSelRemote;
}

void CConnection::setSelLocal(QVector<char>& tSelLocal)
{
	m_tSelLocal = tSelLocal;
}

quint16 CConnection::readRFC1006Header()
{
	quint8 data8 = 0;
	quint16 data16 = 0;

	*m_pIs >> data8;
	if (data8 != 0x03) { emit signalIOError("Error: RFC 1006 version failed!"); return 0; }

	*m_pIs >> data8;
	if (data8 != 0x00) { emit signalIOError("Error: RFC 1006 reserved byte failed!");  return 0; }

	// read packet length
	*m_pIs >> data16;

	return data16;
}

CConnection::TRFC905ServiceHeader CConnection::readRFC905ServiceHeader(qint8 cdtCode, quint8 readOption)
{
	TRFC905ServiceHeader hdr;
	TRFC905ServiceHeader hdrnull;

	*m_pIs >> hdr.lengthIndicator;
	if (hdr.lengthIndicator != 6) { emit signalIOError("Error: RFC 905 length indicator not equal 6!"); return hdrnull; }

	*m_pIs >> hdr.CRCDT;
	if (hdr.CRCDT != cdtCode) { emit signalIOError("Error: RFC 905 CR-code failed!");  return hdrnull; }

	*m_pIs >> hdr.dstRef;
	*m_pIs >> hdr.srcRef;

	// Read class
	*m_pIs >> hdr.option;
	if (hdr.option != readOption) { emit signalIOError("Error: Read option RFC905 failed!"); return hdrnull; }

	return hdr;
}

CConnection::TRFC905DataHeader CConnection::readRFC905DataHeader()
{
	TRFC905DataHeader hdr;
	TRFC905DataHeader hdrnull;

	*m_pIs >> hdr.lengthIndicator;
	if (hdr.lengthIndicator != 2) { emit signalIOError("Error: RFC 905 length indicator not equal 2!"); return hdrnull; }

	*m_pIs >> hdr.pduCode;

	return hdr;
}

quint32 CConnection::readUserDataBlock(QVector<char>& tSel)
{
	quint8 data8;
	*m_pIs >> data8;
	qint32 parameterLength = data8;
	if (tSel.isEmpty())
	{
		tSel.resize(parameterLength);
		m_pIs->readRawData(&tSel[0], parameterLength);
	}
	else
	{
		if (parameterLength != tSel.size()) { emit signalIOError("Error: Local T-Selector is wrong!"); return 0; }

		for (qint32 i = 0; i < parameterLength; ++i)
		{
			*m_pIs >> data8;
			if (data8 != tSel[i]) { emit signalIOError("Error: Local T-Selector is wrong!"); return 0; }
		}

	}

	return parameterLength;
}

quint32 CConnection::readRFC905VariablePart(quint32 lengthIndicator, QVector<char>& tSel1, QVector<char>& tSel2)
{
	quint8 data8;

	quint32 variableBytesRead = 0;
	while (lengthIndicator > (6 + variableBytesRead))
	{
		// read parameter code
		*m_pIs >> data8;
		switch (data8)
		{
		case 0xC2:
			{
				quint32 len = readUserDataBlock(tSel1);
				if (!len) return 0;
				variableBytesRead += 2 + len;
			}
			break;

		case 0xC1:
			{
				quint32 len = readUserDataBlock(tSel2);
				if (!len) return 0;
				variableBytesRead += 2 + len;
			}
			break;
		case 0xC0:
			{
				*m_pIs >> data8;
				if (data8 != 1) { emit signalIOError("Error: CConnection::listenForCR: case 0xC0!"); return 0; }

				*m_pIs >> data8;
				quint8 newMaxTPDUSizeParam = data8;
				if (newMaxTPDUSizeParam < 7 || newMaxTPDUSizeParam > 16)
				{
					emit signalIOError("Error: newMaxTPDUSizeParam is out of bound!");
					return 0;
				}
				else
				{
					if (newMaxTPDUSizeParam < m_maxTPDUSizeParam)
					{
						m_maxTPDUSizeParam = newMaxTPDUSizeParam;
						m_maxTPDUSize = CClientTSAP::getMaxTPDUSize(m_maxTPDUSizeParam);
					}
				}
				variableBytesRead += 3;
			}
			break;

		default:
			emit signalIOError("Error: CConnection::listenForCR: Unknown case!");
			return 0;
		}
	}

	return variableBytesRead;
}

quint32 CConnection::writeRFC1006ServiceHeader(quint32 beginSize)
{
	quint16 data16;
	quint32 variableLength = beginSize;

	*m_pOs << (char)0x03 << (char)0x00;

	if (!m_tSelLocal.isEmpty()) variableLength += 2 + m_tSelLocal.size();
	if (!m_tSelRemote.isEmpty()) variableLength += 2 + m_tSelRemote.size();

	data16 = 4 + 7 +  variableLength;
	*m_pOs << data16;

	return variableLength;
}

quint32 CConnection::writeRFC1006DataHeader(quint32 size)
{
	quint16 data16;
	quint32 variableLength=3;

	*m_pOs << (char)0x03 << (char)0x00;

	if (size) variableLength += 2 + size;

	data16 = 2 +  variableLength;
	*m_pOs << data16;

	return variableLength;
}

quint32 CConnection::writeRFC905DataHeader(quint8 opCode)
{
	*m_pOs << (char)0x02 << (char)0xf0 << opCode;

	return 3;
}

quint32 CConnection::writeRFC905ServiceHeader(quint8 opCode)
{
	quint16 data16;
	quint8 serviceHdrSize = 6;

	// write length indicator
	*m_pOs << serviceHdrSize;

	// write fixed part
	*m_pOs << opCode;

	data16 = m_dstRef;
	*m_pOs << data16;
	data16 = m_srcRef;
	*m_pOs << data16;

	// write reason - 0x00 corresponds to reason not specified
	*m_pOs << (char) 0;

	return serviceHdrSize;
}

quint32 CConnection::writeRFC905Service(QVector<char>& tSel1, QVector<char>& tSel2)
{
	// write variable part
	if (!tSel1.isEmpty())
	{
		*m_pOs << (char) 0xc2;
		*m_pOs << tSel1.size();
		*m_pOs << tSel1;
	}

	if (!tSel2.isEmpty())
	{
		*m_pOs << (char) 0xc1;
		*m_pOs << tSel2.size();
		*m_pOs << tSel2;
	}

	// write proposed max TPDU size
	*m_pOs << (char) 0xC0;
	*m_pOs << (char) 1;
	*m_pOs << m_maxTPDUSizeParam;

	return 0;
}

// Emit for Errors occurs into private functions
void CConnection::listenForCR()
{

	m_pSocket->waitForReadyRead(m_messageFragmentTimeout);

	// Read Connect Request (CR)
	if (!readRFC1006Header()) return;

	TRFC905ServiceHeader hdr = readRFC905ServiceHeader(c_CRCDT, 0);
	if (!hdr.lengthIndicator) return;
	m_dstRef = hdr.srcRef;

	if (!readRFC905VariablePart(hdr.lengthIndicator, m_tSelLocal, m_tSelRemote)) return;

	// Write Connection Confirm (CC)
	writeRFC1006ServiceHeader(3);
	writeRFC905ServiceHeader(c_CCCDT);
	writeRFC905Service(m_tSelLocal, m_tSelRemote);

	m_pSocket->flush();

	emit signalCRReady(this);
}

// Emit for Errors occurs into private functions
void CConnection::startConnection()
{
	if (!m_pSocket->waitForConnected(m_messageTimeout)) return;

	// Send connection request (CR)
	writeRFC1006ServiceHeader(3);
	writeRFC905ServiceHeader(c_CRCDT);
	writeRFC905Service(m_tSelRemote, m_tSelLocal);

	// Read connection confirm (CC)
	if (!readRFC1006Header()) return;
	TRFC905ServiceHeader shdr = readRFC905ServiceHeader(c_CCCDT, 0);
	if (!shdr.lengthIndicator) return;
	m_dstRef = shdr.srcRef;

	if (!readRFC905VariablePart(shdr.lengthIndicator, m_tSelRemote, m_tSelLocal)) return;

	emit signalConnectionReady(this);

}

// Emit for Errors occurs into private functions
void CConnection::send(QLinkedList<QVector<char> > tsdus, QLinkedList<quint32> offsets, QLinkedList<quint32> lengths)
{

	m_pSocket->waitForBytesWritten(m_messageTimeout);

	quint32 bytesLeft = 0;
	for (quint32 length: lengths) bytesLeft+=length;

	quint32 tsduOffset = 0;
	quint32 numBytesToWrite = 0;
	quint32 maxTSDUSize = m_maxTPDUSize - 3;

	while (bytesLeft)
	{
		if (bytesLeft > maxTSDUSize)
		{
			writeRFC1006DataHeader(maxTSDUSize);
			numBytesToWrite = maxTSDUSize;
			writeRFC905DataHeader(0x00);
		}
		else
		{
			writeRFC1006DataHeader(bytesLeft);
			numBytesToWrite = bytesLeft;
			writeRFC905DataHeader(0x80);
		}

		bytesLeft -= numBytesToWrite;

		auto it_tsdu = tsdus.begin();
		auto it_len = lengths.begin();
		auto it_offset = offsets.begin();

		while (numBytesToWrite)
		{

			QVector<char>& tsdu = *it_tsdu;
			quint32 len = *it_len;
			quint32 offset = *it_offset;

			quint32 tsduWrLen = len - tsduOffset;

			if (numBytesToWrite > tsduWrLen)
			{
				m_pOs->writeRawData(&tsdu[tsduOffset+offset], tsduWrLen);
				numBytesToWrite -= tsduWrLen;
				tsduOffset = 0;

				it_tsdu++;
				it_len++;
				it_offset++;
			}
			else
			{
				m_pOs->writeRawData(&tsdu[tsduOffset+offset], numBytesToWrite);
				if (numBytesToWrite == tsduWrLen)
				{
					tsduOffset = 0;

					it_tsdu++;
					it_len++;
					it_offset++;
				}
				else
				{
					tsduOffset += numBytesToWrite;
				}

				numBytesToWrite = 0;
			}
		}

	}

	m_pSocket->flush();

}

// Emit for Errors occurs into private functions
void CConnection::send(QVector<char> tsdu, quint32 offset, quint32 length)
{
	QLinkedList<QVector<char> > tsdus;
	tsdus.push_back(tsdu);

	QLinkedList<quint32> offsets;
	offsets.push_back(offset);

	QLinkedList<quint32> lengths;
	lengths.push_back(length);

	send(tsdus, offsets, lengths);
}

quint32 CConnection::getMessageTimeout()
{
	return m_messageTimeout;
}

void CConnection::setMessageTimeout(int messageTimeout)
{
	m_messageTimeout = messageTimeout;
}

quint32 CConnection::getMessageFragmentTimeout()
{
	return m_messageFragmentTimeout;
}

void CConnection::setMessageFragmentTimeout(int messageFragmentTimeout) {
	m_messageFragmentTimeout = messageFragmentTimeout;
}

// Emit for Errors may occur into private functions
void CConnection::receive(QByteArray& tSduBuffer)
{

	quint16 packetLength = 0;
	quint32 eot = 0;
	qint8 reason = 0;
	quint16 data16 = 0;

	TRFC905DataHeader hdr;

	if (m_pSocket->waitForReadyRead(m_messageTimeout) == false) emit signalIOError("Error: Received no data.");

	do
	{
		packetLength = readRFC1006Header();
		if (packetLength <= 7) { emit signalIOError("Syntax error: receive packet length parameter < 7."); return; }

		hdr = readRFC905DataHeader();
		if (!hdr.lengthIndicator) return;

		eot = 0;
		if (hdr.pduCode == 0xF0)
		{
			*m_pIs >> eot;
			if ( eot != 0 && eot != 0x80 ) { emit signalIOError("Syntax Error: EOT wrong"); return; }

			if ( (quint64)(packetLength - 7) > (tSduBuffer.MaxSize - tSduBuffer.size()))
			{
				emit signalIOError("tSduBuffer size is too small to hold the complete TSDU");
				return;
			}

			tSduBuffer += m_pSocket->readAll();

		}
		else if (hdr.pduCode == 0x80) // Disconnect Request - DR (ISO RFC-905 ISO DP 8073)
		{
			if (hdr.lengthIndicator != 6) { emit signalIOError("Syntax error: LI field does not equal 6."); return; }

			*m_pIs >> data16;	// Read DST-REF (Source Reference here)
			if (data16 != m_srcRef) { emit signalIOError("Syntax error: Source reference wrong."); return; }

			*m_pIs >> data16;	// Read SRC-REF (Destination Reference here)
			if (data16 != m_dstRef) { emit signalIOError("Syntax error: Destination reference wrong."); return; }

			// check the reason field only between 1 and 4 for class 0
			reason = 0;
			*m_pIs >> reason;
			if (reason < 0 || reason > 4) { emit signalIOError("Syntax error: reason out of bound."); return; }

			// Disconnect is valid
			return;
		}
		else if (hdr.pduCode == 0x70) // Error Message - ER (ISO RFC-905 ISO DP 8073)
		{
			emit signalIOError("Got TPDU Error: ER message.");
			return;
		}
		else
		{
			emit signalIOError("Syntax Error: unknown TPDU code.");
			return;
		}

	}while ( eot != 0x80 || (m_pSocket->waitForReadyRead(m_messageFragmentTimeout) == true) );

	if (!eot)  emit signalIOError("Error: Received last eot error.");
	else emit signalTSduReady(this);

}

void CConnection::disconnect()
{
	writeRFC1006ServiceHeader(0);
	writeRFC905ServiceHeader(0x80); // Disconnect Request - DR (ISO RFC-905 ISO DP 8073)

	m_pSocket->flush();

	close();
}

void CConnection::close()
{
	if (!m_closed)
	{
		m_closed = true;
		m_pSocket->close();
		m_pIs->setStatus(QDataStream::WriteFailed);

		emit signalConnectionClosed(this);
	}
}

