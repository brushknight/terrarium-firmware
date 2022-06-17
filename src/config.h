#ifndef CONFIG
#define CONFIG

#include <string>
#include "ArduinoJson.h"

//#define SENSORS_COUNT 2
// #define TERRARIUM_ID 1

//#define VERSION "v0.0.0-dev"

//#define DEBUG false

//#define DISPLAY_ENABLED true
//#define RTC_ENABLED true

//#define SERVER_ENABLED true

//#define STATUS_LED_ENABLED true

//#define SENSORS_ENABLED true

//#define TEST_BOARD false
#define DEMO_BOARD false

#define EXTERNAL_EEPROM true

#define MAX_DHT22_SENSORS_IN_CLIMATE_ZONE int(3)
#define MAX_CLIMATE_ZONES int(3)
#define MAX_LIGHT_EVENTS int(10)
#define MAX_SENSOR_RETRY int(3)
#define MAX_SCHEDULE_COUNT int(10)

#define CLIMATE_LOOP_INTERVAL_SEC 5
#define LIGHT_LOOP_INTERVAL_SEC 5
#define DISPLAY_RENDER_INTERVAL_SEC 1
#define BATTERY_CHECK_INTERVAL_SEC 60 * 5

// Hardware
#define SENSORS_ENABLE_PIN 32

// DHT22 sensors pins
#define DHT_0 int(16) // #0
#define DHT_1 int(17) // #1
#define DHT_2 int(18) // #2
#define DHT_3 int(19) // #3
#define DHT_4 int(27) // #4
#define DHT_5 int(26) // #5
// relay pins
#define RELAY_0_PIN 4
#define RELAY_1_PIN 25
#define RELAY_2_PIN 5

class Schedule
{
public:
    int sinceHour;
    int sinceMinute;
    int untilHour;
    int untilMinute;
    float temperature;
    bool isSet = false;
    static int jsonSize()
    {
        return 100;
    }
    DynamicJsonDocument toJSON()
    {
        DynamicJsonDocument doc(jsonSize());

        doc["sinceHour"] = sinceHour;
        doc["sinceMinute"] = sinceMinute;
        doc["untilHour"] = untilHour;
        doc["untilMinute"] = untilMinute;
        doc["temperature"] = temperature;
        doc["isSet"] = isSet;

        return doc;
    }
    static Schedule fromJSON(DynamicJsonDocument doc)
    {
        Schedule schedule;

        schedule.sinceHour = doc["sinceHour"];
        schedule.sinceMinute = doc["sinceMinute"];
        schedule.untilHour = doc["untilHour"];
        schedule.untilMinute = doc["untilMinute"];
        schedule.temperature = doc["temperature"];
        schedule.isSet = doc["isSet"];

        return schedule;
    }
};

class ClimateZoneConfig
{
public:
    std::string name;
    std::string slug;
    int relayPin = 0;
    int dht22SensorPins[MAX_DHT22_SENSORS_IN_CLIMATE_ZONE] = {0, 0, 0};
    bool isSet = false;
    Schedule schedule[MAX_SCHEDULE_COUNT];
    static int jsonSize()
    {
        return (200 + Schedule::jsonSize() * MAX_SCHEDULE_COUNT) * MAX_CLIMATE_ZONES;
    }
    DynamicJsonDocument toJSON()
    {
        DynamicJsonDocument doc(jsonSize());

        doc["name"] = name;
        doc["slug"] = slug;
        doc["relayPin"] = relayPin;
        for (int j = 0; j < MAX_DHT22_SENSORS_IN_CLIMATE_ZONE; j++)
        {
            doc["dht22SensorPins"][j] = dht22SensorPins[j];
        }

        doc["isSet"] = isSet;
        for (int k = 0; k < MAX_SCHEDULE_COUNT; k++)
        {
            doc["schedule"][k] = schedule[k].toJSON();
        }

        return doc;
    }
    static ClimateZoneConfig fromJSON(DynamicJsonDocument doc)
    {
        ClimateZoneConfig climateZoneConfig;
        climateZoneConfig.name = doc["name"].as<std::string>();
        climateZoneConfig.slug = doc["slug"].as<std::string>();
        climateZoneConfig.relayPin = doc["relayPin"];
        climateZoneConfig.isSet = doc["isSet"];

        for (int i = 0; i < MAX_DHT22_SENSORS_IN_CLIMATE_ZONE; i++)
        {
            climateZoneConfig.dht22SensorPins[i] = doc["dht22SensorPins"][i];
        }

        for (int j = 0; j < MAX_SCHEDULE_COUNT; j++)
        {
            climateZoneConfig.schedule[j] = Schedule::fromJSON(doc["schedule"][j]);
        }
        return climateZoneConfig;
    }
};

class LightEvent
{
public:
    std::string name;
    std::string since;
    std::string until;
    int relay;
    int intencity;   // 0 - 100
    int durationSec; // duration in seconds
    bool isSet()
    {
        return relay > 0;
    }
    static int jsonSize()
    {
        return 192;
    }
    DynamicJsonDocument toJSON()
    {
        DynamicJsonDocument doc(jsonSize());

        doc["name"] = name;
        doc["since"] = since;
        doc["until"] = until;
        doc["relay"] = relay;
        doc["intencity"] = intencity;
        doc["durationSec"] = durationSec;

        return doc;
    }
    static LightEvent fromJSON(DynamicJsonDocument doc)
    {
        LightEvent event;

        event.name = doc["name"].as<std::string>();
        event.since = doc["since"].as<std::string>();
        event.until = doc["until"].as<std::string>();
        event.relay = doc["relay"];
        event.intencity = doc["intencity"];
        event.durationSec = doc["durationSec"];

        return event;
    }
};

class ClimateConfig
{
public:
    ClimateZoneConfig climateZoneConfigs[MAX_CLIMATE_ZONES];
    LightEvent lightEvents[MAX_LIGHT_EVENTS];

    static int jsonSize()
    {
        return ClimateZoneConfig::jsonSize() * MAX_CLIMATE_ZONES + LightEvent::jsonSize() * MAX_LIGHT_EVENTS;
    }
    DynamicJsonDocument toJSON()
    {
        DynamicJsonDocument doc(jsonSize());
        for (int i = 0; i < MAX_CLIMATE_ZONES; i++)
        {
            doc["climateZoneConfigs"][i] = climateZoneConfigs[i].toJSON();
        }
        for (int i = 0; i < MAX_LIGHT_EVENTS; i++)
        {
            doc["lightEvents"][i] = lightEvents[i].toJSON();
        }
        return doc;
    }
    static ClimateConfig fromJSON(std::string json)
    {
        ClimateConfig config;

        DynamicJsonDocument doc(jsonSize());
        deserializeJson(doc, json);

        for (int i = 0; i < MAX_CLIMATE_ZONES; i++)
        {
            config.climateZoneConfigs[i] = ClimateZoneConfig::fromJSON(doc["climateZoneConfigs"][i]);
        }
        for (int i = 0; i < MAX_LIGHT_EVENTS; i++)
        {
            config.lightEvents[i] = LightEvent::fromJSON(doc["lightEvents"][i]);
        }
        return config;
    }
};

class ControllerConfig
{
public:
    std::string wifiSSID;
    std::string wifiPassword;
    std::string id;

    static int jsonSize()
    {
        return 128;
    }
    DynamicJsonDocument toJSON()
    {
        DynamicJsonDocument doc(jsonSize());

        doc["wifiSSID"] = wifiSSID;
        doc["wifiPassword"] = wifiPassword;
        doc["id"] = id;
        return doc;
    }
    static ControllerConfig fromJSON(std::string json)
    {
        ControllerConfig config;

        DynamicJsonDocument doc(jsonSize());
        deserializeJson(doc, json);

        config.wifiSSID = doc["wifiSSID"].as<std::string>();
        config.wifiPassword = doc["wifiPassword"].as<std::string>();
        config.id = doc["id"].as<std::string>();

        return config;
    }
};

ClimateConfig loadInitClimateConfig();

#endif