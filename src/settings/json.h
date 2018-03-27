#ifndef JSON_H
#define JSON_H

#include <QObject>
#include <QDebug>
#include <QVariantMap>

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>

class JSON : public QObject
{
    Q_OBJECT
public:
    JSON(QObject *_parent = 0);

    QVariantMap parse(const QByteArray &_data);
    QByteArray serialize(const QVariantMap &_data);
};

#endif // JSON_H
