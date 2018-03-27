#ifndef LTECONTEXT_H
#define LTECONTEXT_H

#include <QString>

class LTEContext
{
public:
    LTEContext(double _snr_dB = 0, double _rsrp_dBm = 0, double _rsrq_dB = 0, double _cqi = 0);

    double getValue(const QString &_key);

public:
    double snr_dB;
    double rsrp_dBm;
    double rsrq_dB;
    double cqi;
};

#endif // LTECONTEXT_H
