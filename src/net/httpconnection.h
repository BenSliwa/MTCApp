#ifndef HTTPCONNECTION_H
#define HTTPCONNECTION_H

#include "networkconnection.h"
#include <QTcpSocket>
#include <QMap>
#include "httprequest.h"
#include "httpreply.h"
#include "transmission.h"


class HttpConnection : public NetworkConnection
{
    Q_OBJECT
public:
    HttpConnection(QObject *_parent = 0);

    void startConnection(const QString &_address, int _port);
    Transmission* sendData(const QByteArray &_data);
    void transmit(Transmission *_transmission);
    void transmitNext();

    qint64 getTime();


private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();

signals:
    void transmitted(Transmission *_transmission);
    void transmissionFailed(Transmission *_transmission);

private:
    QTcpSocket m_socket;
    QVector<Transmission*> m_transmissions;
};

#endif // HTTPCONNECTION_H
