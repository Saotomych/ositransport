#include "cconnection.h"
#include "exceptions.h"
#include "cclienttsap.h"

qint32 CConnection::s_connectionCounter = 0;

CConnection::CConnection(CTcpEasySocket* socket,
						quint32 maxTPduSizeParam,
						qint32 messageTimeout,
						qint32 messageFragmentTimeout,
						CServerThread* pServerThread):
m_pSocket(socket),
m_srcRef(0),
m_dstRef(0),
m_maxTPDUSizeParam(maxTPduSizeParam),
m_maxTPDUSize(0),
m_messageTimeout(messageTimeout),
m_messageFragmentTimeout(messageFragmentTimeout),
m_closed(true),
m_pServerThread(pServerThread),
c_CRCDT(0xe0),
c_CCCDT(0xd0)
{
	QScopedPointer<QDataStream> os(new QDataStream(socket));
	m_pOs.swap(os);

	QScopedPointer<QDataStream> is(new QDataStream(socket));
	m_pIs.swap(is);

	m_maxTPDUSize = CClientTSAP::getMaxTPDUSize(m_maxTPDUSizeParam);

}

void CConnection::setSelRemote(QVector<char>& tSelRemote)
{
	m_tSelRemote = tSelRemote;
}

void CConnection::setSelLocal(QVector<char>& tSelLocal)
{
	m_tSelLocal = tSelLocal;
}

quint32 CConnection::readRFC1006Header()
{
	quint8 data8;
	quint16 data16;
	quint32 variableLength=3;

	*m_pIs >> data8;
	if (data8 != 0x03) throw OSIExceptions::CExIOError("Error: RFC 1006 version failed!");

	*m_pIs >> data8;
	if (data8 != 0x00) throw OSIExceptions::CExIOError("Error: RFC 1006 reserved byte failed!");

	// read packet length but is not needed
	*m_pIs >> data16;

	*m_pIs >> data8;
	quint8 lengthIndicator = data8;

	return lengthIndicator;

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

quint32 CConnection::readRFC1006CR(QVector<char>& tSel1, QVector<char>& tSel2, quint32 lengthIndicator, qint8 cdtCode)
{
	qint8 data8;
	qint16 data16;

	// 0xe0 - CR code
	*m_pIs >> data8;
	if (data8 != cdtCode)  throw OSIExceptions::CExIOError("Error: RFC 1006 CR-code failed!");

	// DST-REF needs to be 0 in a CR packet
	*m_pIs >> data16;
	if (data16 != 0)  throw OSIExceptions::CExIOError("Error: RFC 1006 DST-REF in CR failed!");

	// SRC REF which is the dstRef fot this endpoint
	*m_pIs >> data16;
	m_dstRef = data16;

	// Read class
	*m_pIs >> data8;
	if (data8 != 0)  throw OSIExceptions::CExIOError("Error: Read class failed!");

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

quint32 CConnection::writeRFC1006Header()
{
	quint16 data16;
	quint32 variableLength=3;

	*m_pOs << (char)0x03 << (char)0x00;

	if (!m_tSelLocal.isEmpty()) variableLength += 2 + m_tSelLocal.size();
	if (!m_tSelRemote.isEmpty()) variableLength += 2 + m_tSelRemote.size();

	data16 = 4 + 7 +  variableLength;
	*m_pOs << data16;

	return variableLength;
}

quint32 CConnection::writeRFC1006Header(quint32 size)
{
	quint16 data16;
	quint32 variableLength=3;

	*m_pOs << (char)0x03 << (char)0x00;

	if (size) variableLength += 2 + size;

	data16 = 2 +  variableLength;
	*m_pOs << data16;

	return variableLength;
}

quint32 CConnection::write8073Header(quint8 lastCode)
{
	*m_pOs << (char)0x02 << (char)0xf0 << lastCode;

	return 3;
}

quint32 CConnection::writeRFC1006CR(QVector<char>& tSel1, QVector<char>& tSel2, qint8 cdtCode)
{
	quint16 data16;
	quint32 variableLength=0;
	quint8 data8;

	// write length indicator
	data8 = 6 +  variableLength;
	*m_pOs << data16;

	// write fixed part
	// CC CDT
	*m_pOs << cdtCode;

	data16 = m_dstRef;
	*m_pOs << data16;
	data16 = m_srcRef;
	*m_pOs << data16;
	*m_pOs << (char) 0;

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

	return variableLength;
}

quint32 CConnection::writeBuffer(QVector<char> buffer, quint32 offset, quint32 len)
{

}

void CConnection::listenForCR()
{

	quint8 data8;
	quint16 data16;

	m_pSocket->setMessageFragmentTimeout(m_messageFragmentTimeout);

	// start reading rfc 1006 header hardcode
	quint8 lengthIndicator = readRFC1006Header();

	quint32 variableBytesRead = readRFC1006CR(m_tSelLocal, m_tSelRemote, lengthIndicator, c_CRCDT);


	// write RFC 1006 header
	quint32 variableLength = writeRFC1006Header();

	// write connection request  CR
	variableLength += writeRFC1006CR(m_tSelLocal, m_tSelRemote, c_CCCDT);

}

void CConnection::startConnection()
{
	quint16 data16;
	quint8 data8;

	// RFC 1006 Header
	qint32 variableLength = writeRFC1006Header();

	// Connection request (CR)
	variableLength += writeRFC1006CR(m_tSelRemote, m_tSelLocal, c_CRCDT);

	m_pSocket->setMessageTimeout(m_messageTimeout);

	// start reading rfc 1006 header hardcode
	quint8 lengthIndicator = readRFC1006Header();

	quint32 variableBytesRead = readRFC1006CR(m_tSelRemote, m_tSelLocal, lengthIndicator, c_CCCDT);

}

void CConnection::send(QLinkedList<QVector<char> > tsdus, QLinkedList<quint32> offsets, QLinkedList<quint32> lengths)
{
	quint32 bytesLeft;
	for (quint32 length: lengths) bytesLeft+=length;

	quint32 tsduOffset = 0;
	quint32 numBytesToWrite;
	quint32 maxTSDUSize = m_maxTPDUSize - 3;

	while (bytesLeft)
	{
		if (bytesLeft > maxTSDUSize)
		{
			writeRFC1006Header(maxTSDUSize);
			numBytesToWrite = maxTSDUSize;
			write8073Header(0x00);
		}
		else
		{
			writeRFC1006Header(bytesLeft);
			numBytesToWrite = bytesLeft;
			write8073Header(0x80);
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

}

void CConnection::setMessageTimeout(int messageTimeout)
{

}

quint32 CConnection::getMessageFragmentTimeout()
{

}

void CConnection::setMessageFragmentTimeout(int messageFragmentTimeout) {
	m_messageFragmentTimeout = messageFragmentTimeout;
}

void CConnection::receive(QVector<quint8> tSduBuffer)
{

}

void CConnection::disconnect()
{

}

void CConnection::close()
{

}

