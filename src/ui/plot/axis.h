#ifndef AXIS_H
#define AXIS_H


#include <QString>
#include <QVector>
#include "tick.h"

struct Limits
{
    float min;
    float max;
};


class Axis
{
public:
    Axis();

    QString label;
    double min;
    double max;

    QVector<Tick> ticks;
    QString color;
};

#endif // AXIS_H
