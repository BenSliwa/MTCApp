#include "pcat.h"
#include <QDebug>
#include <QTextStream>


pCAT::pCAT() :
    CAT(),
    m_mobilityPrediction(0),
    m_horizon_s(10),
    m_gamma(1),
    m_useExperimental(false)
{
    p_connectivityMap = ConnectivityMap::getInstance();
    p_logger = Logger::getInstance();
}

void pCAT::initPrediction(const QVariantMap &_settings)
{
    QString type = _settings["type"].toString();
    if(type=="gpsBased")
    {
        m_mobilityPrediction = new GpsBased();
    }
    else if(type=="trackBased")
    {
        QStringList tracks = _settings["tracks"].toStringList();

        TrackBased *trackBased = new TrackBased();
        trackBased->load(tracks);

        m_mobilityPrediction = trackBased;
    }
    else if(type=="traceBased")
    {
        QStringList traces = _settings["tracks"].toStringList();

        TraceBased *traceBased = new TraceBased();
        traceBased->load(traces);

        m_mobilityPrediction = traceBased;

        qDebug() << "init tracebased" << traces;



        /*


                TODO: kein zugriff auf map, direkte verwendungvm



        */

    }

    m_gamma = _settings["gamma"].toFloat();
    m_horizon_s = _settings["horizon"].toFloat();

    qDebug() << "gamma" << m_gamma << m_horizon_s;
}

bool pCAT::transmit(float _timeDelta_s, const QVariantMap &_context)
{
    bool transmit = false;

    // predict the future position
    double lat = _context["latitude"].toDouble();
    double lon = _context["longitude"].toDouble();
    Position currentPosition(lon, lat);
    QString cellKey = p_connectivityMap->computeCellKey(currentPosition);



    if(currentPosition.isValid()) // gps valid
    {
        // predict the future position
        double velocity_mps = _context["velocity"].toDouble();
        double direction = _context["direction"].toDouble();

        MobilityEntry predictedPosition = m_mobilityPrediction->predict(m_horizon_s);
        QString predictedCellKey = p_connectivityMap->computeCellKey(predictedPosition.position);

        qDebug() << "cells" << cellKey << predictedCellKey << currentPosition.toString() << predictedPosition.position.toString() << velocity_mps;



        if(m_mobilityPrediction->getType()=="trace")
        {
            MobilityEntry prediction = m_mobilityPrediction->predict(m_horizon_s);
            if(prediction.valid)
            {
                // predict the future payload size
                float payload_MB = _context["payload"].toFloat() + m_horizon_s * 0.0512;          // TODO: read sensor frequency and payload size

                // apply the pCAT mechanism to compute the transmission probability
                int transmissionType = getTransmissionType(_timeDelta_s);
                if(transmissionType==PROBABILISTIC)
                {
                    QVector<double> probabilities;
                    double overallProbability = 0;
                    // evaluate all metrics
                    for(unsigned int i=0; i<m_metrics.size(); i++)
                    {
                        Metric metric = m_metrics.at(i);
                        float kpi = _context.value(QString::fromStdString(metric.key)).toFloat();
                        float predicted_kpi = prediction.lte.getValue(QString::fromStdString(metric.key));

                        // if the cell is unchanged, prefer the measured kpi over the saved one
                        if(predictedCellKey==cellKey)
                            predicted_kpi = kpi;

                        if(metric.key=="goodput_prediction")
                        {
                            kpi = _context["goodput_prediction"].toFloat();
                            predicted_kpi = M5T::predictGoodput(payload_MB, prediction.lte.snr_dB, prediction.lte.rsrp_dBm, prediction.lte.rsrq_dB, prediction.lte.cqi, velocity_mps);
                        }

                        float p_i = computeTransmissionProbabilityForMetric(kpi, predicted_kpi, metric);
                        probabilities << p_i;

                        //qDebug() << "TX?" << QString::fromStdString(metric.key) << kpi << predicted_kpi << p_i;

                    }

                    // compute the overall probability
                    overallProbability = probabilities.at(0);
                    if(probabilities.size()>1)
                    {
                        for(int i=0; i<probabilities.size(); i++)
                        {
                            if(probabilities.at(i)<overallProbability)
                                overallProbability = probabilities.at(i);
                        }
                    }


                    qDebug() << "TX?" << overallProbability << probabilities;

                    //
                    transmit = transmitWithProbability(overallProbability);
                    if(transmit)
                    {
                        m_transmissions++;
                        //
                    }



                }
                else if(transmissionType==WAIT)
                    transmit = false;
                else if(transmissionType==SEND)
                    transmit = true;
            }
            else
                return CAT::transmit(_timeDelta_s, _context);
        }
        else if(predictedPosition.position.isValid())
        {
            // predict the future context from the connectivity map

            ConnectivityMapEntry entry = p_connectivityMap->getEntry(predictedPosition.position);
            if(entry.isValid)
            {
                // predict the future payload size
                float payload_MB = _context["payload"].toFloat() + m_horizon_s * 0.0512;          // TODO: read sensor frequency and payload size

                // apply the pCAT mechanism to compute the transmission probability
                int transmissionType = getTransmissionType(_timeDelta_s);
                if(transmissionType==PROBABILISTIC)
                {
                    for(unsigned int i=0; i<m_metrics.size(); i++)
                    {
                        Metric metric = m_metrics.at(i);
                        float kpi = _context.value(QString::fromStdString(metric.key)).toFloat();
                        float predicted_kpi = entry.getValue(metric.key);

                        // if the cell is unchanged, prefer the measured kpi over the saved one
                        if(predictedCellKey==cellKey)
                            predicted_kpi = kpi;

                        if(metric.key=="goodput_prediction")
                        {
                            kpi = _context["goodput_prediction"].toFloat();
                            predicted_kpi = M5T::predictGoodput(payload_MB, entry.lte.snr_dB, entry.lte.rsrp_dBm, entry.lte.rsrq_dB, entry.lte.cqi, velocity_mps);
                        }



                        float p_i = computeTransmissionProbabilityForMetric(kpi, predicted_kpi, metric);
                        transmit = transmitWithProbability(p_i);
                        if(transmit)
                        {
                            m_transmissions++;

                            //

                        }

                        qDebug() << "TX?" << QString::fromStdString(metric.key) << kpi << predicted_kpi << p_i;

                        // logging
                        QString log; QTextStream stream(&log); stream << "pCAT::transmit " << kpi << "," << predicted_kpi << "," << p_i << "," << currentPosition.toString();
                        p_logger->log(log); qDebug() << log;
                    }
                }
                else if(transmissionType==WAIT)
                    transmit = false;
                else if(transmissionType==SEND)
                    transmit = true;
            }
            else // no connectivity entry
            {
                p_logger->log("pCAT::missing entry," + currentPosition.toString() + "," + entry.key );
                return CAT::transmit(_timeDelta_s, _context);
            }

        }
        else // no valid predicted position
        {
            p_logger->log("pCAT::invalid mobility prediction," + currentPosition.toString());
            return CAT::transmit(_timeDelta_s, _context);
        }
    }
    else // no valid gps signal
    {
        p_logger->log("pCAT::invalid gps");
        return CAT::transmit(_timeDelta_s, _context);
    }

    return transmit;
}

float pCAT::computeTransmissionProbabilityForMetric(float _current, float _predicted, const Metric &_metric)
{
    float delta = _predicted - _current;
    float m = (_current - _metric.min) / (_metric.max - _metric.min);
    float z = 1;
    if(delta>0) // channel gets better
    {
        z = std::max(delta * (1.0 - m) * m_gamma, 1.0);
    }
    else if(delta==0)
        z = 1;
    else // channel gets worse
    {
        z = 1/std::max(fabs(delta * m * m_gamma), 1.0);
    }
    float p = powf(m, _metric.exponent * z);

    if(m_useExperimental) // new pCAT
    {
        float m = (_current - _metric.min - delta * m_gamma) / (_metric.max - _metric.min);
        p = powf(m, _metric.exponent);
    }



    return p;
}

void pCAT::activateExperimentalMode()
{
    qDebug() << "pCAT::activateExperimentalMode";

    m_useExperimental = true;
}


