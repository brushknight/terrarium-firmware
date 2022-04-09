#include "config.h"

Config loadConfig()
{
    // load config from eeprom

    Config config = Config();

    config.climateZoneConfigs[0].isSet = true;
    config.climateZoneConfigs[0].name = "hot zone";
    config.climateZoneConfigs[0].slug = "hot_zone";

    config.climateZoneConfigs[0].dht22SensorPins[0] = DHT_0;
    config.climateZoneConfigs[0].dht22SensorPins[1] = 0;
    config.climateZoneConfigs[0].dht22SensorPins[2] = 0;

    config.climateZoneConfigs[0].relayPin = RELAY_0_PIN;

    // night
    config.climateZoneConfigs[0].schedule[0].sinceHour = 18;
    config.climateZoneConfigs[0].schedule[0].sinceMinute = 0;
    config.climateZoneConfigs[0].schedule[0].untilHour = 8;
    config.climateZoneConfigs[0].schedule[0].untilMinute = 0;
    config.climateZoneConfigs[0].schedule[0].temperature = 23;
    config.climateZoneConfigs[0].schedule[0].isSet = true;
    // morning warming up 1

    config.climateZoneConfigs[0].schedule[1].sinceHour = 8;
    config.climateZoneConfigs[0].schedule[1].sinceMinute = 0;
    config.climateZoneConfigs[0].schedule[1].untilHour = 8;
    config.climateZoneConfigs[0].schedule[1].untilMinute = 30;
    config.climateZoneConfigs[0].schedule[1].temperature = 25;
    config.climateZoneConfigs[0].schedule[1].isSet = true;

    // morning warming up 2
    config.climateZoneConfigs[0].schedule[2].sinceHour = 8;
    config.climateZoneConfigs[0].schedule[2].sinceMinute = 30;
    config.climateZoneConfigs[0].schedule[2].untilHour = 9;
    config.climateZoneConfigs[0].schedule[2].untilMinute = 00;
    config.climateZoneConfigs[0].schedule[2].temperature = 27;
    config.climateZoneConfigs[0].schedule[2].isSet = true;

    // day
    config.climateZoneConfigs[0].schedule[3].sinceHour = 9;
    config.climateZoneConfigs[0].schedule[3].sinceMinute = 0;
    config.climateZoneConfigs[0].schedule[3].untilHour = 17;
    config.climateZoneConfigs[0].schedule[3].untilMinute = 00;
    config.climateZoneConfigs[0].schedule[3].temperature = 30;
    config.climateZoneConfigs[0].schedule[3].isSet = true;

    // evening cooling 1
    config.climateZoneConfigs[0].schedule[4].sinceHour = 17;
    config.climateZoneConfigs[0].schedule[4].sinceMinute = 0;
    config.climateZoneConfigs[0].schedule[4].untilHour = 17;
    config.climateZoneConfigs[0].schedule[4].untilMinute = 30;
    config.climateZoneConfigs[0].schedule[4].temperature = 27;
    config.climateZoneConfigs[0].schedule[4].isSet = true;

    // evening cooling 2
    config.climateZoneConfigs[0].schedule[4].sinceHour = 17;
    config.climateZoneConfigs[0].schedule[4].sinceMinute = 30;
    config.climateZoneConfigs[0].schedule[4].untilHour = 18;
    config.climateZoneConfigs[0].schedule[4].untilMinute = 0;
    config.climateZoneConfigs[0].schedule[4].temperature = 25;
    config.climateZoneConfigs[0].schedule[4].isSet = true;

    config.climateZoneConfigs[1].isSet = true;
    config.climateZoneConfigs[1].name = "cold zone";
    config.climateZoneConfigs[1].slug = "cold_zone";

    config.climateZoneConfigs[1].dht22SensorPins[0] = DHT_1;
    config.climateZoneConfigs[1].dht22SensorPins[1] = 0;
    config.climateZoneConfigs[1].dht22SensorPins[2] = 0;

    config.climateZoneConfigs[1].relayPin = RELAY_1_PIN;

    // constant 23
    config.climateZoneConfigs[1].schedule[0].sinceHour = 9;
    config.climateZoneConfigs[1].schedule[0].sinceMinute = 0;
    config.climateZoneConfigs[1].schedule[0].untilHour = 9;
    config.climateZoneConfigs[1].schedule[0].untilMinute = 0;
    config.climateZoneConfigs[1].schedule[0].temperature = 23;
    config.climateZoneConfigs[1].schedule[0].isSet = true;


    config.climateZoneConfigs[2].isSet = false;

    return config;
}
