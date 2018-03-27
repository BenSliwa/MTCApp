#include "trace.h"
#include "sensor/gpsreceiver.h"

Trace::Trace()
{

}

void Trace::load(const QString &_file)
{
    m_fileName = _file;
    QStringList lines = FileHandler::read(m_fileName);
    for(int i=0; i<lines.size(); i++)
    {
        QStringList line = lines.at(i).split(",");
        if(line.size()>10)
        {
            TraceEntry entry;
            entry.time_s = line.at(0).toDouble();
            entry.distance_m = line.at(1).toDouble();
            entry.speed_mps = line.at(5).toDouble();

            float x = line.at(3).toDouble();
            float y = line.at(2).toDouble();
            entry.position = Position(x,y);

            entry.snr_dB = line.at(8).toDouble();
            entry.rsrp_dBm = line.at(9).toDouble();
            entry.rsrq_dB = line.at(10).toDouble();
            entry.cqi = line.at(11).toDouble();
            entry.index = m_entries.size();

            //printTrace(entry);
            m_entries << entry;
        }
    }
}

TraceEntry Trace::findNearestPosition(const Position &_position)
{
    TraceEntry entry;

    double minDistance_m = std::numeric_limits<double>::max();
    for(int i=0; i<m_entries.size(); i++)
    {
        TraceEntry currentEntry = m_entries.at(i);
        Position position = currentEntry.position;
        double distance_m = GpsReceiver::getDistance(_position, position);

        //qDebug() << "pos" << _position.toString() << position.toString();

        if(distance_m<minDistance_m)
        {
            minDistance_m = distance_m;
            entry = currentEntry;
            entry.distanceDifference_m = distance_m;
        }
    }

    return entry;
}

TraceEntry Trace::predict(float _distance_m, int _index)
{
    int index = _index;
    TraceEntry startEntry = m_entries.at(index);
    TraceEntry prediction = startEntry;

    // find the entry for the specified distance

    bool endReached = false;
    while(!endReached)
    {
        index++;
        if(index>m_entries.size()-1)
            break;

        TraceEntry entry = m_entries.at(index);
        float normedDistance_m = entry.distance_m - startEntry.distance_m;
        if(normedDistance_m>=_distance_m)
        {
            prediction = entry;
            endReached = true;
        }

    }


    return prediction;
}

void Trace::printTrace(const TraceEntry &_entry)
{
    qDebug() << "Trace::printTrace" << _entry.time_s << _entry.distance_m << _entry.snr_dB << _entry.rsrp_dBm << _entry.rsrq_dB << _entry.cqi;
}
