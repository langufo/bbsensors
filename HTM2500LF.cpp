#include <math.h>

#include "HTM2500LF.h"

HTM2500LF::HTM2500LF(double tempDividerVoltage, double tempNonprobedR,
                     double humNonprobedR, double humProbedR) {

    HTM2500LF::tempDividerVoltage = tempDividerVoltage;
    HTM2500LF::tempNonprobedR = tempNonprobedR;
    HTM2500LF::humNonprobedR = humNonprobedR;
    HTM2500LF::humProbedR = humProbedR;
}

double HTM2500LF::getTemp(double voltage) {
    const double a = 8.54942e-4;
    const double b = 2.57305e-4;
    const double c = 1.65368e-7;

    double probedTempR = tempNonprobedR * voltage/(tempDividerVoltage-voltage);

    return 1 / (a + b*log(probedTempR) + c*pow(log(probedTempR), 3)) - 273.15;
}

double HTM2500LF::getHum(double voltage, double temp) {
    const double a = -1.9206e-9;
    const double b = 1.437e-5;
    const double c = 3.421e-3;
    const double d = -12.4;
    const double e = 2.4e-3;

    voltage = voltage*1e3 * (1+humNonprobedR/humProbedR);  // millivolt

    return (a*pow(voltage, 3) + b*pow(voltage, 2) + c*voltage + d) / (1 + e*(temp-23));
}
