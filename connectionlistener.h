#ifndef CCONNECTIONLISTENER_H
#define CCONNECTIONLISTENER_H

#include "ositransport_global.h"
#include "connection.h"

class OSITRANSPORTSHARED_EXPORT CConnectionListener : public QObject
{

	Q_OBJECT

public:
    explicit CConnectionListener(QObject *parent = 0);

signals:
	void signalConnected(const CConnection* that);
	void signalDisconnected(const CConnection* that);
	void signalTSduReady(const CConnection* that);
	void signalCRReady(const CConnection* that);
	void signalIOError(QString str);

private slots:
	void slotClientConnected(const CConnection* that);
	void slotClientDisconnected(const CConnection* that);
	void slotTSduReady(const CConnection* that);
	void slotCRReady(const CConnection* that);
	void slotIOError(QString strErr);

};

#endif // CCONNECTIONLISTENER_H
