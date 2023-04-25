#ifndef TERRARIUM_EEPROM
#define TERRARIUM_EEPROM

#include <EEPROM.h>
#include "SparkFun_External_EEPROM.h"
#include "config.h"
#include "zone.h"
#include "status.h"

namespace Eeprom
{
    static const char *TAG = "eeprom";

    void setup(ExternalEEPROM *extEEPROM);
    bool isMemorySet();
    void clear();
    void clearZoneController();
    void clearZoneControllerFull();
    void clearSystemSettings();
    void saveSystemConfig(SystemConfig config);
    // void updateZoneControllerFromJson(std::string *json);
    void saveZoneController();
    void saveZoneControllerJSON(Zone::Controller *zoneController);
    // Zone::Controller *loadZoneController();
    SystemConfig loadSystemConfig();
    SystemConfig loadSystemConfigFromESP32();
    SystemConfig loadSystemConfigFromExternalEEPROM();
    bool isSystemConfigSetESP32();
    bool isSystemConfigSetExternalEEPROM();
    bool isZoneControllerSetExternalEEPROM();
    void saveZoneControllerTask(void *parameter);
    bool resetEepromChecker();

    // refactoring
    std::string loadZoneControllerJSON();
}

#endif