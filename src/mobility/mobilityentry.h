#ifndef MOBILITYENTRY_H
#define MOBILITYENTRY_H

#include <QObject>
#include "sensor/position.h"
#include "connectivity/ltecontext.h"

class MobilityEntry
{
public:
    MobilityEntry();

    QString toString();

public:
    float timestamp_s;
    Position position;
    QString cellId;
    bool valid;

    // TODO: clean up
    LTEContext lte;
};

#endif // MOBILITYENTRY_H
