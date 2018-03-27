#ifndef TRANSMISSION_H
#define TRANSMISSION_H

#include <QDebug>
#include <QByteArray>

class Transmission
{
public:
    Transmission();

public:
    QByteArray data;
    qint64 sendTime_ms;
    qint64 ackTime_ms;
    qint64 deltaTime_ms;

    float payloadSize_MB;
    float throughput_mbps;
};

#endif // TRANSMISSION_H
