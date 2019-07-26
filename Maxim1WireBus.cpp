#include <fcntl.h>
#include <iostream>
#include <set>
#include <stdexcept>
#include <termios.h>
#include <unistd.h>

#include "Maxim1WireBus.h"

using std::cerr;
using std::invalid_argument;
using std::runtime_error;
using std::set;


Maxim1WireBus::Maxim1WireBus(const char* ttyPath) {
    ttyFile = open(ttyPath, O_RDWR);

    if (ttyFile < 0) {  /* invalid path */
        throw new runtime_error("Could not open the device");
    }
    if (!isatty(ttyFile)) {
        throw new runtime_error("The device is not a serial terminal");
    }

    tcflush(ttyFile, TCIOFLUSH);

    tcgetattr(ttyFile, &ttyConfig);
    ttyConfig.c_oflag &= !OPOST;
    ttyConfig.c_lflag &= !ICANON;
    ttyConfig.c_cc[VMIN] = 1;  /* to always read at least one byte */
    ttyConfig.c_cc[VTIME] = 0;
}

Maxim1WireBus::~Maxim1WireBus() {
    close(ttyFile);
}

int Maxim1WireBus::read() {
    const unsigned char tx = 0xFF;
    unsigned char rx;

    cfsetspeed(&ttyConfig, B115200);
    tcsetattr(ttyFile, TCSAFLUSH, &ttyConfig);

    ::write(ttyFile, &tx, 1);
    ::read(ttyFile, &rx, 1);
    
    return rx == 0xFF;
}

void Maxim1WireBus::write(unsigned long long data, int bits) {
    unsigned char tx;

    cfsetspeed(&ttyConfig, B115200);
    tcsetattr(ttyFile, TCSAFLUSH, &ttyConfig);

    if (bits < 0 || bits > 64) {
        throw new invalid_argument("Invalid number of bits to write");
    }

    do {
        if ((data & 1) == 0) {
            tx = 0x00;
        } else {
            tx = 0xFF;
        }
        
        ::write(ttyFile, &tx, 1);

        data >>= 1;
        --bits;
    } while (bits > 0);

    tcdrain(ttyFile);

    /* 
     * By sending tx, the same byte is read by the UART receiver and stored in 
     * the input buffer. To prevent this byte to be returned in the next read 
     * call the input buffer needs to be flushed.
     */
    tcflush(ttyFile, TCIFLUSH);
}

int Maxim1WireBus::reset() {
    const unsigned char tx = 0xF0;
    unsigned char rx;
    
    cfsetspeed(&ttyConfig, B9600);
    tcsetattr(ttyFile, TCSAFLUSH, &ttyConfig);

    ::write(ttyFile, &tx, 1);
    ::read(ttyFile, &rx, 1);
    
    return rx >= 0x90 && rx <= 0xE0;
}

set<unsigned long long> Maxim1WireBus::findDevices() {
    set<unsigned long long> deviceIDs;

    unsigned long long code = 0;
    unsigned long long onesMask = 0;
    int i, k;
    bool zero, one;

    k = -1;  // to find the first ROM code
    if (reset() == 0) {
        return deviceIDs;
    }
    do {
        reset();
        write(0xF0, 8);
        for (i = 0; i < 63; ++i) {
            if (i < k) {  /* copying last found code */
                read();
                read();
                write(code >> i, 1);
            } else if (i == k) {  /* switch to new path */
                read();
                read();
                code |= 1ULL<<i;
                write(1);
            } else {  /* new path exploration */
                zero = !read();
                one = !read();
                if (one) {
                    onesMask |= 1ULL<<i;
                    if (!zero) {
                        code |= 1ULL<<i;
                    }
                } else if (!zero) {
                    cerr << "A device seems to have vanished during discovery. Starting again.\n";
                    return findDevices();
                }
                write((code & 1ULL<<i) != 0);
            }
        }
        zero = !read();
        one = !read();
        if (!zero && !one) {
            cerr << "A device seems to have vanished during discovery. Starting again.\n";
            return findDevices();
        } else {
            if (zero) {
                deviceIDs.insert(code);
            }
            if (one) {
                deviceIDs.insert(code + (1ULL<<63));
            }
        }

        if (code != onesMask) {
            /* deciding how many bits to keep of the last found code */
            for (k = 62; (code^onesMask) >> k == 0; --k);
            /* clearing all the other bits */
            code = code << 63-k >> 63-k;
            onesMask = onesMask << 63-k >> 63-k;
        }

    } while (code != onesMask);
    
    return deviceIDs;
}
