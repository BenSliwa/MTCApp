#include "m5t.h"
#include <QDebug>

M5T::M5T()
{

}

float M5T::predictGoodput(float payload_MB, float snr_dB, float rsrp_dB, float rsrq_dB, float cqi, float _speed_kmh)
{
    float speed_mps = _speed_kmh / 3.6;

    float goodput_mbits;
    if ((payload_MB <= 1.259) && (payload_MB <= 0.565) && (rsrp_dB > -100.5)) {
        goodput_mbits = 3.2572 * payload_MB + 0.0002 * snr_dB - 0.0089 * rsrp_dB + 0.1999 * rsrq_dB + 0.0544 * cqi + 0.0025 * speed_mps + 2.738; // [798/35.3%]
    } else if ((payload_MB <= 1.543) && (rsrp_dB > -100.5) && (payload_MB <= 1.259)) {
        goodput_mbits = 1.8156 * payload_MB + 0.0002 * snr_dB + 0.0018 * rsrp_dB + 0.174 * rsrq_dB + 0.0006 * cqi + 0.0048 * speed_mps + 5.7261; // [684/34.538%]
    } else if ((rsrp_dB > -100.5) && (payload_MB <= 2.057) && (payload_MB > 1.543)) {
        goodput_mbits = 1.7174 * payload_MB + 0.0216 * snr_dB + 0.002 * rsrp_dB + 0.187 * rsrq_dB + 0.0513 * cqi + 7.5918; //[296/37.68%]
    } else if ((rsrp_dB > -100.5) && (payload_MB <= 1.8)) {
        goodput_mbits = 0.0672 * payload_MB + 0.0597 * snr_dB + 0.003 * rsrp_dB + 0.3522 * rsrq_dB - 0.0015 * cqi - 0.005 * speed_mps + 9.8522; //[282/42.916%]
    } else if ((rsrp_dB > -100.5) && (payload_MB <= 4.009)) {
        goodput_mbits = 0.4843 * payload_MB + 0.0026 * snr_dB + 0.0112 * rsrp_dB + 0.4706 * rsrq_dB - 0.0016 * cqi + 0.0056 * speed_mps + 13.8995; //[241/45.674%]
    } else if ((rsrp_dB <= -100.5) && (payload_MB <= 0.617)) {
        goodput_mbits = 0.0491 * payload_MB + 0.2164 * rsrp_dB + 0.0179 * rsrq_dB - 0.0078 * cqi + 26.1121; // [210/25.021%]
    } else if (rsrp_dB <= -100.5) {
        goodput_mbits = 0.1852 * payload_MB + 0.4577 * rsrp_dB + 0.0223 * rsrq_dB - 0.0914 * cqi + 53.7492; //[195/47.3%]
    } else if (rsrp_dB > -94.5) {
        goodput_mbits = 0.0795 * snr_dB + 0.0091 * rsrp_dB + 0.346 * rsrq_dB + 15.582; //[163/79.684%]
    } else {
        goodput_mbits = 0.4183 * rsrp_dB - 0.2225 * cqi + 52.2136; //[58/94.844%]
    }

    //qDebug() << "M5T::predict_goodput" << goodput_mbits << payload_MB << snr_dB << rsrp_dB << rsrq_dB << cqi << speed;

    return goodput_mbits;
}
