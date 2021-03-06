#ifndef TERRARIUM_UTILS
#define TERRARIUM_UTILS

#include "Arduino.h"
#include <Wire.h>
#include <EEPROM.h>
#include <string>

namespace Utils
{
    void log(const char str[]);
    void scanForI2C();
    void TCA9548A(uint8_t bus);
    std::string hourMinuteToString(int hour, int minute);
}

#endif