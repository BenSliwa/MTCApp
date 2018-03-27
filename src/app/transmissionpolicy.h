#ifndef TRANSMISSIONPOLICY_H
#define TRANSMISSIONPOLICY_H

#include <iostream>
#include <QVariantMap>

class TransmissionPolicy
{
public:
    TransmissionPolicy();

    virtual bool transmit(float _timeDelta_s, const QVariantMap &_context) = 0;

    bool transmitWithProbability(float _probability);
};

#endif // TRANSMISSIONPOLICY_H
