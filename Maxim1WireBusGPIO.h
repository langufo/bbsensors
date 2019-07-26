#ifndef MAXIM1WIRE_H
#define MAXIM1WIRE_H

#include <stdint.h>
#include <vector>

#include "AM335xGPIO.h"

class Maxim1WireBus {
public:
    // Maxim1WireBus(const char* gpioFolder);
    Maxim1WireBus(AM335xGPIO& gpio);
    std::vector<uint_fast64_t> getDeviceIDs();
    int reset();
    int read();
    void write(uint_fast64_t data, int bits = 1);
private:
    AM335xGPIO& gpio;
    std::vector<uint_fast64_t> deviceIDs;
    void findDevices();
};

#endif