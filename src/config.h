#ifndef CONFIG
#define CONFIG

#include <string>
#include "ArduinoJson.h"

//#define EXTERNAL_EEPROM true

#define CLIMATE_LOOP_INTERVAL_SEC 5
#define LIGHT_LOOP_INTERVAL_SEC 5
#define DISPLAY_RENDER_INTERVAL_SEC 1
#define BATTERY_CHECK_INTERVAL_SEC 60 * 5
#define SYNC_RTC_SEC 60 * 5

// Hardware
#define SENSORS_ENABLE_PIN 32

// DHT22 sensors pins
#define ONE_WIRE_0 int(16) 
#define ONE_WIRE_1 int(17) 
#define ONE_WIRE_2 int(18) 
#define ONE_WIRE_3 int(19) 
#define ONE_WIRE_4 int(27) 
#define ONE_WIRE_5 int(26) 

// I2c Busses ports
#define I2C_BUS_0 int(3) 
#define I2C_BUS_1 int(0) 
#define I2C_BUS_2 int(1) 
#define I2C_BUS_3 int(2) 
#define I2C_BUS_4 int(4) 
#define I2C_BUS_5 int(5) 

// relay pins
#define RELAY_0_PIN 5
#define RELAY_1_PIN 4
#define RELAY_2_PIN 25

// lights
#define LEDPIN 32
#define STATUS_PIN 23

// buttons
#define BUTTON_RESET_EEPROM 15

const int SENSOR_PINS[6] = {ONE_WIRE_0, ONE_WIRE_1, ONE_WIRE_2, ONE_WIRE_3, ONE_WIRE_4, ONE_WIRE_5};
const int I2C_BUSES[6] = {I2C_BUS_0, I2C_BUS_1, I2C_BUS_2, I2C_BUS_3, I2C_BUS_4, I2C_BUS_5};
const int RELAY_PINS[3] = {RELAY_0_PIN, RELAY_1_PIN, RELAY_2_PIN};

class SystemConfig
{
public:
    std::string wifiSSID;
    std::string wifiPassword;
    std::string id;
    std::string name;

    static int jsonSize()
    {
        return 1024;
    }
    DynamicJsonDocument toJSON()
    {
        DynamicJsonDocument doc(jsonSize());

        doc["wifiSSID"] = wifiSSID;
        doc["wifiPassword"] = wifiPassword;
        doc["id"] = id;
        doc["name"] = name;
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
        config.name = doc["name"].as<std::string>();

        return config;
    }
};

#endif