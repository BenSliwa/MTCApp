#include "tracebased.h"

TraceBased::TraceBased(QObject *_parent) :
    MobilityPrediction("trace", _parent)
{

}

void TraceBased::load(const QStringList &_tracks)
{
    for(int i=0; i<_tracks.size(); i++)
    {
        Trace *trace = new Trace();
        trace->load(_tracks.at(i));
        m_traces << trace;
    }
}

Trace* TraceBased::findBestMatchingTrajectory(const Position &_position)
{
    Trace *trajectory = 0;

    double distance_m = std::numeric_limits<double>::max();
    for(int i=0; i<m_traces.size(); i++)
    {
        Trace *track = m_traces.at(i);
        TraceEntry entry = track->findNearestPosition(_position);

        if(entry.distanceDifference_m<distance_m) //&&<maxDistance
        {
            trajectory = track;
            distance_m = entry.distanceDifference_m;
        }
   }

   return trajectory;
}


MobilityEntry TraceBased::predict(float _width_s)
{
    MobilityEntry entry;
    entry.valid = false;


    // estimate the travellable distance
    Position position = p_gps->getPosition();
    float velocity_mps = p_gps->getVelocity();
    float distance_m = velocity_mps * _width_s;

    // find the best matching trace
    Trace *trajectory = findBestMatchingTrajectory(position);
    if(trajectory)
    {
        // move on the trace until the distance is reached
        TraceEntry nearestEntry = trajectory->findNearestPosition(position);

        //
        if(nearestEntry.distanceDifference_m<50)
        {
            TraceEntry prediction = trajectory->predict(distance_m, nearestEntry.index);

            entry.valid = true;
            entry.position = prediction.position;
            entry.lte.snr_dB = prediction.snr_dB;
            entry.lte.rsrp_dBm = prediction.rsrp_dBm;
            entry.lte.rsrq_dB = prediction.rsrq_dB;
            entry.lte.cqi = prediction.cqi;
        }
    }

    return entry;
}
