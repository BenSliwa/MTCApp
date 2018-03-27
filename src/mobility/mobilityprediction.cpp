#include "mobilityprediction.h"
#include "settings/filehandler.h"
#include "appcontroller.h"

MobilityPrediction::MobilityPrediction(QString _type, QObject *_parent) :
    QObject(_parent),
    m_type(_type),
    p_gps(0)
{
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(onTimeout()));

    p_gps = AppController::getInstance()->getGps();
}

void MobilityPrediction::start(int _width_s, const QString &_fileName)
{
    m_width_s = _width_s;
    m_fileName = _fileName + "_" + m_type + QString::number(_width_s);

    qDebug() << "MobilityPrediction::start" << m_width_s << m_fileName;

    m_timer.start(1000);
}

void MobilityPrediction::stop()
{

    disconnect(&m_timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    m_timer.stop();
}

float MobilityPrediction::getTimestamp()
{
    return AppController::getInstance()->getMeasurementTime() / 1000.0;
}

QString MobilityPrediction::getType()
{
    return m_type;
}

void MobilityPrediction::onTimeout()
{
    MobilityEntry entry = predict(m_width_s);


    if(entry.valid)
    {
        QString data;
        QTextStream stream(&data);
        stream << entry.toString();

        FileHandler::append(data, m_fileName + ".txt");
    }
    else
    {
        qDebug() << "invalid prediction";
    }

}
