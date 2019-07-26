#include <math.h>

#include "HTM2500LF.h"

HTM2500LF::HTM2500LF(double nonprobedTempR,
                     double nonprobedHumR, double probedHumR) {

    HTM2500LF::nonprobedTempR = nonprobedTempR;
    HTM2500LF::nonprobedHumR = nonprobedHumR;
    HTM2500LF::probedHumR = probedHumR;
}

double HTM2500LF::getTemp(double adcValue) {
    const double a = 8.54942e-4;
    const double b = 2.57305e-4;
    const double c = 1.65368e-7;

    double probedTempR = nonprobedTempR * adcValue/(4096-adcValue);

    return 1 / (a + b*log(probedTempR) + c*pow(log(probedTempR), 3)) - 273.15;
}

double HTM2500LF::getHum(double adcValue, double temp) {
    const double a = -1.9206e-9;
    const double b = 1.437e-5;
    const double c = 3.421e-3;
    const double d = -12.4;
    const double e = 2.4e-3;

    double voltage = 1800 * adcValue/4096 * (1+nonprobedHumR/probedHumR);  // millivolt
    
    return (a*pow(voltage, 3) + b*pow(voltage, 2) + c*voltage + d) / (1 + e*(temp-23));
}
