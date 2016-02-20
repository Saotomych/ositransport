#include "connection.h"

#include "clienttsap.h"	// include for getMaxPDUSize // TODO fix this back link

qint32 CConnection::s_connectionCounter = 0;

QMutex CConnection::s_mutexConCounter;

CConnection::CConnection(CTcpEasySocket* socket,
						quint32 maxTPduSizeParam,
						qint32 messageTimeout,
						qint32 messageFragmentTimeout):
c_CRCDT(0xe0),
c_CCCDT(0xd0),
c_maxTSDUBufferSize(65535),
m_pSocket(socket),
m_srcRef(0),
m_dstRef(0),
m_maxTPDUSizeParam(maxTPduSizeParam),
m_maxTPDUSize(0),
m_messageTimeout(messageTimeout),
m_messageFragmentTimeout(messageFragmentTimeout),
m_closed(true)
{
	QScopedPointer<QDataStream> os(new QDataStream(m_pSocket->getSocket()));
	m_pOs.swap(os);

	QScopedPointer<QDataStream> is(new QDataStream(m_pSocket->getSocket()));
	m_pIs.swap(is);

	m_maxTPDUSize = CClientTSAP::getMaxTPDUSize(m_maxTPDUSizeParam);

	s_mutexConCounter.lock();
	c_connectionNum = s_connectionCounter;
	s_connectionCounter++;
	s_mutexConCounter.unlock();

}

CConnection::~CConnection()
{
	// Pointers class members created no class
	// so that destructor and copy constructor are public

	delete m_pSocket;

}

void CConnection::setSelRemote(QByteArray& tSelRemote)
{
	m_tSelRemote = tSelRemote;
}

void CConnection::setSelLocal(QByteArray& tSelLocal)
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

CConnection::TRFC905ServiceHeader CConnection::readRFC905ServiceHeader(quint8 cdtCode, quint8 readOption)
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

quint32 CConnection::readUserDataBlock(QByteArray& tSel)
{
	quint8 data8;
	*m_pIs >> data8;
	qint32 parameterLength = data8;
	if (tSel.isEmpty())
	{
		tSel.resize(parameterLength);
		m_pIs->readRawData(tSel.data(), parameterLength);
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

quint32 CConnection::readRFC905VariablePart(quint32 lengthIndicator, QByteArray& tSel1, QByteArray& tSel2)
{
	quint8 data8;

	quint32 variableBytesRead = 0;
	while (lengthIndicator > (7 + variableBytesRead))
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

	*m_pOs << (quint8)0x03 << (quint8)0x00;

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

	*m_pOs << (quint8)0x03 << (quint8)0x00;

	if (size) variableLength += 2 + size;

	data16 = 2 +  variableLength;
	*m_pOs << data16;

	return variableLength;
}

quint32 CConnection::writeRFC905DataHeader(quint8 opCode)
{
	*m_pOs << (quint8)0x02 << (quint8)0xf0 << opCode;

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
	*m_pOs << (quint8) 0;

	return serviceHdrSize;
}

quint32 CConnection::writeRFC905Service(QByteArray& tSel1, QByteArray& tSel2)
{
	// write variable part
	if (!tSel1.isEmpty())
	{
		*m_pOs << (quint8) 0xc2;
		*m_pOs << tSel1.size();
		*m_pOs << tSel1;
	}

	if (!tSel2.isEmpty())
	{
		*m_pOs << (quint8) 0xc1;
		*m_pOs << tSel2.size();
		*m_pOs << tSel2;
	}

	// write proposed max TPDU size
	*m_pOs << (quint8) 0xC0;
	*m_pOs << (quint8) 1;
	*m_pOs << (quint8) m_maxTPDUSizeParam;

	return 0;
}

// Emit for Errors occurs into private functions
void CConnection::listenForCR()
{

	if (m_pSocket->getSocket()->waitForReadyRead(m_messageFragmentTimeout) == false)
	{
		emit signalIOError("CConnection::listenForCR: waiting of data timed is out");
		return;
	}

	// Read Connect Request (CR)
	quint16 dataLenght = readRFC1006Header();
	if (!dataLenght) return;

	TRFC905ServiceHeader hdr = readRFC905ServiceHeader(c_CRCDT, 0);
	if (!hdr.lengthIndicator) return;
	m_dstRef = hdr.srcRef;

	if (!readRFC905VariablePart(dataLenght-4, m_tSelLocal, m_tSelRemote))
	{
		emit signalIOError("CConnection::listenForCR: readRFC905VariablePart has unknown error.");
		return;
	}

	// Write Connection Confirm (CC)
	writeRFC1006ServiceHeader(3);
	writeRFC905ServiceHeader(c_CCCDT);
	writeRFC905Service(m_tSelLocal, m_tSelRemote);

	m_pSocket->getSocket()->flush();
	m_pSocket->getSocket()->waitForBytesWritten(m_messageTimeout);

	connect(m_pSocket->getSocket(), SIGNAL(readyRead()), this, SLOT(slotReadyRead()));

	emit signalCRReady(this);
}

// Emit for Errors occurs into private functions
void CConnection::startConnection()
{
	if (m_pSocket->getSocket()->state() != QAbstractSocket::ConnectedState)
	{
		QString str = QString("Socket not connected to %1:%2")
				.arg(m_pSocket->getHost().toString())
				.arg(m_pSocket->getPort());

		emit signalIOError(str);

		return;
	}

	// Send connection request (CR)
	writeRFC1006ServiceHeader(3);
	writeRFC905ServiceHeader(c_CRCDT);
	writeRFC905Service(m_tSelRemote, m_tSelLocal);

	m_pSocket->getSocket()->flush();
	m_pSocket->getSocket()->waitForBytesWritten(m_messageTimeout);

	if (m_pSocket->getSocket()->waitForReadyRead(m_messageTimeout) == true)
	{
		// Read connection confirm (CC)
		quint16 dataLenght = readRFC1006Header();
		if (!dataLenght) return;

		TRFC905ServiceHeader shdr = readRFC905ServiceHeader(c_CCCDT, 0);
		if (!shdr.lengthIndicator) return;

		m_dstRef = shdr.srcRef;

		if (!readRFC905VariablePart(dataLenght-4, m_tSelRemote, m_tSelLocal)) return;

		connect(m_pSocket->getSocket(), SIGNAL(readyRead()), this, SLOT(slotReadyRead()));

		emit signalConnectionReady(this);

		connect(m_pSocket->getSocket(), SIGNAL(readyRead()), this, SLOT(slotReadyRead()));

	}
	else
	{
		emit signalIOError("CConnection::startConnection: don't answer for startConnection");
	}

}

// Emit for Errors occurs into private functions
void CConnection::send(QLinkedList<QByteArray >& tsdus, QLinkedList<quint32>& offsets, QLinkedList<quint32>& lengths)
{

	m_pSocket->getSocket()->waitForBytesWritten(m_messageTimeout);

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

			QByteArray& tsdu = *it_tsdu;
			quint32 len = *it_len;
			quint32 offset = *it_offset;

			quint32 tsduWrLen = len - tsduOffset;

			if (numBytesToWrite > tsduWrLen)
			{
				m_pOs->writeRawData(&tsdu.data()[tsduOffset+offset], tsduWrLen);
				numBytesToWrite -= tsduWrLen;
				tsduOffset = 0;

				it_tsdu++;
				it_len++;
				it_offset++;
			}
			else
			{
				m_pOs->writeRawData(&tsdu.data()[tsduOffset+offset], numBytesToWrite);
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

	m_pSocket->getSocket()->flush();

}

// Emit for Errors occurs into private functions
void CConnection::send(QByteArray& tsdu, quint32 offset, quint32 length)
{
	QLinkedList<QByteArray > tsdus;
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
bool CConnection::receive(QByteArray& tSduBuffer)
{

	quint16 packetLength = 0;
	quint16 fullTSduLength = 0;
	quint8 eot = 0;
	qint8 reason = 0;
	quint16 data16 = 0;

	TRFC905DataHeader hdr;

	if (m_pSocket->getSocket()->bytesAvailable() == 0) return false;

	do
	{
		packetLength = readRFC1006Header();
		if (packetLength <= 7) { emit signalIOError("Syntax error: receive packet length parameter < 7."); return false; }

		quint16 tsduLength = packetLength-7;

		hdr = readRFC905DataHeader();
		if (!hdr.lengthIndicator) return false;

		eot = 0;
		if (hdr.pduCode == 0xF0)
		{
			*m_pIs >> eot;
			if ( eot != 0 && eot != 0x80 ) { emit signalIOError("Syntax Error: EOT wrong"); return false; }

			if ( (qint64)(tsduLength) > c_maxTSDUBufferSize )
			{
				emit signalIOError("tSduBuffer size is too small to hold the complete TSDU");
				return false;
			}

			tSduBuffer += m_pSocket->getSocket()->read(tsduLength);
			fullTSduLength += tsduLength;

		}
		else if (hdr.pduCode == 0x80) // Disconnect Request - DR (ISO RFC-905 ISO DP 8073)
		{
			if (hdr.lengthIndicator != 6) { emit signalIOError("Syntax error: LI field does not equal 6."); return false; }

			*m_pIs >> data16;	// Read DST-REF (Source Reference here)
			if (data16 != m_srcRef) { emit signalIOError("Syntax error: Source reference wrong."); return false; }

			*m_pIs >> data16;	// Read SRC-REF (Destination Reference here)
			if (data16 != m_dstRef) { emit signalIOError("Syntax error: Destination reference wrong."); return false; }

			// check the reason field only between 1 and 4 for class 0
			reason = 0;
			*m_pIs >> reason;
			if (reason < 0 || reason > 4) { emit signalIOError("Syntax error: reason out of bound."); return false; }

			// Disconnect is valid
			emit signalConnectionClosed(this);

			return false;
		}
		else if (hdr.pduCode == 0x70) // Error Message - ER (ISO RFC-905 ISO DP 8073)
		{
			emit signalIOError("Got TPDU Error: ER message.");
			return false;
		}
		else
		{
			emit signalIOError("Syntax Error: unknown TPDU code.");
			return false;
		}

	}while ( eot != 0x80 && m_pSocket->getSocket()->bytesAvailable() != 0);

	if (!eot)
	{
		emit signalIOError("Error: Received last eot error.");
		return false;
	}

	return true;
}

void CConnection::disconnect()
{
	writeRFC1006ServiceHeader(0);
	writeRFC905ServiceHeader(0x80); // Disconnect Request - DR (ISO RFC-905 ISO DP 8073)

	m_pSocket->getSocket()->flush();

	close();
}

void CConnection::close()
{
	if (!m_closed)
	{
		m_closed = true;
		m_pSocket->getSocket()->close();
		m_pIs->setStatus(QDataStream::WriteFailed);

		delete m_pSocket;

		emit signalConnectionClosed(this);
	}
}

void CConnection::slotReadyRead()
{

	emit signalTSduReady(this);
}
