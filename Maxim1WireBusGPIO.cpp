#include <fcntl.h>
#include <stdint.h>
#include <string>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <vector>

#include "AM335xGPIO.h"
#include "Maxim1WireBusGPIO.h"

Maxim1WireBus::Maxim1WireBus(AM335xGPIO& gpio) : gpio(gpio) {
    findDevices();
}

std::vector<uint_fast64_t> Maxim1WireBus::getDeviceIDs() {
    return deviceIDs;
}

int Maxim1WireBus::read() {
    timespec start, split;
    int answer;
    int buffer = 1;

    /* issuing a read time slot */
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    gpio.write(0);
    do {
        clock_gettime(CLOCK_MONOTONIC_RAW, &split);
    } while ((split.tv_sec-start.tv_sec)*1000000 + (split.tv_nsec-start.tv_nsec)/1000 < 1);

    /* reading the bus */
    do {
        answer = buffer;
        buffer = gpio.read();
        clock_gettime(CLOCK_MONOTONIC_RAW, &split);
    } while ((split.tv_sec-start.tv_sec)*1000000 + (split.tv_nsec-start.tv_nsec)/1000 < 15);

    /* waiting for read time slot to end */
    do {
        clock_gettime(CLOCK_MONOTONIC_RAW, &split);
    } while ((split.tv_sec-start.tv_sec)*1000000 + (split.tv_nsec-start.tv_nsec)/1000 < 60+1);

    return answer;
}

void Maxim1WireBus::write(uint_fast64_t data, int bits) {
    timespec start, split;

    if (bits < 1 || bits > 64) {
        // TODO throw exception
    } else if (bits == 1 && data % 2 == 0) {
        gpio.write(0);
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        do {
            clock_gettime(CLOCK_MONOTONIC_RAW, &split);
        } while ((split.tv_sec-start.tv_sec)*1000000 + (split.tv_nsec-start.tv_nsec)/1000 < 60);
        gpio.read();
        do {
            clock_gettime(CLOCK_MONOTONIC_RAW, &split);
        } while ((split.tv_sec-start.tv_sec)*1000000 + (split.tv_nsec-start.tv_nsec)/1000 < 60+1+10);

        return;
    } else if (bits == 1 && data % 2 == 1) {
        gpio.write(0);
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        do {
            clock_gettime(CLOCK_MONOTONIC_RAW, &split);
        } while ((split.tv_sec-start.tv_sec)*1000000 + (split.tv_nsec-start.tv_nsec)/1000 < 1);
        gpio.read();
        do {
            clock_gettime(CLOCK_MONOTONIC_RAW, &split);
        } while ((split.tv_sec-start.tv_sec)*1000000 + (split.tv_nsec-start.tv_nsec)/1000 < 60+1);

        return;
    } else {
        write(data % 2);
        write(data/2, bits-1);
        return;
    }
}

int Maxim1WireBus::reset() {
    timespec start, split;
    timespec t;
    int buffer = 1;
    int answer;

    gpio.write(0);
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    do {
        clock_gettime(CLOCK_MONOTONIC_RAW, &split);
    } while ((split.tv_sec-start.tv_sec)*1000000 + (split.tv_nsec-start.tv_nsec)/1000 < 480);

    /* detecting presence pulse */
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    do {
        answer = buffer;
        buffer = gpio.read();
        clock_gettime(CLOCK_MONOTONIC_RAW, &split);
    } while ((split.tv_sec-start.tv_sec)*1000000 + (split.tv_nsec-start.tv_nsec)/1000 < 75);

    t.tv_sec = 0;
    t.tv_nsec = 405*1000;
    nanosleep(&t, NULL);

    return !answer;
}

void Maxim1WireBus::findDevices() {
    const uint_fast64_t delta = 1;

    uint_fast64_t code, onesMask;
    int i, j, k;
    int zero, one;


    /* finding a first ROM code */
    code = 0;
    onesMask = 0;
    if (reset()) {
        // TODO no device answering case
    }
    write(0xF0, 8);
    for (i = 0; i < 63; ++i) {
        zero = !read();
        one = !read();
        if (one) {
            onesMask += delta<<i;
            if (!zero) {
                code += delta<<i;
            }
        } else if (!zero) {
            // TODO device vanished?
        }
        write((code & delta<<i) != 0);
    }
    zero = !read();
    one = !read();
    if (!zero && !one) {
        // TODO device vanished?
    } else {
        if (zero) {
            deviceIDs.push_back(code);
        }
        if (one) {
            deviceIDs.push_back(code + (delta<<63));
        }
    }

    /* finding all the other codes */
    while (code != onesMask) {
        for (k = 62; (code^onesMask) >> k == 0; --k);  // 63rd bit is always a 0
        code = code << 63-k >> 63-k;
        code += delta<<k;
        onesMask = onesMask << 63-k >> 63-k;
        reset();
        write(0xF0, 8);
        for (i = 0; i < 63; ++i) {
            if (i <= k) {
                read();
                read();
                write(code >> i, 1);
            } else {
                zero = !read();
                one = !read();
                if (one) {
                    onesMask += delta<<i;
                    if (!zero) {
                        code += delta<<i;
                    }
                } else if (!zero) {
                    // TODO device vanished?
                }
                write((code & delta<<i) != 0);
            }
        }
        zero = !read();
        one = !read();
        if (!zero && !one) {
            // TODO device vanished?
        } else {
            if (zero) {
                deviceIDs.push_back(code);
            }
            if (one) {
                deviceIDs.push_back(code + (delta<<63));
            }
        }
        reset();
    }
}