#include <stdint.h>
#include <time.h>

#include "DS18B20.h"
#include "Maxim1WireBus.h"

DS18B20::DS18B20(unsigned long long deviceID, Maxim1WireBus& bus) : bus(bus) {
    DS18B20::deviceID = deviceID;
}

void DS18B20::convertT(Maxim1WireBus& bus) {
    static timespec t;
    t.tv_sec = 0;
    t.tv_nsec = 100*1000*1000;

    bus.reset();
    bus.write(0xCC, 8);  // SKIP ROM command
    bus.write(0x44, 8);  // CONVERT T command
    while (!bus.read()) {
        /* waiting for all devices to end */
        nanosleep(&t, NULL);
    }

    return;
}

double DS18B20::getMeasuredTemp() {
    uint16_t temperature = 0;

    bus.reset();
    bus.write(0x55, 8);  // MATCH ROM command
    bus.write(deviceID, 64);
    bus.write(0xBE, 8);  // READ SCRATCHPAD command
    for (int i = 0; i < 16; ++i) {
        temperature += bus.read() << i;
    }

    return (int16_t)temperature / 16.;
}
