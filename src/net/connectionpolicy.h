#ifndef CONNECTIONPOLICY_H
#define CONNECTIONPOLICY_H

#include <QObject>
#include <QDebug>
#include <QTimer>
#include "networkconnection.h"

class ConnectionPolicy : public QObject
{
    Q_OBJECT
public:
    ConnectionPolicy(NetworkConnection *_connection, QObject *_parent = 0);

private slots:
    void onConnected();
    void onConnecting();
    void onDisconnected();

    void onConnectionTimeout();

private:
    NetworkConnection *p_connection;
    QTimer m_connectionTimer;

};

#endif // CONNECTIONPOLICY_H
