#ifndef TRACE_H
#define TRACE_H

#include "settings/filehandler.h"
#include "sensor/position.h"

struct TraceEntry
{
    float distance_m;
    float time_s;
    float speed_mps;
    float rsrp_dBm;
    float rsrq_dB;
    float snr_dB;
    float cqi;
    Position position;

    //
    int index;
    float distanceDifference_m;
};

class Trace
{
public:
    Trace();

    void load(const QString &_file);
    TraceEntry findNearestPosition(const Position &_position);
    TraceEntry predict(float _distance_m, int _index);


    void printTrace(const TraceEntry &_entry);


public:
    QVector<TraceEntry> m_entries;
    QString m_fileName;
};



#endif // TRACE_H
