#ifndef CAT_H
#define CAT_H

#include "transmissionpolicy.h"
#include <vector>

struct Metric
{
    std::string key;
    float min;
    float max;
    float exponent;
    int progressive;
    float weight = 1;
};

enum FusionType
{
    WEIGHTED_MEAN,
    OPTIMISTIC,
    PESSIMISTIC,
};

enum TransmissionType
{
    SEND,
    WAIT,
    PROBABILISTIC
};

class CAT : public TransmissionPolicy
{
public:
    CAT();


    bool transmit(float _timeDelta_s, const QVariantMap &_context);
    float computeOptimisticProbability(const QVariantMap &_context);
    float computePessimisticProbability(const QVariantMap &_context);
    float computeWeightedMeanProbability(const QVariantMap &_context);

    int getTransmissionType(float _delta_s);

    //
    void addMetric(const Metric &_metric);
    void setType(int _type);

    void setTimeValues(float _min, float _max);

protected:
    std::vector<Metric> m_metrics;
    float m_tMin_s;
    float m_tMax_s;

    int m_transmissions;
    int m_type;
};
#endif // CAT_H
