#include "trackbased.h"

TrackBased::TrackBased(QObject *_parent) :
    MobilityPrediction("track", _parent)
{

}

void TrackBased::load(const QStringList &_tracks)
{
    for(int i=0; i<_tracks.size(); i++)
    {
        Trajectory *trajectory = new Trajectory();
        trajectory->loadTrace(_tracks.at(i));
        m_tracks << trajectory;
    }
}

Trajectory* TrackBased::findBestMatchingTrajectory(const Position &_position)
{
    Trajectory *trajectory = 0;

    double distance_m = std::numeric_limits<double>::max();
    for(int i=0; i<m_tracks.size(); i++)
    {
        Trajectory *track = m_tracks.at(i);
        TrajectoryEntry entry = track->findNearestPosition(_position);

        if(entry.distance_m<distance_m) //&&<maxDistance
        {
            trajectory = track;
            distance_m = entry.distance_m;
        }

        //qDebug() << "best match i:" << entry.position.toString() << entry.index << entry.distance_m;
    }

    return trajectory;
}

MobilityEntry TrackBased::predict(float _width_s)
{
    MobilityEntry entry;
    entry.valid = false;

    // estimate the travellable distance
    Position position = p_gps->getPosition();
    float velocity_mps = p_gps->getVelocity();
    float distance_m = velocity_mps * _width_s;

    // find the best matching trace
    Trajectory *trajectory = findBestMatchingTrajectory(position);
    if(trajectory)
    {
        // move on the trace until the distance is reached
        TrajectoryEntry nearestEntry = trajectory->findNearestPosition(position);

        //
        if(nearestEntry.distance_m<50)
        {
            Position prediction = trajectory->predict(distance_m, nearestEntry);
            entry.position = prediction;
            entry.valid = true;
        }
    }

    return entry;
}
