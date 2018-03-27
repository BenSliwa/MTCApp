#ifndef CONNECTIVITYMAP_H
#define CONNECTIVITYMAP_H

#include <QVariantMap>
#include <QMap>
#include "sensor/position.h"
#include "sensor/gpsreceiver.h"
#include "connectivity/ltecontext.h"

struct ConnectivityMapEntry
{
    QString key;
    LTEContext lte;
    bool isValid;

    float getValue(const std::string &_key)
    {
        float value = -1;
        if(_key=="snr")
            value = lte.snr_dB;
        else if(_key=="rsrp")
            value = lte.rsrp_dBm;
        else if(_key=="rsrq")
            value = lte.rsrq_dB;
        else if(_key=="cqi")
            value = lte.cqi;
        return value;
    }
};

class ConnectivityMap
{
public:
    ConnectivityMap();
    static ConnectivityMap* getInstance();

    void init(const QVariantMap &_settings);
    void load(const QString &_path);

    QString computeCellKey(const Position &_position);
    ConnectivityMapEntry getEntry(const Position &_position);


private:
    Position m_min;
    Position m_max;
    int m_cellSize_m;

    QMap<QString,ConnectivityMapEntry> m_data;
};

#endif // CONNECTIVITYMAP_H
