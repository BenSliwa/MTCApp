#include "appcontroller.h"
#include "app/periodic.h"
#include "app/cat.h"
#include "app/pcat.h"
#include "mobility/gpsbased.h"
#include "settings/filehandler.h"
#include "settings/json.h"
#include "settings/logger.h"
#include "connectivity/connectivitymap.h"

#include <QDir>
#include <QFile>

AppController::AppController(QObject *_parent) : QObject(_parent),
    m_gps(0),
    m_trackBasedPrediction(0),
    m_traceBasedPrediction(0),
    m_gpsBasedPrediction(0)
{
    m_startTime_ms = getTimestamp();
    m_contextManager = new ContextManger(this);
}

AppController* AppController::getInstance()
{
    static AppController instance;
    return &instance;
}

void AppController::init()
{
    m_uiManager.addQtObject("AppController", this);
    m_uiManager.load("qrc:/qml/main.qml");

    m_gps = new GpsReceiver(this);
    connect(&m_updateTimer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    connect(m_contextManager, SIGNAL(receivedData(QVariantMap)), this, SLOT(onDataReception(QVariantMap)));
}

void AppController::start(const QString &_config)
{
    m_distance_m = 0;
    m_tripDuration_s = 0;
    m_tripStart_ms = getTimestamp();
    m_lastPosition = m_gps->getPosition();

    //
    m_fileName = QDateTime::currentDateTime().toString("ddMMyy_hhmmss");

    if(_config!="")
        loadSettings(_config);


    initPredictions();

    m_updateTimer.start(1000);
    onTimeout();


}

void AppController::stop()
{
    for(int i=0; i<m_apps.size(); i++)
        m_apps.at(i)->deleteLater();
    m_apps.clear();

    m_updateTimer.stop();
}

void AppController::loadSettings(const QString &_config)
{
    qDebug() << "AppController::loadSettings" << _config;

    QString config = ":/" + _config + ".json";

    FileHandler fh;
    JSON json;
    QString rawData = fh.read(config).join("\n");
    QVariantMap data = json.parse(rawData.toUtf8());

    Logger *logger = Logger::getInstance();
    #ifdef Q_OS_ANDROID
        logger->init("/sdcard/log.txt");
    #else
        logger->init("log.txt");
    #endif

    //
    QVariantList apps = data["apps"].toList();
    for(int a=0; a<apps.size(); a++)
    {
        QVariantMap appData = apps.at(a).toMap();

        QString name = appData["name"].toString();
        QString address = appData["address"].toString();
        int port = appData["port"].toInt();
        int sensorInterval_ms = appData["sensorInterval"].toDouble() * 1000.0;
        int transmissionInterval_ms = appData["transmissionInterval"].toDouble() * 1000.0;
        int payload_byte = appData["sensorPayload"].toInt();


        SensorApp *app = new SensorApp(name, m_tripStart_ms, this);
        if(name.contains("periodic"))
        {
            app->setTransmissionPolicy(new Periodic());
        }
        else
        {
            CAT *cat = 0;
            if(name.contains("pCAT"))
            {
                QVariantMap mobilityPredictionData = appData["mobilityPrediction"].toMap();

                pCAT *pCat = new pCAT();
                pCat->initPrediction(mobilityPredictionData);

                if(appData.contains("experimental"))
                    pCat->activateExperimentalMode();

                cat = pCat;

                ConnectivityMap *map = ConnectivityMap::getInstance();
                map->init(appData["connectivityMap"].toMap());
                map->load(":/data/map.txt");
            }
            else
            {
                cat = new CAT();
            }

            if(appData.contains("type"))
            {
                QString type = appData.value("type").toString();
                if(type=="optimistic")
                    cat->setType(OPTIMISTIC);
                else if(type=="pessimistic")
                    cat->setType(PESSIMISTIC);
            }

            QVariantList metrics = appData["metrics"].toList();
            for(int m=0; m<metrics.size(); m++)
            {
                QVariantMap metricData = metrics.at(m).toMap();

                Metric metric;
                metric.key = metricData["key"].toString().toStdString();
                metric.exponent = metricData["exponent"].toDouble();
                metric.max = metricData["max"].toDouble();
                metric.min = metricData["min"].toDouble();
                metric.progressive = metricData["progressive"].toInt();

                qDebug() << "CAT" << QString::fromStdString(metric.key) << metric.exponent << metric.max << metric.min << metric.progressive;

                cat->addMetric(metric);
            }

            float tMin_s = appData["tMin"].toDouble();
            float tMax_s = appData["tMax"].toDouble();
            cat->setTimeValues(tMin_s, tMax_s);

            app->setTransmissionPolicy(cat);
        }

        app->setFileName("/sdcard/" + m_fileName);
        app->setServer(address, port);
        app->startSensor(sensorInterval_ms, payload_byte);
        app->startTransmission(transmissionInterval_ms);

        qDebug() << "load app" << sensorInterval_ms << payload_byte << transmissionInterval_ms;
        qDebug() << address << port;

        connect(app, SIGNAL(transmitted(QString)), this, SLOT(onTransmitted(QString)));
        m_apps << app;
    }
}

GpsReceiver* AppController::getGps()
{
    return m_gps;
}

ContextManger* AppController::getClient()
{
    return m_contextManager;
}

qint64 AppController::getPassedTime()
{
    return getTimestamp() - m_startTime_ms;
}

qint64 AppController::getMeasurementTime()
{
    return getTimestamp() - m_tripStart_ms;
}

qint64 AppController::getStartTime()
{
    return m_startTime_ms;
}

qint64 AppController::getTimestamp()
{
    return QDateTime::currentMSecsSinceEpoch();
}

void AppController::initPredictions()
{
    if(!m_trackBasedPrediction)
    {
        m_trackBasedPrediction = new TrackBased();

        QStringList tracks;
        tracks << ":/data/track1.txt";
        tracks << ":/data/track2.txt";
        m_trackBasedPrediction->load(tracks);
    }
    if(!m_traceBasedPrediction)
    {
        m_traceBasedPrediction = new TraceBased();

        QStringList traces;
        traces << ":/data/track1.txt";
        traces << ":/data/track2.txt";
        m_traceBasedPrediction->load(traces);
    }
    if(!m_gpsBasedPrediction)
        m_gpsBasedPrediction = new GpsBased;
}

void AppController::logPredictions()
{
    QVariantMap info = m_contextManager->getInfo();
    QString data;
    QTextStream stream(&data);

    // time
    stream << m_tripDuration_s << ",";

    // geo
    stream << m_distance_m << ",";
    stream << m_lastPosition.y << ",";
    stream << m_lastPosition.x << ",";

    stream << m_gps->getVelocity() << ",";
    stream << m_gps->getDirection() << ",";

    // signal
    bool connected = (info["type"]=="LTE");
    stream << connected << ",";
    stream << info["snr"].toFloat() << ",";
    stream << info["rsrp"].toFloat() << ",";
    stream << info["rsrq"].toFloat() << ",";
    stream << info["cqi"].toFloat() << ",";


    QList<int> horizons = QList<int>() << 10 << 30 << 60;
    for(int i=0; i<horizons.size(); i++)
    {
        MobilityEntry predictedPosition = m_trackBasedPrediction->predict(horizons.at(i));
        stream << createLogEntry(predictedPosition.position, predictedPosition.valid);
    }

    for(int i=0; i<horizons.size(); i++)
    {
        QString data;
        QTextStream log(&data);

        MobilityEntry entry = m_traceBasedPrediction->predict(horizons.at(i));
        if(entry.valid)
        {
            log << entry.position.y << ",";
            log << entry.position.x << ",";
            log << entry.lte.snr_dB << ",";
            log << entry.lte.rsrp_dBm << ",";
            log << entry.lte.rsrq_dB << ",";
            log << entry.lte.cqi << ",";
        }
        else
        {
            for(int i=0; i<6; i++)
                log << "nan,";
        }

        stream << data;
    }


    for(int i=0; i<horizons.size(); i++)
    {
        MobilityEntry predictedPosition = m_gpsBasedPrediction->predict(horizons.at(i));
        stream << createLogEntry(predictedPosition.position, predictedPosition.valid);
    }

    QString path = "/sdcard/" + m_fileName + "_pred.txt";
    FileHandler fh;
    fh.append(data, path);
}

QString AppController::createLogEntry(const Position &_position, bool _valid)
{
    QString data;
    QTextStream stream(&data);
    if(_valid)
    {
        stream << _position.y << ",";
        stream << _position.x << ",";

        // predict the future context from the connectivity map
        ConnectivityMap *p_connectivityMap = ConnectivityMap::getInstance();
        ConnectivityMapEntry entry = p_connectivityMap->getEntry(_position);
        if(entry.isValid)
        {
            stream << entry.lte.snr_dB << ",";
            stream << entry.lte.rsrp_dBm << ",";
            stream << entry.lte.rsrq_dB << ",";
            stream << entry.lte.cqi << ",";
        }
        else
        {
            for(int i=0; i<4; i++)
                stream << "nan,";
        }
    }
    else
    {
        for(int i=0; i<6; i++)
            stream << "nan,";
    }

    return data;
}

void AppController::onTimeout()
{
    QVariantMap info = m_contextManager->getInfo();

    m_tripDuration_s = getMeasurementTime() / 1000.0;


    QString data;
    QTextStream stream(&data);

    // time
    stream << m_tripDuration_s << ",";

    // geo
    Position position = m_gps->getPosition();
    if(m_lastPosition.norm()>0 && position.norm()>0)
        m_distance_m += m_gps->getDistance(m_lastPosition, position);
    m_lastPosition = position;

    stream << m_distance_m << ",";
    stream << position.y << ",";
    stream << position.x << ",";
    stream << position.z << ",";
    stream << m_gps->getVelocity() << ",";
    stream << m_gps->getDirection() << ",";

    // signal
    bool connected = (info["type"]=="LTE");
    stream << connected << ",";
    stream << info["snr"].toFloat() << ",";
    stream << info["rsrp"].toFloat() << ",";
    stream << info["rsrq"].toFloat() << ",";
    stream << info["cqi"].toFloat() << ",";
    stream << info["asu"].toFloat() << ",";
    stream << info["ss"].toFloat() << ",";
    stream << info["ta"].toInt() << ",";

    // cell
    stream << info["ci"].toInt() << ",";
    stream << info["mcc"].toInt() << ",";
    stream << info["mnc"].toInt() << ",";
    stream << info["pci"].toInt() << ",";
    stream << info["tac"].toInt() << ",";




    QString path = "/sdcard/" + m_fileName + ".txt";
    FileHandler fh;
    fh.append(data, path);

    logPredictions();
}

void AppController::onDataReception(const QVariantMap &_info)
{
    // compose the string for the ui representation
    QString result = "";
    QTextStream stream(&result);

    stream << "ASU: " << _info["asu"].toFloat() << "\n";
    stream << "PCI: " << _info["pci"].toFloat() << "\n";
    stream << "dBm: " << _info["dBm"].toFloat() << "\n";
    stream << "RSRP: " << _info["rsrp"].toFloat() << "\n";
    stream << "RSRQ: " << _info["rsrq"].toFloat() << "\n";
    stream << "SNR: " << _info["snr"].toFloat() << "\n";
    stream << "CQI: " << _info["cqi"].toFloat() << "\n";
    stream << "TA: " << _info["ta"].toString() << "\n";
    stream << "d: " << _info["ta"].toFloat() * 78.125 << " m\n";
    stream << "SS: " << _info["ss"].toFloat() << "\n";
    stream << "Net: " << _info["type"].toString() << "\n";

    stream << "Trip: " << m_distance_m << " m - " << m_tripDuration_s << " s \n";
    stream << "POS: " << QString::number(_info["latitude"].toFloat(), 'f', 2) << " - ";
    stream << QString::number(_info["longitude"].toFloat(), 'f', 2) << " - ";
    stream << QString::number(_info["velocity"].toFloat()*3.6, 'f', 2) << " km/h - ";
    stream << QString::number(_info["direction"].toFloat(), 'f', 2) << "\n";

    //if(_info.contains("raw"))
    //    stream << _info["raw"].toString() << "\n";

    stream << "\n";

    for(int i=0; i<m_apps.size(); i++)
    {
        SensorApp *app = m_apps.at(i);

        Statistics statistics = app->getStatistics();
        stream << app->getName();
        stream << ": " << QString::number(statistics.meanThroughput_mbps, 'f', 2) << "\t";
        stream << ": " << QString::number(statistics.throughputPerTransmission_mbps, 'f', 2);
        stream << " MBit/s\n";
    }


    emit updateInfo(result);

    QVariantMap context = _info;
    context["time"] = getPassedTime() / 1000.0;

    emit updateContext(context);
}

void AppController::onTransmitted(const QString &_name)
{
    float time_s = getPassedTime() / 1000.0;
    emit appTransmitted(time_s, _name);
}

