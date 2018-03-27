#include "connectivitymap.h"
#include "settings/filehandler.h"

ConnectivityMap::ConnectivityMap()
{

}

ConnectivityMap* ConnectivityMap::getInstance()
{
    static ConnectivityMap instance;
    return &instance;
}

void ConnectivityMap::init(const QVariantMap &_settings)
{
    m_min.x = _settings["minLon"].toFloat();
    m_min.y = _settings["minLat"].toFloat();
    m_max.x = _settings["maxLon"].toFloat();
    m_max.y = _settings["maxLat"].toFloat();
    m_cellSize_m = _settings["cellSize"].toFloat();
}

void ConnectivityMap::load(const QString &_path)
{
    m_data.clear();
    QStringList lines = FileHandler::read(_path);
    for(int i=0; i<lines.size(); i++)
    {
        QStringList line = lines.at(i).split(",");
        if(line.size()>5)
        {
            ConnectivityMapEntry entry;
            entry.key = line.at(0) + "," + line.at(1);
            entry.lte.snr_dB = line.at(2).toFloat();
            entry.lte.rsrp_dBm = line.at(3).toFloat();
            entry.lte.rsrq_dB = line.at(4).toFloat();
            entry.lte.cqi = line.at(5).toFloat();
            entry.isValid = true;

            m_data[entry.key] = entry;
        }
    }

    qDebug() << "ConnectivityMap::load" << m_data.size();
}

QString ConnectivityMap::computeCellKey(const Position &_position)
{
    float dY = GpsReceiver::getDistance(m_min, Position(m_min.x, _position.y)); // latitude distance - y direction
    float dX = GpsReceiver::getDistance(m_min, Position(_position.x, m_min.y)); // longitude distance - x direction

    int cellX = dX/m_cellSize_m;
    int cellY = dY/m_cellSize_m;

    QString key = QString::number(cellX) + "," + QString::number(cellY);

    return key;
}

ConnectivityMapEntry ConnectivityMap::getEntry(const Position &_position)
{
    QString key = computeCellKey(_position);
    ConnectivityMapEntry entry;
    entry.isValid = false;
    entry.key = key;

    if(m_data.contains(key))
        entry = m_data[key];

    return entry;
}
