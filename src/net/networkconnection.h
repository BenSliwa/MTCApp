#ifndef NETWORKCONNECTION_H
#define NETWORKCONNECTION_H

#include <QObject>
#include <QDebug>

enum
{
    DISCONNTECTED,
    CONNECTING,
    CONNECTED
};

class NetworkConnection : public QObject
{
    Q_OBJECT
public:
    NetworkConnection(QObject *_parent = 0);

    virtual void startConnection(const QString &_address, int _port) = 0;

    QString getAddress();
    int getPort();

protected slots:
    void onConnected();
    void onConnecting();
    void onDisconnected();


signals:
    void connected();
    void connecting();
    void disconnected();


protected:
    QString m_address;
    int m_port;

    int m_state;

};

#endif // NETWORKCONNECTION_H
