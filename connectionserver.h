#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include "ositransport_global.h"
#include "connection.h"
#include "connectionlistener.h"
#include "tcpeasysocket.h"

class OSITRANSPORTSHARED_EXPORT CConnectionServer: public QObject
{

	Q_OBJECT

private:

	quint32 m_localPort;
	quint32 m_maxTPduSizeParam;
	quint32 m_messageTimeout;
	quint32 m_messageFragmentTimeout;

	CConnectionListener* m_pConnListener;

	QThreadPool m_srvThreadPool;

	explicit CConnectionServer();

	Q_DISABLE_COPY(CConnectionServer);

	QTcpServer m_TcpServer;

public:

	CConnectionServer(
			quint32 localPort,
			quint32 maxTPduSizeParam,
			quint32 msgTimeout,
			quint32 msgFragmentTimeout,
			quint32 maxConnections,
			CConnectionListener* listener);

	CConnection* createNewConnection(CTcpEasySocket* tcpSocket);

    quint32 getMaxConnections();
    void setMaxConnections(quint32 maxConnections);

public: //API
    void startServer();
    void stopServer();

private slots:
	void slotConnectionClosed(const CConnection* that);
	void slotAcceptConnection();

signals:
	void signalClientConnected(const CConnection* that);
	void signalClientDisconnected(const CConnection* that);

};

#endif // SERVERTHREAD_H
