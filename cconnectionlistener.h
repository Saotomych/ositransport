#ifndef CCONNECTIONLISTENER_H
#define CCONNECTIONLISTENER_H

#include "ositransport_global.h"
#include "cconnection.h"

class OSITRANSPORTSHARED_EXPORT CConnectionListener : public QObject
{

	Q_OBJECT

public:
    explicit CConnectionListener(QObject *parent = 0);

signals:
	void signalConnected();
	void signalDisconnected();
	void signalTSduReady();
	void signalCRReady();

public slots:
	void slotDisconnect();

private slots:
	void slotCreated(const CConnection* that);
	void slotConnected(const CConnection* that);
	void slotClosed(const CConnection* that);
	void slotConnectionLost(const CConnection* that);

};

#endif // CCONNECTIONLISTENER_H
