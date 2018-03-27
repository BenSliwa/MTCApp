#include "cat.h"
#include <QDebug>

CAT::CAT() :
    TransmissionPolicy(),
    m_tMin_s(10),
    m_tMax_s(120),
    m_transmissions(0),
    m_type(WEIGHTED_MEAN)
{

}

bool CAT::transmit(float _timeDelta_s, const QVariantMap &_context)
{
    bool transmit = false;

    //
    if((_timeDelta_s > m_tMin_s && _timeDelta_s < m_tMax_s) || (m_transmissions==0 && _timeDelta_s < m_tMax_s))
    {
        float probability = 1;
        switch(m_type)
        {
            case WEIGHTED_MEAN:
                probability = computeWeightedMeanProbability(_context);
            break;

            case OPTIMISTIC:
                probability = computeOptimisticProbability(_context);
            break;

            case PESSIMISTIC:
                probability = computePessimisticProbability(_context);
            break;
        }

        transmit = transmitWithProbability(probability);
        if(transmit)
            m_transmissions++;

        qDebug() << "CAT::transmit" << m_tMin_s << _timeDelta_s  << probability << transmit << m_transmissions;
    }
    else if(_timeDelta_s <= m_tMin_s)
        transmit = false;
    else if(_timeDelta_s > m_tMax_s)
    {
        transmit = true;
    }



    return transmit;
}

float CAT::computeOptimisticProbability(const QVariantMap &_context)
{
    float probability = 0;

    QVector<double> p;
    for(unsigned int i=0; i<m_metrics.size(); i++)
    {
        Metric metric = m_metrics.at(i);

        float kpi = _context.value(QString::fromStdString(metric.key)).toFloat();
        float exponent = powf(metric.exponent, metric.progressive);
        float p_i = powf((kpi-metric.min)/(metric.max-metric.min), exponent);

        probability = std::max(probability, p_i);
        p << p_i;
    }

    qDebug() << "CAT " << m_type << " : " << p << " = " << probability;

    return probability;
}

float CAT::computePessimisticProbability(const QVariantMap &_context)
{
    float probability = 1;

    QVector<double> p;
    for(unsigned int i=0; i<m_metrics.size(); i++)
    {
        Metric metric = m_metrics.at(i);

        float kpi = _context.value(QString::fromStdString(metric.key)).toFloat();
        float exponent = powf(metric.exponent, metric.progressive);
        float p_i = powf((kpi-metric.min)/(metric.max-metric.min), exponent);

        probability = std::min(probability, p_i);
        p << p_i;
    }

    return probability;
}

float CAT::computeWeightedMeanProbability(const QVariantMap &_context)
{
    float probability = 0;
    float weights = 0;

    QVector<double> p;
    for(unsigned int i=0; i<m_metrics.size(); i++)
    {
        Metric metric = m_metrics.at(i);

        float kpi = _context.value(QString::fromStdString(metric.key)).toFloat();
        float exponent = powf(metric.exponent, metric.progressive);
        float p_i = powf((kpi-metric.min)/(metric.max-metric.min), exponent);

        probability += p_i * metric.weight;
        weights += metric.weight;
        p << p_i;
    }

    probability /= weights;
    return probability;
}

int CAT::getTransmissionType(float _delta_s)
{
    int type = SEND;
    if((_delta_s > m_tMin_s && _delta_s < m_tMax_s) || (m_transmissions==0 && _delta_s < m_tMax_s))
        type = PROBABILISTIC;
    else if(_delta_s <= m_tMin_s)
        type = WAIT;
    else if(_delta_s > m_tMax_s)
        type = SEND;

    return type;
}


//
void CAT::addMetric(const Metric &_metric)
{
    m_metrics.push_back(_metric);
}

void CAT::setType(int _type)
{
    m_type = _type;
}

void CAT::setTimeValues(float _min, float _max)
{
    m_tMin_s = _min;
    m_tMax_s = _max;
}
