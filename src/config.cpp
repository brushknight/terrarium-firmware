#include "config.h"

Config loadConfig()
{
    // load config from eeprom

    Config config = Config();

    config.climateZoneConfigs[0].isSet = true;

    config.climateZoneConfigs[0].dht22SensorPins[0] = DHT_0;
    config.climateZoneConfigs[0].dht22SensorPins[1] = DHT_1;
    config.climateZoneConfigs[0].dht22SensorPins[2] = DHT_2;

    config.climateZoneConfigs[0].relayPin = RELAY_0_PIN;

    config.climateZoneConfigs[0].schedule[0].sinceHour = 9;
    config.climateZoneConfigs[0].schedule[0].sinceMinute = 0;
    config.climateZoneConfigs[0].schedule[0].untilHour = 18;
    config.climateZoneConfigs[0].schedule[0].untilMinute = 0;
    config.climateZoneConfigs[0].schedule[0].temperature = 29;
    config.climateZoneConfigs[0].schedule[0].isSet = true;

    config.climateZoneConfigs[1].isSet = false;
    config.climateZoneConfigs[2].isSet = false;

    return config;
}
