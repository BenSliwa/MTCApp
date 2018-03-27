#include "mobilityentry.h"

MobilityEntry::MobilityEntry() :
    valid(true)
{

}

QString MobilityEntry::toString()
{
    QString data = QString::number(timestamp_s) + "," + position.toString() + "," + cellId;
    return data;
}
