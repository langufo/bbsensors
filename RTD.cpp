#include "RTD.h"

RTD::RTD(double alpha, double rtd0R, 
         double currentR, double currentV, 
         double ampGainR, double ampRefV, bool inverting) {
    RTD::alpha = alpha;
    RTD::ampRefV = ampRefV;
    k = currentR/rtd0R/currentV / (5 + 2e5/ampGainR) * (inverting ? -1 : 1);
}

double RTD::getTemp(double adcInV) {
    return ((adcInV-ampRefV)*k - 1) / alpha;
}