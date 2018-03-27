#include "httpconnection.h"
#include <QDateTime>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTextStream>

HttpConnection::HttpConnection(QObject *_parent) :
    NetworkConnection(_parent),
    m_transmissions(0)
{
    connect(&m_socket, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(&m_socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(&m_socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
}

void HttpConnection::startConnection(const QString &_address, int _port)
{
    m_address = _address;
    m_port = _port;

    emit connecting();

    m_socket.connectToHost(m_address, m_port);
}

Transmission* HttpConnection::sendData(const QByteArray &_data)
{
    Transmission *transmission = new Transmission();
    transmission->data = _data;
    transmission->payloadSize_MB = _data.size() / powf(10,6);
    m_transmissions << transmission;

    if(m_transmissions.size()==1 && m_state==CONNECTED)
    {
        transmit(transmission);
    }
    else
    {
    }


    return transmission;
}

void HttpConnection::transmit(Transmission *_transmission)
{
    HttpRequest request = HttpRequest::build("POST", _transmission->data);
    _transmission->sendTime_ms = getTime();

    m_socket.write(request.serialize());
}

void HttpConnection::transmitNext()
{
    if(m_transmissions.size()>0 && m_state==CONNECTED)
        transmit(m_transmissions.at(0));
}

qint64 HttpConnection::getTime()
{
    return QDateTime::currentMSecsSinceEpoch();
}

void HttpConnection::onConnected()
{
    qDebug() << "TCP::onConnected" << QDateTime::currentMSecsSinceEpoch();

    emit connected();

    // transmit buffered data
    transmitNext();

}

void HttpConnection::onDisconnected()
{
    qDebug() << "onDisconnected" << QDateTime::currentMSecsSinceEpoch();

    emit disconnected();

}

void HttpConnection::onReadyRead()
{
    qDebug() << "TCP::onReadyRead" << QDateTime::currentMSecsSinceEpoch();

    qint64 time_ms = getTime();
    QString result = "";

    QStringList items = QString::fromUtf8(m_socket.readAll()).split("\r\n");
    if(items.size()>0)
        result = items.last();

    qDebug() << "HttpConnection::onReadyRead" << result;

    //if(result=="OK")

    // this should be equal to status code 200

   // if(result.contains("Hello"))
    {
        Transmission *transmission = m_transmissions.at(0);
        m_transmissions.removeFirst();
        transmission->ackTime_ms = time_ms;
        transmission->deltaTime_ms = time_ms - transmission->sendTime_ms;
        transmission->throughput_mbps = transmission->payloadSize_MB * 8.0 / (transmission->deltaTime_ms/1000.0);

        //qDebug() << "state" << m_state;
        //qDebug() << transmission->payloadSize_MB << "MB" << "-" << transmission->deltaTime_ms << "ms" << "-" << transmission->throughput_mbps << "MBit/s";

        emit transmitted(transmission);
        transmitNext();
    }
    /*else
    {
        Transmission *transmission = m_transmissions.at(0);
        m_transmissions.removeFirst();
        emit transmissionFailed(transmission);
    }*/
}
