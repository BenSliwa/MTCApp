#include "contextmanager.h"

#ifdef Q_OS_ANDROID
    #include <QtAndroid>
    #include <QtAndroidExtras/QAndroidJniObject>
#endif

#include <QDebug>
#include <QTextStream>

#include "appcontroller.h"
#include "sensor/gpsreceiver.h"



ContextManger::ContextManger(QObject *parent)
    : QObject(parent)
{
    connect(&m_updateTimer, SIGNAL(timeout()), this, SLOT(onUpdateTimeout()));
    m_updateTimer.start(100);
}


void ContextManger::updateAndroidNotification()
{
    #ifdef Q_OS_ANDROID

    QAndroidJniObject str = QAndroidJniObject::callStaticObjectMethod<jstring>("org/cni/CellularNetworkInfo/notification/NotificationClient", "readCellularInfo");

    JSON json;
    QVariantMap data = json.parse(str.toString().toUtf8());
    QVariantList lteCellInfo = data.value("lteCellInfo").toList();

    m_info = QVariantMap();
    for(int i=0; i<lteCellInfo.size(); i++)
    {
        QVariantMap report = lteCellInfo.at(i).toMap();
        bool isRegistered = report.value("isRegistered").toBool();
        if(isRegistered)
        {
            m_info = report;

            /*
            GpsReceiver *gps = AppController::getInstance()->getGps();
            m_info["latitude"] = gps->getLatitude();
            m_info["longitude"] = gps->getLongitude();
            m_info["altitude"] = gps->getAltitude();
            m_info["velocity"] = gps->getVelocity();
            m_info["direction"] = gps->getDirection();*/


        }
    }

    #else

    #endif

    GpsReceiver *gps = AppController::getInstance()->getGps();
    m_info["latitude"] = gps->getLatitude();
    m_info["longitude"] = gps->getLongitude();
    m_info["altitude"] = gps->getAltitude();
    m_info["velocity"] = gps->getVelocity();
    m_info["direction"] = gps->getDirection();



    //QAndroidJniObject networkScan = QAndroidJniObject::callStaticObjectMethod<jstring>("org/cni/CellularNetworkInfo/notification/NotificationClient", "scanNetworks");
   // qDebug().noquote() << networkScan.toString().simplified();
    //qDebug() << m_info["snr"].toString();



    emit receivedData(m_info);
}

QVariantMap ContextManger::getInfo()
{
    return m_info;
}

void ContextManger::onUpdateTimeout()
{
    updateAndroidNotification();
}
