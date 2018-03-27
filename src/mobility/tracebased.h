#ifndef TRACEBASED_H
#define TRACEBASED_H

#include "mobilityprediction.h"
#include <QStringList>
#include <QVector>
#include "mobility/trajectory.h"
#include "trace.h"

class TraceBased : public MobilityPrediction
{
    Q_OBJECT
public:
    TraceBased(QObject *_parent = 0);

    void load(const QStringList &_tracks);
    Trace* findBestMatchingTrajectory(const Position &_position);

    MobilityEntry predict(float _width_s);

private:
    QVector<Trace*> m_traces;
};

#endif // TRACEBASED_H
