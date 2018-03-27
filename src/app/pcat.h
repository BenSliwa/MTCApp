#ifndef PCAT_H
#define PCAT_H

#include "cat.h"
#include "mobility/gpsbased.h"
#include "mobility/trackbased.h"
#include "mobility/tracebased.h"
#include "connectivity/connectivitymap.h"
#include "settings/logger.h"
#include "m5t.h"

class pCAT : public CAT
{
public:
    pCAT();

    void initPrediction(const QVariantMap &_settings);

    bool transmit(float _timeDelta_s, const QVariantMap &_context);

    float computeTransmissionProbabilityForMetric(float _current, float _predicted, const Metric &_metric);

    void activateExperimentalMode();

protected:
    ConnectivityMap *p_connectivityMap;
    MobilityPrediction *m_mobilityPrediction;
    float m_horizon_s;
    float m_gamma;

    Logger *p_logger;
    bool m_useExperimental;

};

#endif // PCAT_H
