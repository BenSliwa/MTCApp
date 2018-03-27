#ifndef GPSBASED_H
#define GPSBASED_H

#include "mobilityprediction.h"

class GpsBased : public MobilityPrediction
{
    Q_OBJECT
public:
    GpsBased(QObject *_parent = 0);
    MobilityEntry predict(float _width_s);

    Position predictPosition(const Position &_position, float _speed_mps, float _direction, float _width_s);



};

#endif // GPSBASED_H
