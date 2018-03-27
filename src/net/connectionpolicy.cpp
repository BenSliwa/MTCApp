#include "connectionpolicy.h"

ConnectionPolicy::ConnectionPolicy(NetworkConnection *_connection, QObject *_parent) :
    QObject(_parent),
    p_connection(_connection)
{
    connect(&m_connectionTimer, SIGNAL(timeout()), this, SLOT(onConnectionTimeout()));
    connect(p_connection, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(p_connection, SIGNAL(connecting()), this, SLOT(onConnecting()));
    connect(p_connection, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
}

void ConnectionPolicy::onConnected()
{
    m_connectionTimer.stop();
}

void ConnectionPolicy::onConnecting()
{
    m_connectionTimer.start(10*1000);
}

void ConnectionPolicy::onDisconnected()
{
    p_connection->startConnection(p_connection->getAddress(), p_connection->getPort());
}

void ConnectionPolicy::onConnectionTimeout()
{
    onDisconnected();
}
