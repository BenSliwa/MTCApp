#include "trajectory.h"

Trajectory::Trajectory()
{

}

void Trajectory::loadTrace(const QString &_file)
{
    m_fileName = _file;
    QStringList lines = FileHandler::read(m_fileName);
    for(int i=0; i<lines.size(); i++)
    {
        QStringList line = lines.at(i).split(",");
        if(line.size()>5)
        {
            double latitude = line.at(2).toDouble();
            double longitude = line.at(3).toDouble();
            Position position(longitude, latitude);

            m_positions << position;
        }
    }

    qDebug() << "Trajectory::loadTrace" << m_positions.size();
}

TrajectoryEntry Trajectory::findNearestPosition(const Position &_position)
{
    TrajectoryEntry entry;

    double minDistance_m = std::numeric_limits<double>::max();
    for(int i=0; i<m_positions.size(); i++)
    {
        Position position = m_positions.at(i);
        double distance_m = GpsReceiver::getDistance(_position, position);

        if(distance_m<minDistance_m)
        {
            minDistance_m = distance_m;
            entry.position = position;
            entry.index = i;
            entry.distance_m = distance_m;
        }
    }

    return entry;
}

Position Trajectory::predict(double _distance_m, const TrajectoryEntry &_start)
{
    double distancePotential_m = _distance_m;

    Position prediction = _start.position;
    for(int i=_start.index+1; i<m_positions.size(); i++)  // TODO: trajectory direction, could also be integrated into TrackBased::findBestMatchingTrajectory
    {
        Position waypoint = m_positions.at(i);
        double distance_m = GpsReceiver::getDistance(prediction, waypoint);
        if(distance_m<=distancePotential_m)
        {
            prediction = waypoint;
            distancePotential_m -= distance_m;
        }
        else // interpolate the position towards the next waypoint
        {
            Position dir = (waypoint-prediction);
            prediction = prediction + dir * (distancePotential_m/distance_m);

            distancePotential_m = 0;
        }

        if(distancePotential_m<=0)
            return prediction;
    }

    return prediction;
}

QString Trajectory::getFileName()
{
    return m_fileName;
}
