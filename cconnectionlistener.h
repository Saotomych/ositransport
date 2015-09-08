#ifndef CCONNECTIONLISTENER_H
#define CCONNECTIONLISTENER_H

#include "ositransport_global.h"
#include "cconnection.h"

class OSITRANSPORTSHARED_EXPORT CConnectionListener : public QObject
{

	Q_OBJECT

public:
    explicit CConnectionListener(QObject *parent = 0);

    void userConnected(const CConnection* that);

signals:
	void signalUserConnected(const CConnection* that);
	void signalUserDisconnected(const CConnection* that);
	void signalTSduReady(const CConnection* that);
	void signalCRReady(const CConnection* that);
	void signalIOError(const CConnection* that);

private slots:
	void slotConnectionReady(const CConnection* that);
	void slotConnectionClosed(const CConnection* that);
	void slotTSduReady(const CConnection* that);
	void slotCRReady(const CConnection* that);
	void slotIOError(QString strErr);

};

#endif // CCONNECTIONLISTENER_H
