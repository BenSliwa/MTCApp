#include "httprequest.h"

HttpRequest::HttpRequest()
{

}

HttpRequest HttpRequest::build(const QString &_type, const QByteArray &_data)
{
    HttpRequest request;

    request.m_type = _type;
    request.m_data = _data;

    return request;
}

HttpRequest HttpRequest::fromByteArray(const QByteArray &_data)
{
    HttpRequest request;

    return request;
}

QByteArray HttpRequest::serialize()
{
    QString message;
    message += "POST / HTTP/1.1\r\n";
    message += "Content-Type: application/x-www-form-urlencoded\r\n";
    message += "Content-Length: " + QString::number(m_data.size()) + "\r\n";
    message += "Connection: Keep-Alive\r\n";
    message += "Host: 127.0.0.1\r\n\r\n";
    message += QString(m_data);

    return message.toUtf8();
}
