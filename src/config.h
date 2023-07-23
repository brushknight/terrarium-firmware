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

// DHT22 sensors pins
#define ONE_WIRE_0 int(16)
#define ONE_WIRE_1 int(15)
#define ONE_WIRE_2 int(7)
#define ONE_WIRE_3 int(6)
#define ONE_WIRE_4 int(5)
#define ONE_WIRE_5 int(4)

// I2c Busses ports
#define I2C_BUS_0 int(3)
#define I2C_BUS_1 int(0)
#define I2C_BUS_2 int(1)
#define I2C_BUS_3 int(2)
#define I2C_BUS_4 int(4)
#define I2C_BUS_5 int(5)

// lights
#define LEDPIN 11
#define STATUS_PIN 10

// buttons
#define BUTTON_RESET_EEPROM 9

const int SENSOR_PINS[6] = {ONE_WIRE_0, ONE_WIRE_1, ONE_WIRE_2, ONE_WIRE_3, ONE_WIRE_4, ONE_WIRE_5};
const int I2C_BUSES[6] = {I2C_BUS_0, I2C_BUS_1, I2C_BUS_2, I2C_BUS_3, I2C_BUS_4, I2C_BUS_5};

const int IO_EXPANDER_RELAY_PORTS[4] = {3, 2, 1, 0};
const int IO_EXPANDER_SENSORS_EN = 5;
const int IO_EXPANDER_SENSORS_PULL_UP = 4;

const int SAFETY_RELAY_PIN = 12;

const int FAN_1_PIN = 18;
const int FAN_2_PIN = 17;

const int FANS[2] = {FAN_1_PIN, FAN_2_PIN};

const int BUZZER_1_PIN = 13;
const int BUZZERS[1] = {BUZZER_1_PIN};

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
    bool changed = false;

    bool isNetConfigEqual(SystemConfig *compareTo)
    {
        static const char *TAG = "config";

        ESP_LOGD(TAG, "ID %d, %d \n SSID %s, %s \n Password %s, %s \n AP mode %d, %d",
                 id,
                 compareTo->id,
                 wifiSSID.c_str(),
                 compareTo->wifiSSID.c_str(),
                 wifiPassword.c_str(),
                 compareTo->wifiPassword.c_str(),
                 wifiAPMode,
                 compareTo->wifiAPMode);

        return id == compareTo->id &&
               wifiSSID == compareTo->wifiSSID &&
               wifiPassword == compareTo->wifiPassword &&
               wifiAPMode == compareTo->wifiAPMode;
    }

    bool toBePersisted()
    {
        return changed;
    }
    void persisted()
    {
        changed = false;
    }

    SystemConfig()
    {
        // defaults for wifi
        wifiSSID = "Terrarium Controller";
        wifiPassword = "Chameleon";
        wifiAPMode = true;
        ntpEnabled = false;
    }

    SystemConfig(std::string mac)
    {
        // defaults for wifi
        wifiSSID = "Terrarium Controller " + mac;
        wifiPassword = "Chameleon";
        wifiAPMode = true;
        ntpEnabled = false;
    }

    static int jsonSize()
    {
        return 2048;
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

    void updateFromJSON(std::string *json, bool triggerPersister)
    {
        static const char *TAG = "config";

        ESP_LOGD(TAG, "[..] String to JSON");
        DynamicJsonDocument jsonObj(jsonSize());
        deserializeJson(jsonObj, *json);
        ESP_LOGD(TAG, "[OK] String to JSON");

        wifiSSID = jsonObj["wifi_ssid"].as<std::string>();
        wifiPassword = jsonObj["wifi_password"].as<std::string>();
        id = jsonObj["id"].as<std::string>();
        animalName = jsonObj["animal_name"].as<std::string>();
        timeZone = jsonObj["time_zone"].as<std::string>();
        ntpEnabled = jsonObj["ntp_enabled"];
        wifiAPMode = jsonObj["wifi_ap_mode"];
        changed = triggerPersister;
    }
};

#endif