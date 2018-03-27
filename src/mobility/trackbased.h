#ifndef TRACKBASED_H
#define TRACKBASED_H

#include "mobilityprediction.h"
#include <QStringList>
#include <QVector>
#include "mobility/trajectory.h"


class TrackBased : public MobilityPrediction
{
    Q_OBJECT
public:
    TrackBased(QObject *_parent = 0);

    void load(const QStringList &_tracks);
    Trajectory* findBestMatchingTrajectory(const Position &_position);


    MobilityEntry predict(float _width_s);

private:
    QVector<Trajectory*> m_tracks;
};

#endif // TRACKBASED_H
