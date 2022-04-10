#ifndef TERRARIUM_EEPROM
#define TERRARIUM_EEPROM

#include <EEPROM.h>

namespace Eeprom
{
    std::string readWiFiSSIDFromMemory();
    std::string readWiFiPassFromMemory();
    std::string readIDFromMemory();
    bool isMemorySet();
    void setMemory();
    void resetMemory();
    void writeWiFiSSIDToMemory(std::string ssid);
    void writeWiFiPassToMemory(std::string pass);
    void writeIDToMemory(std::string id);

}

#endif