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
#define SYNC_RTC_SEC 60 * 5

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
#define RELAY_0_PIN 5
#define RELAY_1_PIN 4
#define RELAY_2_PIN 25

// lights
#define LEDPIN 23

// buttons
#define BUTTON_RESET_EEPROM 15

const int SENSOR_PINS[6] = {DHT_0, DHT_1, DHT_2, DHT_3, DHT_4, DHT_5};
const int I2C_BUSES[6] = {3, 0, 1, 2, 4, 5};
const int RELAY_PINS[3] = {RELAY_0_PIN, RELAY_1_PIN, RELAY_2_PIN};


class SystemConfig
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
    static SystemConfig fromJSON(std::string json)
    {
        SystemConfig config;

        DynamicJsonDocument doc(jsonSize());
        deserializeJson(doc, json);

        config.wifiSSID = doc["wifiSSID"].as<std::string>();
        config.wifiPassword = doc["wifiPassword"].as<std::string>();
        config.id = doc["id"].as<std::string>();

        return config;
    }
};

#endif