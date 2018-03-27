#ifndef MOBILITYPREDICTION_H
#define MOBILITYPREDICTION_H

#include <QObject>
#include <QDebug>
#include <QTimer>
#include "mobilityentry.h"
#include "sensor/gpsreceiver.h"

class MobilityPrediction : public QObject
{
    Q_OBJECT
public:
    MobilityPrediction(QString _type, QObject *_parent = 0);

    void start(int _width_s, const QString &_fileName);
    void stop();

    float getTimestamp();
    QString getType();

    virtual MobilityEntry predict(float _width_s) = 0;

private slots:
    void onTimeout();

protected:
    GpsReceiver *p_gps;

    QString m_type;
    QTimer m_timer;
    QString m_fileName;

    int m_width_s;


    // TODO: location module

};

#endif // MOBILITYPREDICTION_H
