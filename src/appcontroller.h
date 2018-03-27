#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include <QObject>
#include <QDebug>
#include <QTimer>

#include "uimanager.h"

#include "sensor/gpsreceiver.h"
#include "app/sensorapp.h"
#include "mobility/mobilityprediction.h"
#include "mobility/trackbased.h"
#include "mobility/gpsbased.h"
#include "mobility/tracebased.h"
#include "contextmanager.h"

class AppController : public QObject
{
    Q_OBJECT
public:
    AppController(QObject *_parent = 0);

    static AppController* getInstance();
    void init();


    Q_INVOKABLE void start(const QString &_config);
    Q_INVOKABLE void stop();

    void loadSettings(const QString &_config);

    GpsReceiver* getGps();

    ContextManger* getClient();

    qint64 getPassedTime();
    qint64 getMeasurementTime();
    qint64 getStartTime();
    qint64 getTimestamp();

    void initPredictions();
    void logPredictions();
    QString createLogEntry(const Position &_position, bool _valid);

private slots:
    void onTimeout();
    void onDataReception(const QVariantMap &_info);
    void onTransmitted(const QString &_name);

signals:
    void updateInfo(const QString &_data);
    void updateContext(const QVariantMap &_context);
    void appTransmitted(float _time_s, const QString &_name);

private:
    UiManager m_uiManager;
    ContextManger *m_contextManager;
    QTimer m_updateTimer;


    qint64 m_startTime_ms;

    GpsReceiver *m_gps;
    Position m_lastPosition;
    double m_distance_m;
    qint64 m_tripStart_ms;
    double m_tripDuration_s;

    QVector<SensorApp*> m_apps;
    QString m_fileName;

    TrackBased *m_trackBasedPrediction;
    TraceBased *m_traceBasedPrediction;
    GpsBased *m_gpsBasedPrediction;
};

#endif // APPCONTROLLER_H
