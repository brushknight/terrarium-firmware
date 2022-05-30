#include "config.h"




Config loadInitConfig()
{
    // load config from eeprom

    Config config = Config();

    config.climateZoneConfigs[0].isSet = true;
    config.climateZoneConfigs[0].name = "hot zone";
    config.climateZoneConfigs[0].slug = "hot_zone";

    config.climateZoneConfigs[0].dht22SensorPins[0] = DHT_0;
    // config.climateZoneConfigs[0].dht22SensorPins[1] = DHT_1; // 4 sensors terr
    config.climateZoneConfigs[0].dht22SensorPins[1] = 0;
    config.climateZoneConfigs[0].dht22SensorPins[2] = 0;

    config.climateZoneConfigs[0].relayPin = RELAY_0_PIN;

    int hourOffset = -2;

    // night + base 23
    config.climateZoneConfigs[1].schedule[0].sinceHour = 3 + hourOffset;
    config.climateZoneConfigs[1].schedule[0].sinceMinute = 0;
    config.climateZoneConfigs[1].schedule[0].untilHour = 3 + hourOffset;
    config.climateZoneConfigs[1].schedule[0].untilMinute = 0;
    config.climateZoneConfigs[0].schedule[0].temperature = 23;
    config.climateZoneConfigs[0].schedule[0].isSet = true;

    // morning warming up 1
    config.climateZoneConfigs[0].schedule[1].sinceHour = 7 + hourOffset;
    config.climateZoneConfigs[0].schedule[1].sinceMinute = 0;
    config.climateZoneConfigs[0].schedule[1].untilHour = 7 + hourOffset;
    config.climateZoneConfigs[0].schedule[1].untilMinute = 30;
    config.climateZoneConfigs[0].schedule[1].temperature = 25;
    config.climateZoneConfigs[0].schedule[1].isSet = true;

    // morning warming up 2
    config.climateZoneConfigs[0].schedule[2].sinceHour = 7 + hourOffset;
    config.climateZoneConfigs[0].schedule[2].sinceMinute = 30;
    config.climateZoneConfigs[0].schedule[2].untilHour = 8 + hourOffset;
    config.climateZoneConfigs[0].schedule[2].untilMinute = 0;
    config.climateZoneConfigs[0].schedule[2].temperature = 27;
    config.climateZoneConfigs[0].schedule[2].isSet = true;

    // day
    config.climateZoneConfigs[0].schedule[3].sinceHour = 8 + hourOffset;
    config.climateZoneConfigs[0].schedule[3].sinceMinute = 0;
    config.climateZoneConfigs[0].schedule[3].untilHour = 20 + hourOffset;
    config.climateZoneConfigs[0].schedule[3].untilMinute = 0;
    config.climateZoneConfigs[0].schedule[3].temperature = 29;
    config.climateZoneConfigs[0].schedule[3].isSet = true;

    // evening cooling 1
    config.climateZoneConfigs[0].schedule[4].sinceHour = 20 + hourOffset;
    config.climateZoneConfigs[0].schedule[4].sinceMinute = 0;
    config.climateZoneConfigs[0].schedule[4].untilHour = 20 + hourOffset;
    config.climateZoneConfigs[0].schedule[4].untilMinute = 30;
    config.climateZoneConfigs[0].schedule[4].temperature = 27;
    config.climateZoneConfigs[0].schedule[4].isSet = true;

    // evening cooling 2
    config.climateZoneConfigs[0].schedule[5].sinceHour = 20 + hourOffset;
    config.climateZoneConfigs[0].schedule[5].sinceMinute = 30;
    config.climateZoneConfigs[0].schedule[5].untilHour = 21 + hourOffset;
    config.climateZoneConfigs[0].schedule[5].untilMinute = 0;
    config.climateZoneConfigs[0].schedule[5].temperature = 25;
    config.climateZoneConfigs[0].schedule[5].isSet = true;

    // test schedule

    // // night
    // config.climateZoneConfigs[0].schedule[0].sinceHour = 15;
    // config.climateZoneConfigs[0].schedule[0].sinceMinute = 15;
    // config.climateZoneConfigs[0].schedule[0].untilHour = 12;
    // config.climateZoneConfigs[0].schedule[0].untilMinute = 15;
    // config.climateZoneConfigs[0].schedule[0].temperature = 23;
    // config.climateZoneConfigs[0].schedule[0].isSet = true;
    // // morning warming up 1

    // config.climateZoneConfigs[0].schedule[1].sinceHour = 12;
    // config.climateZoneConfigs[0].schedule[1].sinceMinute = 15;
    // config.climateZoneConfigs[0].schedule[1].untilHour = 12;
    // config.climateZoneConfigs[0].schedule[1].untilMinute = 45;
    // config.climateZoneConfigs[0].schedule[1].temperature = 25;
    // config.climateZoneConfigs[0].schedule[1].isSet = true;

    // // morning warming up 2
    // config.climateZoneConfigs[0].schedule[2].sinceHour = 12;
    // config.climateZoneConfigs[0].schedule[2].sinceMinute = 45;
    // config.climateZoneConfigs[0].schedule[2].untilHour = 13;
    // config.climateZoneConfigs[0].schedule[2].untilMinute = 15;
    // config.climateZoneConfigs[0].schedule[2].temperature = 27;
    // config.climateZoneConfigs[0].schedule[2].isSet = true;

    // // day
    // config.climateZoneConfigs[0].schedule[3].sinceHour = 13;
    // config.climateZoneConfigs[0].schedule[3].sinceMinute = 15;
    // config.climateZoneConfigs[0].schedule[3].untilHour = 14;
    // config.climateZoneConfigs[0].schedule[3].untilMinute = 15;
    // config.climateZoneConfigs[0].schedule[3].temperature = 30;
    // config.climateZoneConfigs[0].schedule[3].isSet = true;

    // // evening cooling 1
    // config.climateZoneConfigs[0].schedule[4].sinceHour = 14;
    // config.climateZoneConfigs[0].schedule[4].sinceMinute = 15;
    // config.climateZoneConfigs[0].schedule[4].untilHour = 14;
    // config.climateZoneConfigs[0].schedule[4].untilMinute = 45;
    // config.climateZoneConfigs[0].schedule[4].temperature = 27;
    // config.climateZoneConfigs[0].schedule[4].isSet = true;

    // // evening cooling 2
    // config.climateZoneConfigs[0].schedule[4].sinceHour = 14;
    // config.climateZoneConfigs[0].schedule[4].sinceMinute = 45;
    // config.climateZoneConfigs[0].schedule[4].untilHour = 15;
    // config.climateZoneConfigs[0].schedule[4].untilMinute = 15;
    // config.climateZoneConfigs[0].schedule[4].temperature = 25;
    // config.climateZoneConfigs[0].schedule[4].isSet = true;

    config.climateZoneConfigs[1].isSet = true;
    config.climateZoneConfigs[1].name = "cold zone";
    config.climateZoneConfigs[1].slug = "cold_zone";

    config.climateZoneConfigs[1].dht22SensorPins[0] = DHT_1;
    // config.climateZoneConfigs[1].dht22SensorPins[0] = DHT_2; // 4 sensors terr
    // config.climateZoneConfigs[1].dht22SensorPins[1] = DHT_3; // 4 sensors terr
    config.climateZoneConfigs[1].dht22SensorPins[1] = 0;
    config.climateZoneConfigs[1].dht22SensorPins[2] = 0;

    config.climateZoneConfigs[1].relayPin = RELAY_1_PIN;

    // constant 23
    config.climateZoneConfigs[1].schedule[0].sinceHour = 9 + hourOffset;
    config.climateZoneConfigs[1].schedule[0].sinceMinute = 0;
    config.climateZoneConfigs[1].schedule[0].untilHour = 9 + hourOffset;
    config.climateZoneConfigs[1].schedule[0].untilMinute = 0;
    config.climateZoneConfigs[1].schedule[0].temperature = 23;
    config.climateZoneConfigs[1].schedule[0].isSet = true;

    config.climateZoneConfigs[2].isSet = false;

    return config;
}
