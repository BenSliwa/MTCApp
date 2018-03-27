#include "json.h"

JSON::JSON(QObject *_parent) : QObject(_parent)
{

}

QVariantMap JSON::parse(const QByteArray &_data)
{
    QJsonDocument document = QJsonDocument::fromJson(_data);
    QJsonObject object = document.object();

    return object.toVariantMap();
}

QByteArray JSON::serialize(const QVariantMap &_data)
{
    QJsonObject object = QJsonObject::fromVariantMap(_data);
    QJsonDocument document;
    document.setObject(object);

    return document.toJson();
}
