#include "transmissionpolicy.h"
#include <ctime>

TransmissionPolicy::TransmissionPolicy()
{
     srand((unsigned)time(NULL));
}

bool TransmissionPolicy::transmitWithProbability(float _probability)
{
    double value =((double)rand()/(double)RAND_MAX);

    if(value <= _probability)
        return true;
    return false;
}
