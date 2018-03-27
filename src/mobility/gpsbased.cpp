#include "gpsbased.h"
#include "appcontroller.h"


GpsBased::GpsBased(QObject *_parent) :
    MobilityPrediction("gps", _parent)
{

}

MobilityEntry GpsBased::predict(float _width_s)
{
    MobilityEntry entry;
    entry.valid = false;

    Position position = Position(p_gps->getLongitude(), p_gps->getLatitude());
    if(position.x!=0 && position.y!=0)
    {
        entry.valid = true;
        entry.timestamp_s = getTimestamp() + _width_s;
        entry.position = predictPosition(position, p_gps->getVelocity(), p_gps->getDirection(), _width_s);
    }

    return entry;
}

Position GpsBased::predictPosition(const Position &_position, float _speed_mps, float _direction, float _width_s)
{
    // estimate the direction - TODO: extrapolation vs GPS direction
    double direction = _direction;

    // compute the travelled distance
    double distance_m = _speed_mps * _width_s;

    // compute the resulting position
    Position destination = p_gps->computeDestination(_position, direction, distance_m);

    return destination;
}

