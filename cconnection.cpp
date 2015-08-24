#include "cconnection.h"
#include "exceptions.h"
#include "cclienttsap.h"

qint32 CConnection::s_connectionCounter = 0;

CConnection::CConnection(CTcpEasySocket* socket,
						quint32 maxTPduSizeParam,
						qint32 messageTimeout,
						qint32 messageFragmentTimeout,
						CServerThread* pServerThread):
c_CRCDT(0xe0),
c_CCCDT(0xd0),
m_pSocket(socket),
m_srcRef(0),
m_dstRef(0),
m_maxTPDUSizeParam(maxTPduSizeParam),
m_maxTPDUSize(0),
m_messageTimeout(messageTimeout),
m_messageFragmentTimeout(messageFragmentTimeout),
m_closed(true),
m_pServerThread(pServerThread)
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
	if (data8 != 0x03) throw OSIExceptions::CExIOError("Error: RFC 1006 version failed!");

	*m_pIs >> data8;
	if (data8 != 0x00) throw OSIExceptions::CExIOError("Error: RFC 1006 reserved byte failed!");

	// read packet length
	*m_pIs >> data16;

	return data16;
}

CConnection::TRFC905ServiceHeader CConnection::readRFC905ServiceHeader(qint8 cdtCode, quint8 readOption)
{
	TRFC905ServiceHeader hdr;

	*m_pIs >> hdr.lengthIndicator;
	if (hdr.lengthIndicator != 6) throw OSIExceptions::CExIOError("Error: RFC 905 length indicator not equal 6!");

	*m_pIs >> hdr.CRCDT;
	if (hdr.CRCDT != cdtCode)  throw OSIExceptions::CExIOError("Error: RFC 905 CR-code failed!");

	*m_pIs >> hdr.dstRef;
	*m_pIs >> hdr.srcRef;

	// Read class
	*m_pIs >> hdr.option;
	if (hdr.option != readOption)  throw OSIExceptions::CExIOError("Error: Read option RFC905 failed!");

	return hdr;
}

CConnection::TRFC905DataHeader CConnection::readRFC905DataHeader()
{
	TRFC905DataHeader hdr;

	*m_pIs >> hdr.lengthIndicator;
	if (hdr.lengthIndicator != 2) throw OSIExceptions::CExIOError("Error: RFC 905 length indicator not equal 2!");

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
		if (parameterLength != tSel.size()) throw OSIExceptions::CExIOError("Error: Local T-Selector is wrong!");

		for (qint32 i = 0; i < parameterLength; ++i)
		{
			*m_pIs >> data8;
			if (data8 != tSel[i]) throw OSIExceptions::CExIOError("Error: Local T-Selector is wrong!");
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
				variableBytesRead += 2 + readUserDataBlock(tSel1);
			}
			break;

		case 0xC1:
			{
				variableBytesRead += 2 + readUserDataBlock(tSel2);
			}
			break;
		case 0xC0:
			{
				*m_pIs >> data8;
				if (data8 != 1)
					throw OSIExceptions::CExIOError("Error: CConnection::listenForCR: case 0xC0!");

				*m_pIs >> data8;
				quint8 newMaxTPDUSizeParam = data8;
				if (newMaxTPDUSizeParam < 7 || newMaxTPDUSizeParam > 16)
				{
					throw OSIExceptions::CExIOError("Error: newMaxTPDUSizeParam is out of bound!");
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
			throw OSIExceptions::CExIOError("Error: CConnection::listenForCR: Unknown case!");
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

void CConnection::listenForCR()
{

	m_pSocket->waitForReadyRead(m_messageFragmentTimeout);

	// Read Connect Request (CR)
	readRFC1006Header();
	TRFC905ServiceHeader hdr = readRFC905ServiceHeader(c_CRCDT, 0);
	m_dstRef = hdr.srcRef;

	readRFC905VariablePart(hdr.lengthIndicator, m_tSelLocal, m_tSelRemote);

	// Write Connection Confirm (CC)
	writeRFC1006ServiceHeader(3);
	writeRFC905ServiceHeader(c_CCCDT);
	writeRFC905Service(m_tSelLocal, m_tSelRemote);

	m_pSocket->flush();

}

void CConnection::startConnection()
{
	m_pSocket->waitForConnected(m_messageTimeout);

	// Send connection request (CR)
	writeRFC1006ServiceHeader(3);
	writeRFC905ServiceHeader(c_CRCDT);
	writeRFC905Service(m_tSelRemote, m_tSelLocal);

	// Read connection confirm (CC)
	readRFC1006Header();
	TRFC905ServiceHeader shdr = readRFC905ServiceHeader(c_CCCDT, 0);
	m_dstRef = shdr.srcRef;

	readRFC905VariablePart(shdr.lengthIndicator, m_tSelRemote, m_tSelLocal);

}

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

void CConnection::receive(QByteArray& tSduBuffer)
{

	quint16 packetLength = 0;
	quint32 eot = 0;
	qint8 reason = 0;
	quint16 data16 = 0;

	TRFC905DataHeader hdr;

	if (m_pSocket->waitForReadyRead(m_messageTimeout) == false) throw OSIExceptions::CExIOError("Error: Received no data.");

	do
	{
		packetLength = readRFC1006Header();
		if (packetLength <= 7) throw OSIExceptions::CExIOError("Syntax error: packet length parameter < 7.");

		hdr = readRFC905DataHeader();

		eot = 0;
		if (hdr.pduCode == 0xF0)
		{
			*m_pIs >> eot;
			if ( eot != 0 && eot != 0x80 ) throw OSIExceptions::CExIOError("Syntax Error: EOT wrong");

			if ( (quint64)(packetLength - 7) > (tSduBuffer.MaxSize - tSduBuffer.size()))
				throw OSIExceptions::CExIOError("tSduBuffer size is too small to hold the complete TSDU");

			tSduBuffer += m_pSocket->readAll();

		}
		else if (hdr.pduCode == 0x80) // Disconnect Request - DR (ISO RFC-905 ISO DP 8073)
		{
			if (hdr.lengthIndicator != 6) throw OSIExceptions::CExIOError("Syntax error: LI field does not equal 6.");

			*m_pIs >> data16;	// Read DST-REF (Source Reference here)
			if (data16 != m_srcRef) throw OSIExceptions::CExIOError("Syntax error: Source reference wrong.");

			*m_pIs >> data16;	// Read SRC-REF (Destination Reference here)
			if (data16 != m_dstRef) throw OSIExceptions::CExIOError("Syntax error: Destination reference wrong.");

			// check the reason field only between 1 and 4 for class 0
			reason = 0;
			*m_pIs >> reason;
			if (reason < 0 || reason > 4) throw OSIExceptions::CExIOError("Syntax error: reason out of bound.");

			// Disconnect is valid
			return;
		}
		else if (hdr.pduCode == 0x70) // Error Message - ER (ISO RFC-905 ISO DP 8073)
		{
			throw OSIExceptions::CExIOError("Got TPDU Error: ER message.");
		}
		else
		{
			throw OSIExceptions::CExIOError("Syntax Error: unknown TPDU code.");
		}

	}while ( eot != 0x80 || (m_pSocket->waitForReadyRead(m_messageFragmentTimeout) == true) );

	if (!eot)  throw OSIExceptions::CExIOError("Error: Received fragment error.");

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

		emit connectionClosed(this);
	}
}

