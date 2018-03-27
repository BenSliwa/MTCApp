#include "networkconnection.h"

NetworkConnection::NetworkConnection(QObject *_parent) :
    QObject(_parent),
    m_address(""),
    m_port(-1),
    m_state(DISCONNTECTED)
{
    connect(this, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(this, SIGNAL(connecting()), this, SLOT(onConnecting()));
    connect(this, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
}

QString NetworkConnection::getAddress()
{
    return m_address;
}

int NetworkConnection::getPort()
{
    return m_port;
}

void NetworkConnection::onConnected()
{
    m_state = CONNECTED;
}

void NetworkConnection::onConnecting()
{
    m_state = CONNECTING;
}

void NetworkConnection::onDisconnected()
{
    m_state = DISCONNTECTED;
}
