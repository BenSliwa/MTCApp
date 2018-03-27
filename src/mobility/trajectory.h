#ifndef TRAJECTORY_H
#define TRAJECTORY_H

#include "sensor/position.h"
#include "sensor/gpsreceiver.h"
#include "settings/filehandler.h"

struct TrajectoryEntry
{
    Position position;
    int index;
    float distance_m;
};

class Trajectory
{
public:
    Trajectory();

    void loadTrace(const QString &_file);
    TrajectoryEntry findNearestPosition(const Position &_position);

    Position predict(double _distance_m, const TrajectoryEntry &_start);

    //
    QString getFileName();

private:
    QVector<Position> m_positions;
    QString m_fileName;
};

#endif // TRAJECTORY_H
