#ifndef TERRARIUM_EEPROM
#define TERRARIUM_EEPROM

#include <EEPROM.h>
#include "SparkFun_External_EEPROM.h"
#include "config.h"

namespace Eeprom
{
    std::string readWiFiSSIDFromMemory();
    std::string readWiFiPassFromMemory();
    std::string readIDFromMemory();
    void setup();
    bool isMemorySet();
    void setMemory();
    void resetMemory();
    void writeWiFiSSIDToMemory(std::string ssid);
    void writeWiFiPassToMemory(std::string pass);
    void writeIDToMemory(std::string id);
    void saveConfig(Config config);
    Config loadConfig();
}

#endif