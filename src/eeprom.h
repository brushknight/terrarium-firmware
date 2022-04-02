#ifndef TERRARIUM_EEPROM
#define TERRARIUM_EEPROM

#include <EEPROM.h>

namespace Eeprom
{
    std::string readWiFiSSIDFromMemory();
    std::string readWiFiPassFromMemory();
    bool isMemorySet();
    void setMemory();
    void resetMemory();
    void writeWiFiSSIDToMemory(std::string ssid);
    void writeWiFiPassToMemory(std::string pass);

}

#endif