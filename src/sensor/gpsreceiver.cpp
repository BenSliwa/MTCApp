#include "gpsreceiver.h"
#include "math.h"

GpsReceiver::GpsReceiver(QObject *_parent) : QObject(_parent),
    p_source(0),
    m_latitude(0),
    m_longitude(0),
    m_altitude(0),
    m_speed_mps(0),
    m_direction(0)
{
    // fake a gps position on desktop OS
    #ifdef Q_OS_ANDROID
        p_source = QGeoPositionInfoSource::createDefaultSource(this);

        connect(p_source, SIGNAL(positionUpdated(QGeoPositionInfo)), this, SLOT(onPositionUpdated(QGeoPositionInfo)));
        connect(p_source, SIGNAL(error(QGeoPositionInfoSource::Error)), this, SLOT(onError(QGeoPositionInfoSource::Error)));

        p_source->setUpdateInterval(1000);
        p_source->startUpdates();
    #else
        m_latitude = 51.4903;
        m_longitude = 7.41291;
        m_speed_mps = 50.0 / 3.6;
    #endif
}

Position GpsReceiver::getPosition()
{
    return Position(m_longitude, m_latitude, m_altitude);
}

double GpsReceiver::getLatitude()
{
    return m_latitude;
}

double GpsReceiver::getLongitude()
{
    return m_longitude;
}

double GpsReceiver::getAltitude()
{
    return m_altitude;
}

double GpsReceiver::getVelocity()
{
    return m_speed_mps;
}

double GpsReceiver::getDirection()
{
    return m_direction;
}

double GpsReceiver::getDistance(const Position &_from, const Position &_to)
{
    double distance_m = 0;
    if(!(_from.x==_to.x && _from.y==_to.y))
    {

        double lat0 = toRad(_from.y);
        double lon0 = toRad(_from.x);
        double lat1 = toRad(_to.y);
        double lon1 = toRad(_to.x);

        double sinTerm = sin(lat0)*sin(lat1);
        double cosTerm = cos(lat0)*cos(lat1)*cos(lon1-lon0);

        distance_m = acos(sinTerm+cosTerm)*40000/360*1000;    // map to earth
        distance_m = toGrad(distance_m);
    }

    return distance_m;
}

Position GpsReceiver::computeDestination(const Position &_from, double _direction, double _distance_m)
{
    //qDebug() << "GpsReceiver::computeDestination" << _from.toString() << _direction << _distance_m;

    double direction = _direction;
    double d = _distance_m/1000.0;
    double R = 6371;
    double h = toRad(direction);
    double lat = toRad(_from.y);
    double lon = toRad(_from.x);
    double latitude = asin(sin(lat)*cos(d/R) + cos(lat)*sin(d/R)*cos(h));
    double longitude = lon + atan2(sin(h)*sin(d/R)*cos(lat), cos(d/R)-sin(lat)*sin(latitude));

    Position position = Position(toGrad(longitude), toGrad(latitude));
    return position;
}

double GpsReceiver::toRad(double _value)
{
    return _value * 3.14159265359 / 180;
}

double GpsReceiver::toGrad(double _value)
{
    return _value * 180 / 3.14159265359;
}

void GpsReceiver::onPositionUpdated(QGeoPositionInfo _info)
{
    m_latitude = _info.coordinate().latitude();
    m_longitude = _info.coordinate().longitude();
    m_altitude = _info.coordinate().altitude();

    if(_info.hasAttribute(QGeoPositionInfo::GroundSpeed))
        m_speed_mps = _info.attribute(QGeoPositionInfo::GroundSpeed);

    if(_info.hasAttribute(QGeoPositionInfo::Direction))
        m_direction = _info.attribute(QGeoPositionInfo::Direction);
}

void GpsReceiver::onError(QGeoPositionInfoSource::Error _error)
{

}
