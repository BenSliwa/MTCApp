#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <QDebug>
#include <QString>

class HttpRequest
{
public:
    HttpRequest();

    static HttpRequest build(const QString &_type, const QByteArray &_data);
    static HttpRequest fromByteArray(const QByteArray &_data);

    QByteArray serialize();

private:
    QString m_type;
    QByteArray m_data;



};

#endif // HTTPREQUEST_H
