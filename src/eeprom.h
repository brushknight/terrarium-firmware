#ifndef TERRARIUM_EEPROM
#define TERRARIUM_EEPROM

#include <EEPROM.h>
#include "SparkFun_External_EEPROM.h"
#include "config.h"

namespace Eeprom
{
    void setup();
    bool isMemorySet();

    void clear();
    void clearClimate();
    void clearController();
    void saveControllerConfig(ControllerConfig config);
    void saveClimateConfig(ClimateConfig config);
    ClimateConfig loadClimateConfig();
    ControllerConfig loadControllerConfig();
    ControllerConfig loadControllerConfigFromESP32();
    ControllerConfig loadControllerConfigFromExternalEEPROM();
    bool isControllerConfigSetESP32();
    bool isControllerConfigSetExternalEEPROM();
    bool isClimateConfigSetExternalEEPROM();
    void saveClimateConfigTask(void *parameter);
}

#endif