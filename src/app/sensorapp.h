#ifndef SENSORAPP_H
#define SENSORAPP_H

#include <QObject>
#include <QDebug>
#include <QTimer>
#include <QVariantMap>
#include "net/httpconnection.h"

#include "cat.h"
#include "periodic.h"

struct SensorData
{
    QByteArray data;
    qint64 timestamp_ms;
};

struct TransmissionInfo
{
    QVector<SensorData> data;
    QVariantMap context;
};

struct Statistics
{
    float transmittedData_MB = 0;
    float transmittedTime_s = 0;
    float meanThroughput_mbps = 0;
    float totalThroughput_mbps = 0;
    float throughputPerTransmission_mbps = 0;
    int transmissions = 0;
};

class SensorApp : public QObject
{
    Q_OBJECT
public:
    SensorApp(const QString &_name, qint64 _timeReference_ms, QObject *_parent = 0);

    void setServer(const QString &_address, int _port);
    void startSensor(int _updateInterval_ms, int _dataPerUpdate_byte);
    void startTransmission(int _updateInterval_ms);

    int getRandomInt(int _min, int _max);

    //
    void setTransmissionPolicy(TransmissionPolicy *_transmissionPolicy);
    void setFileName(const QString &_fileName);

    QString getName();
    Statistics getStatistics();

    float predict_goodput(float payload_MB, float snr_dB, float rsrp_dB, float rsrq_dB, float cqi, float speed);

private slots:
    void onSensorTimeout();

    void onTransmissionTimeout();
    void onTransmitted(Transmission *_transmission);
    void onTransmissionFailed(Transmission *_transmission);

signals:
    void transmitted(const QString &_name);

private:
    HttpConnection m_http;
    TransmissionPolicy *m_transmissionPolicy;

    QString m_name;
    QString m_fileName;
    qint64 m_timeReference_ms;

    //
    QTimer m_sensorTimer;
    int m_dataPerUpdate_byte;

    //
    QTimer m_transmissionTimer;
    float m_lastTransmission_s;
    int m_totalTransmissions;
    int m_totalAcks;

    //
    QVector<SensorData> m_dataBuffer;
    QMap<Transmission*, TransmissionInfo> m_transmissions;


    Statistics m_statistics;
};

#endif // SENSORAPP_H
