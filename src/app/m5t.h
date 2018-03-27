#ifndef M5T_H
#define M5T_H


class M5T
{
public:
    M5T();

    static float predictGoodput(float payload_MB, float snr_dB, float rsrp_dB, float rsrq_dB, float cqi, float _speed_kmh);

};

#endif // M5T_H
