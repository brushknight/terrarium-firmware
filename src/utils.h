#ifndef TERRARIUM_UTILS
#define TERRARIUM_UTILS

#include "Arduino.h"
#include <Wire.h>
#include <EEPROM.h>

namespace Utils
{
    std::string readWiFiSSIDFromMemory();
    std::string readWiFiPassFromMemory();
    bool isMemorySet();
    void setMemory();
    void resetMemory();
    void writeWiFiSSIDToMemory(std::string ssid);
    void writeWiFiPassToMemory(std::string pass);
    void log(const char str[]);
    void scanForI2C();
    void TCA9548A(uint8_t bus);
}

#endif