#ifndef CONFIG
#define CONFIG

#include <string>
#include "ArduinoJson.h"
#include "utils.h"

// #define EXTERNAL_EEPROM true

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
    bool wifiAPMode; // false - connect to given ssid, true - spawn ssid with given data
    std::string id;
    std::string animalName;
    std::string timeZone;
    bool ntpEnabled;

    SystemConfig(){
        // defaults for wifi
        wifiSSID = "Terrarium Controller" + Utils::getMac();
        wifiPassword = "Chameleon";
        wifiAPMode = true;
        ntpEnabled = false;
    }

    static int jsonSize()
    {
        return 1024;
    }
    DynamicJsonDocument toJSON()
    {
        DynamicJsonDocument doc(jsonSize());

        doc["wifi_ssid"] = wifiSSID;
        doc["wifi_password"] = wifiPassword;
        doc["wifi_ap_mode"] = wifiAPMode;
        doc["id"] = id;
        doc["animal_name"] = animalName;
        doc["time_zone"] = timeZone;
        doc["ntp_enabled"] = ntpEnabled;
        return doc;
    }
    static SystemConfig fromJSON(std::string json)
    {
        DynamicJsonDocument doc(jsonSize());
        deserializeJson(doc, json);

        return SystemConfig::fromJSONObj(doc);
    }
    static SystemConfig fromJSONObj(DynamicJsonDocument jsonObj)
    {
        SystemConfig config;

        if (jsonObj.containsKey("wifiSSID"))
        {
            // legacy compatibility
            config.wifiSSID = jsonObj["wifiSSID"].as<std::string>();
            config.wifiPassword = jsonObj["wifiPassword"].as<std::string>();
            config.id = jsonObj["id"].as<std::string>();
            config.animalName = jsonObj["name"].as<std::string>();
        }
        else
        {
            // new structure
            config.wifiSSID = jsonObj["wifi_ssid"].as<std::string>();
            config.wifiPassword = jsonObj["wifi_password"].as<std::string>();
            config.id = jsonObj["id"].as<std::string>();
            config.animalName = jsonObj["animal_name"].as<std::string>();
            config.timeZone = jsonObj["time_zone"].as<std::string>();
            config.ntpEnabled = jsonObj["ntp_enabled"];
            config.wifiAPMode = jsonObj["wifi_ap_mode"];
        }

        return config;
    }
};

#endif