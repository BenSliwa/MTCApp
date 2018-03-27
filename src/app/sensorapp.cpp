#include "sensorapp.h"
#include "net/connectionpolicy.h"
#include "settings/filehandler.h"
#include "appcontroller.h"
#include "contextmanager.h"
#include <ctime>
#include "m5t.h"

SensorApp::SensorApp(const QString &_name, qint64 _timeReference_ms, QObject *_parent) :
    QObject(_parent),
    m_name(_name),
    m_transmissionPolicy(0),
    m_timeReference_ms(_timeReference_ms),
    m_lastTransmission_s(0),
    m_totalTransmissions(0),
    m_totalAcks(0)
{
    srand((unsigned)time(NULL));

    ConnectionPolicy *policy = new ConnectionPolicy(&m_http, this);

    connect(&m_sensorTimer, SIGNAL(timeout()), this, SLOT(onSensorTimeout()));
    connect(&m_transmissionTimer, SIGNAL(timeout()), this, SLOT(onTransmissionTimeout()));
    connect(&m_http, SIGNAL(transmitted(Transmission*)), this, SLOT(onTransmitted(Transmission*)));
    connect(&m_http, SIGNAL(transmissionFailed(Transmission*)), this, SLOT(onTransmissionFailed(Transmission*)));
}

void SensorApp::setServer(const QString &_address, int _port)
{
    m_http.startConnection(_address, _port);
}

void SensorApp::startSensor(int _updateInterval_ms, int _dataPerUpdate_byte)
{
    m_dataPerUpdate_byte = _dataPerUpdate_byte;
    m_sensorTimer.start(_updateInterval_ms);

    qDebug() << "SensorApp::startSensor" << _updateInterval_ms << m_dataPerUpdate_byte;
}

void SensorApp::startTransmission(int _updateInterval_ms)
{
    m_transmissionTimer.start(_updateInterval_ms);
}

int SensorApp::getRandomInt(int _min, int _max)
{
    int number = qrand() % ((_max + 1) - _min) + _min;
    return number;
}

void SensorApp::setTransmissionPolicy(TransmissionPolicy *_transmissionPolicy)
{
    m_transmissionPolicy = _transmissionPolicy;
}

void SensorApp::setFileName(const QString &_fileName)
{
    m_fileName = _fileName;
}

QString SensorApp::getName()
{
    return m_name;
}

Statistics SensorApp::getStatistics()
{
    return m_statistics;
}

void SensorApp::onSensorTimeout()
{
    SensorData sensorData;
    sensorData.timestamp_ms = m_http.getTime();
    while(sensorData.data.size()<m_dataPerUpdate_byte)
        sensorData.data.append(QString::number(getRandomInt(0, 255)));
    m_dataBuffer << sensorData;
}

void SensorApp::onTransmissionTimeout()
{
    if(m_dataBuffer.size()==0)
        return;

    QVariantMap context = AppController::getInstance()->getClient()->getInfo();

    // estimate the currently achievable goodput
    bool predict = true;
    if(predict)
    {
        float payload_MB = 0;
        for(int i=0; i<m_dataBuffer.size(); i++)
            payload_MB += m_dataBuffer.at(i).data.size();
        payload_MB /= powf(10,6);

        float snr_dB = context["snr"].toFloat();
        float rsrp_dB = context["rsrp"].toFloat();
        float rsrq_dB = context["rsrq"].toFloat();
        float cqi = context["cqi"].toFloat();
        float velocity_mps = context["velocity"].toFloat();

        float goodput_mbps = M5T::predictGoodput(payload_MB, snr_dB, rsrp_dB, rsrq_dB, cqi, velocity_mps);
        context["goodput_prediction"] = goodput_mbps;
        context["payload"] = payload_MB;
    }

    // fake a cellular connection on desktop OS
    #ifdef Q_OS_ANDROID

    #else
        context["type"] = "LTE";
    #endif

    // evaluate the transmission probability
    float time_s = (m_http.getTime() - m_timeReference_ms)/1000.0;
    float timeDelta_s = time_s - m_lastTransmission_s;
    if(context["type"].toString()=="LTE" && m_transmissionPolicy->transmit(timeDelta_s, context))
    {
        m_totalTransmissions++;
        m_lastTransmission_s = time_s;
        emit transmitted(m_name);

        TransmissionInfo info;
        info.data = m_dataBuffer;
        info.context = context;
        m_dataBuffer.clear();

        QByteArray data;
        for(int i=0; i<info.data.size(); i++)
            data.push_back(info.data.at(i).data);

        // save a mapping for the post request to the sensor data
        Transmission *transmission = m_http.sendData(data);
        m_transmissions[transmission] = info;
    }
}

void SensorApp::onTransmitted(Transmission *_transmission)
{
    if(m_transmissions.contains(_transmission))
    {
        m_totalAcks++;
        TransmissionInfo info = m_transmissions[_transmission];


        qDebug() << "SensorApp::onTransmitted" << _transmission->payloadSize_MB << "MB" << "-" << _transmission->deltaTime_ms << "ms" << "-" << _transmission->throughput_mbps << "MBit/s";

        // update internal statistics
        m_statistics.transmissions++;
        m_statistics.transmittedData_MB += _transmission->payloadSize_MB;
        m_statistics.transmittedTime_s += _transmission->deltaTime_ms/1000.0;
        m_statistics.totalThroughput_mbps += _transmission->throughput_mbps;

        if(m_statistics.transmittedTime_s>0)
            m_statistics.meanThroughput_mbps = m_statistics.transmittedData_MB * 8.0 / m_statistics.transmittedTime_s;
        if(m_statistics.transmissions>0)
            m_statistics.throughputPerTransmission_mbps = m_statistics.totalThroughput_mbps / (float)m_statistics.transmissions;


        //
        qint64 age_ms = 0;
        for(int i=0; i<info.data.size(); i++)
        {
            SensorData data = info.data.at(i);
            age_ms += _transmission->ackTime_ms - data.timestamp_ms;
        }
        age_ms /= info.data.size();

        // write statistics

        QString data;
        QTextStream stream(&data);

        stream << (_transmission->sendTime_ms - m_timeReference_ms)/1000.0 << ",";
        stream << _transmission->payloadSize_MB << ",";
        stream << _transmission->deltaTime_ms/1000.0 << ",";
        stream << _transmission->throughput_mbps << ",";
        stream << age_ms/1000.0 << ",";

        stream << info.context["snr"].toFloat() << ",";
        stream << info.context["rsrp"].toFloat() << ",";
        stream << info.context["rsrq"].toFloat() << ",";
        stream << info.context["cqi"].toFloat() << ",";
        stream << info.context["asu"].toFloat() << ",";

        stream << m_lastTransmission_s << ",";

        stream << m_totalTransmissions << ",";
        stream << m_totalAcks << ",";
        stream << info.context["velocity"].toFloat() * 3.6 << ",";

        FileHandler fh;
        fh.append(data, m_fileName + "_" + m_name + ".txt");



        if(info.context.contains("goodput_prediction"))
        {
            QString data;
            QTextStream stream(&data);

            stream << _transmission->throughput_mbps << ",";
            stream << info.context["goodput_prediction"].toFloat();
        }

        m_totalTransmissions = 0;
        m_totalAcks = 0;

        m_transmissions.remove(_transmission);
    }

    delete _transmission;
}

void SensorApp::onTransmissionFailed(Transmission *_transmission)
{
    if(m_transmissions.contains(_transmission))
    {
        TransmissionInfo info = m_transmissions[_transmission];

        // the transmission failed - buffer the data
        m_dataBuffer.append(info.data);
        m_transmissions.remove(_transmission);
    }

    delete _transmission;
}
