#ifndef GPSRECEIVER_H
#define GPSRECEIVER_H

#include <QObject>
#include <QDebug>
#include <QtPositioning/QGeoPositionInfoSource>
#include "position.h"

class GpsReceiver : public QObject
{
    Q_OBJECT
public:
    GpsReceiver(QObject *_parent = 0);

    Position getPosition();
    double getLatitude();
    double getLongitude();
    double getAltitude();
    double getVelocity();
    double getDirection();

    static double getDistance(const Position &_from, const Position &_to);
    static Position computeDestination(const Position &_from, double _direction, double _distance_m);
    static double toRad(double _value);
    static double toGrad(double _value);

private slots:
    void onPositionUpdated(QGeoPositionInfo _info);
    void onError(QGeoPositionInfoSource::Error _error);

private:
    QGeoPositionInfoSource *p_source;

    double m_latitude;
    double m_longitude;
    double m_altitude;
    double m_speed_mps;
    double m_direction;
};

#endif // GPSRECEIVER_H
