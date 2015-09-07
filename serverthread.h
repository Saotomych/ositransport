#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include "ositransport_global.h"
#include "cconnection.h"
#include "cconnectionlistener.h"

class CServerThread: public QObject
{

	Q_OBJECT

private:

	static quint32 s_socketCounter;

	CTcpEasySocket* m_pTcpSocket;
	quint32 m_maxTPduSizeParam;
	quint32 m_messageTimeout;
	quint32 m_messageFragmentTimeout;
	quint32 m_maxConnections;

	CConnectionListener* m_pConnListener;

	QThreadPool m_srvThreadPool;

	class CConnectionThread: public QRunnable
	{
		CConnection* m_pConnection;
		explicit CConnectionThread(): m_pConnection(nullptr) {}
	public:
		CConnectionThread(CConnection* pconn): m_pConnection(pconn) {}

		void run();
	};

	explicit CServerThread();

public:

	Q_DISABLE_COPY(CServerThread);

	CServerThread(
			CTcpEasySocket* sock,
			quint32 maxTPduSizeParam,
			quint32 msgTimeout,
			quint32 msgFragmentTimeout,
			quint32 maxConnections,
			CConnectionListener* listener);

	CConnection* createNewConnection();
	void startConnectionThread(CConnection* pconn);

    quint32 getMaxConnections();
    void setMaxConnections(quint32 maxConnections);
    bool addThread2ServerPool(QRunnable* obj);

    void startServer();
    void stopServer();

private slots:
	void slotConnectionClosed(const CConnection* that);
	void slotConnectionReady(const CConnection* that);
	void slotTryNewConnection(const CConnection* that);

signals:
	void signalUserConnected();
	void signalUserDisconnected();

};

#endif // SERVERTHREAD_H
