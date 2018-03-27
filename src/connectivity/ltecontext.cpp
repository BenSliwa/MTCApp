#include "ltecontext.h"

LTEContext::LTEContext(double _snr_dB, double _rsrp_dBm, double _rsrq_dB, double _cqi) :
    snr_dB(_snr_dB),
    rsrp_dBm(_rsrp_dBm),
    rsrq_dB(_rsrq_dB),
    cqi(_cqi)
{

}

double LTEContext::getValue(const QString &_key)
{
    float value = -1;
    if(_key=="snr")
        value = snr_dB;
    else if(_key=="rsrp")
        value = rsrp_dBm;
    else if(_key=="rsrq")
        value = rsrq_dB;
    else if(_key=="cqi")
        value = cqi;
    return value;
}
