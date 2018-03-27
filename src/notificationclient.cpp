/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtAndroidExtras module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "notificationclient.h"

#include <QtAndroidExtras/QAndroidJniObject>
#include <QDebug>
#include <QTextStream>

#include "appcontroller.h"
#include "sensor/gpsreceiver.h"


NotificationClient::NotificationClient(QObject *parent)
    : QObject(parent)
{
    connect(&m_updateTimer, SIGNAL(timeout()), this, SLOT(onUpdateTimeout()));
    m_updateTimer.start(100);
}


void NotificationClient::updateAndroidNotification()
{

    QAndroidJniObject str = QAndroidJniObject::callStaticObjectMethod<jstring>("org/cni/CellularNetworkInfo/notification/NotificationClient", "readCellularInfo");

    JSON json;
    QVariantMap data = json.parse(str.toString().toUtf8());
    QVariantList lteCellInfo = data.value("lteCellInfo").toList();

    for(int i=0; i<lteCellInfo.size(); i++)
    {
        QVariantMap report = lteCellInfo.at(i).toMap();
        bool isRegistered = report.value("isRegistered").toBool();
        if(isRegistered)
        {
            m_info = report;

            GpsReceiver *gps = AppController::getInstance()->getGps();
            m_info["latitude"] = gps->getLatitude();
            m_info["longitude"] = gps->getLongitude();
            m_info["velocity"] = gps->getVelocity();
        }
    }




    //qDebug().noquote() << str.toString().simplified();

    QAndroidJniObject networkScan = QAndroidJniObject::callStaticObjectMethod<jstring>("org/cni/CellularNetworkInfo/notification/NotificationClient", "scanNetworks");
   // qDebug().noquote() << networkScan.toString().simplified();




    emit receivedData(m_info);
}

QVariantMap NotificationClient::getInfo()
{
    return m_info;
}

void NotificationClient::onUpdateTimeout()
{
    updateAndroidNotification();
}
