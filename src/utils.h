#ifndef TERRARIUM_UTILS
#define TERRARIUM_UTILS

#include "Arduino.h"
#include <Wire.h>
#include <EEPROM.h>
#include <string>
#include "time.h"

namespace Utils
{
    void log(const char str[]);
    void scanForI2C();
    void TCA9548A(uint8_t bus, bool verbose);
    std::string hourMinuteToString(int hour, int minute);
    bool checkScheduleTimeWindow(std::string now, std::string since, std::string until);
    unsigned long getTimestamp();
}

#endif