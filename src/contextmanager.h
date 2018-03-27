#ifndef CONTEXTMANAGER_H
#define CONTEXTMANAGER_H

#include <QObject>
#include <QTimer>

#include "settings/json.h"

class ContextManger : public QObject
{
    Q_OBJECT
public:
    ContextManger(QObject *parent = 0);

    Q_INVOKABLE void updateAndroidNotification();


    QVariantMap getInfo();

private slots:
    void onUpdateTimeout();

signals:
    void notificationChanged();

    void receivedData(const QVariantMap &_info);

private:
    QTimer m_updateTimer;
    QVariantMap m_info;

};

#endif // CONTEXTMANAGER_H
