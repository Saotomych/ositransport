#include "cconnection.h"

CConnection::CConnection(CTcpEasySocket, quint32 maxTPduSizeParam, qint32 messagetimeout,
						qint32 m_messageTimeout, CServerThread serverThread):
m_socket;
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


{
}
