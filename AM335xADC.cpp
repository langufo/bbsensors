#include <fcntl.h>
#include <iostream>
#include <iterator>
#include <math.h>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <vector>

#include "AM335xADC.h"

using std::runtime_error;
using std::vector;

AM335xADC::AM335xADC(CHN channel, unsigned int samples) {
    AM335xADC::samples = samples;

    std::ostringstream path;

    fd = open("/sys/bus/iio/devices/iio:device0/buffer/enable", O_WRONLY);
    if (fd < 0) {
        throw runtime_error("Could not disable continuous acquisition.");
    }
    pwrite(fd, "0\n", 2, 0);
    close(fd);

    for (int i = 0; i < 8; ++i) {
        path.str(std::string());
        path << "/sys/bus/iio/devices/iio:device0/scan_elements/in_voltage" << i << "_en";
        fd = open(path.str().c_str(), O_WRONLY);
        if (fd < 0) {
            throw runtime_error("Could not set the channel to read.");
        }
        if (channel & 1<<i) {
            index[i] = index.size()-1;
            pwrite(fd, "1\n", 2, 0);
        } else {
            pwrite(fd, "0\n", 2, 0);
        }
        close(fd);
    }

    nChannels = index.size();

    fd = open("/sys/bus/iio/devices/iio:device0/buffer/length", O_WRONLY);
    if (fd < 0) {
        throw runtime_error("Could not set the buffer length.");
    }
    path.str(std::string());
    path << nChannels*samples << "\n";
    pwrite(fd, path.str().c_str(), path.str().length(), 0);
    close(fd);

    fd = open("/sys/bus/iio/devices/iio:device0/buffer/enable", O_WRONLY);
    if (fd < 0) {
        throw runtime_error("Could not enable continuous acquisition.");
    }
    pwrite(fd, "1\n", 2, 0);
    close(fd);

    fd = open("/dev/iio:device0", O_RDONLY);
    if (fd < 0) {
        throw runtime_error("Could not read AM335xADC data.");
    }

    data = new unsigned char[nChannels*samples*2];
    if (data == nullptr) {
        throw runtime_error("Could not allocate memory.");
    }

    values.resize(nChannels);
    for (int i = 0; i < nChannels; ++i) {
        values[i].resize(samples);
    }
}

AM335xADC::~AM335xADC() {
    close(fd);
}

void AM335xADC::sample() {
    int total = samples*nChannels*2;
    int remaining = total;

    while (remaining > 0) {
        remaining -= pread(fd, data+total-remaining, remaining, 0);
    }

    for (int i = 0; i < samples; ++i) {
        for (int j = 0; j < nChannels; ++j) {
            values[j][i] = data[(i*nChannels+j)*2 + 1] << 8;
            values[j][i] += data[(i*nChannels+j)*2];
        }
    }
}

const std::vector<unsigned int>& AM335xADC::getADCValues(unsigned int channel) {
    if (channel > 7) {
        throw std::invalid_argument("Invalid channel number.");
    }

    if (index.find(channel) == index.end()) {
        throw runtime_error("The requested channel has not been enabled.");
    }

    return values[index[channel]];
}

double AM335xADC::getAverage(unsigned int channel) {
    if (channel > 7) {
        throw std::invalid_argument("Invalid channel number.");
    }

    if (index.find(channel) == index.end()) {
        throw runtime_error("The requested channel has not been enabled.");
    }

    unsigned long long sum = 0;
    for (int i = 0; i < samples; ++i) {
        sum += values[index[channel]][i];
    }

    return sum / (double)samples;
}

double AM335xADC::getStandardError(unsigned int channel) {
    if (channel > 7) {
        throw std::invalid_argument("Invalid channel number.");
    }

    if (index.find(channel) == index.end()) {
        throw runtime_error("The requested channel has not been enabled.");
    }

    double sum = 0;
    double average = getAverage(channel);
    for (int i = 0; i < samples; ++i) {
        sum += pow(values[index[channel]][i]-average, 2);
    }

    return sqrt(sum / (samples-1) / samples);
}
