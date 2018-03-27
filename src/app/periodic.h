#ifndef PERIODIC_H
#define PERIODIC_H

#include "transmissionpolicy.h"

class Periodic : public TransmissionPolicy
{
public:
    Periodic();

    bool transmit(float _timeDelta_s, const QVariantMap &_context);
};

#endif // PERIODIC_H
