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

AM335xADC::AM335xADC(unsigned char channels, int samples) {
    if (samples < 2) {
        throw std::invalid_argument("Minimum number of samples is 2.");
    }
    AM335xADC::samples = samples;

    std::ostringstream path;

    fd = open("/sys/bus/iio/devices/iio:device0/buffer/enable", O_WRONLY);
    if (fd < 0) {
        throw runtime_error("Could not disable continuous acquisition.");
    }
    pwrite(fd, "0\n", 2, 0);
    close(fd);

    for (int i = 0; i < 8; ++i) {
        calibration[i][0] = 0;
        calibration[i][4095] = 1.8;

        path.str(std::string());
        path << "/sys/bus/iio/devices/iio:device0/scan_elements/in_voltage" << i << "_en";
        fd = open(path.str().c_str(), O_WRONLY);
        if (fd < 0) {
            throw runtime_error("Could not set the channel to read.");
        }
        if (channels & 1<<i) {
            pwrite(fd, "1\n", 2, 0);
            values[i] = vector<unsigned int>(samples);
        } else {
            pwrite(fd, "0\n", 2, 0);
        }
        close(fd);
    }

    fd = open("/sys/bus/iio/devices/iio:device0/buffer/length", O_WRONLY);
    if (fd < 0) {
        throw runtime_error("Could not set the buffer length.");
    }
    path.str(std::string());
    path << values.size()*samples << "\n";
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

    bufferSize = 2*samples*values.size();
    buffer = new unsigned char[bufferSize];
    if (buffer == nullptr) {
        throw runtime_error("Could not allocate memory.");
    }

    sample();
}

AM335xADC::~AM335xADC() {
    close(fd);
    delete[] buffer;
}

void AM335xADC::sample() {
    unsigned char* buffer = AM335xADC::buffer;
    unsigned int remaining = bufferSize;

    averages.clear();
    standardErrors.clear();

    while (remaining > 0) {
        remaining -= pread(fd, buffer+bufferSize-remaining, remaining, 0);
    }

    for (std::map<int, vector<unsigned int>>::iterator i = values.begin(); i != values.end(); ++i) {
        for (int j = 0; j < samples; ++j) {
            i->second[j] = buffer[j*values.size()*2 + 1] << 8;
            i->second[j] += buffer[j*values.size()*2];
        }
        buffer += 2;
    }
}

const vector<unsigned int>& AM335xADC::getADCValues(int channel) {
    if (channel < 0 || channel > 7) {
        throw std::invalid_argument("Invalid channel number.");
    }

    if (values.find(channel) == values.end()) {
        throw runtime_error("The requested channel has not been enabled.");
    }

    return values[channel];
}

double AM335xADC::getAverage(int channel) {
    if (channel < 0 || channel > 7) {
        throw std::invalid_argument("Invalid channel number.");
    }

    if (values.find(channel) == values.end()) {
        throw runtime_error("The requested channel has not been enabled.");
    }

    if (averages.find(channel) == averages.end()) {
        double sum = 0;
        for (int i = 0; i < samples; ++i) {
            sum += convertADCValue(channel, values[channel][i]);
        }
        averages[channel] = sum / samples;
    }

    return averages[channel];
}

double AM335xADC::getStandardError(int channel) {
    if (channel < 0 || channel > 7) {
        throw std::invalid_argument("Invalid channel number.");
    }

    if (values.find(channel) == values.end()) {
        throw runtime_error("The requested channel has not been enabled.");
    }

    if (standardErrors.find(channel) == standardErrors.end()) {
        double sum = 0;
        double average = getAverage(channel);
        double diff;
        for (int i = 0; i < samples; ++i) {
            diff = convertADCValue(channel, values[channel][i])-average;
            sum += diff*diff;
        }
        standardErrors[channel] = sqrt(sum / (samples-1) / samples);
    }

    return standardErrors[channel];
}

void AM335xADC::addCalibPoint(int channel, double voltage, double adcValue) {
    if (channel < 0 || channel > 7) {
        throw std::invalid_argument("Invalid channel number.");
    }

    calibration[channel][adcValue] = voltage;
}

double AM335xADC::convertADCValue(int channel, double adcValue) {
    double x, y;
    std::map<double, double>::iterator i = calibration[channel].begin();

    while (i->first < adcValue) {
        ++i;
    }

    x = i->first;
    y = i->second;
    --i;

    return i->second + (y - i->second)/(x - i->first) * (adcValue - i->first);
}
