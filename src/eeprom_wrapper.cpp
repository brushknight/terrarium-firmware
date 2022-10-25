#include "eeprom_wrapper.h"

namespace Eeprom
{

    const int WIFI_SSID_INDEX = 1;
    const int WIFI_SSID_LEN = 32;
    const int WIFI_PASS_INDEX = WIFI_SSID_INDEX + WIFI_SSID_LEN;
    const int WIFI_PASS_LEN = 32;
    const int ID_INDEX = WIFI_PASS_INDEX + WIFI_PASS_LEN;
    const int ID_INDEX_LEN = 10;
    const int CONFIG_INDEX = ID_INDEX + ID_INDEX_LEN;

    // new indexes
    const int SET_INDEX = 0;
    const int SYSTEM_CONFIG_INDEX = 1;
    const int CONTROLLER_CONFIG_LENGTH = 2048;

    const int ZONE_CONTROLLER_SET_INDEX = SYSTEM_CONFIG_INDEX + CONTROLLER_CONFIG_LENGTH;

    const int ZONE_CONTROLLER_INDEX = ZONE_CONTROLLER_SET_INDEX + 1;
    const int ZONE_CONTROLLER_LENGTH = 4096;

    const uint8_t externallAddress = 0x50;
    ExternalEEPROM externalEEPROM;

    bool isExternalEEPROM = false;

    Zone::Controller zoneController;
    bool wasZoneControllerLoaded = false;
    bool isZoneControllerLoading = false;
    bool isZoneControllerSaving = false;
    SystemConfig systemConfig;
    bool wasSystemConfigLoaded = false;

    void setup()
    {
        EEPROM.begin(4000);

        if (externalEEPROM.begin(externallAddress))
        {
            isExternalEEPROM = true;
            Serial.printf("EEPROM detected\n");
            Serial.printf("Mem size in bytes: %d\n", externalEEPROM.length());
        }
        else
        {
            Serial.printf("EEPROM not detected\n");
        }
    }

    void clear()
    {
        clearSystemSettings();
        clearZoneController();
    }

    void clearZoneController()
    {
        if (isExternalEEPROM)
        {
            Serial.println("Clearing external EEPROM");
            for (int i = 0; i < externalEEPROM.length(); i++)
            {
                externalEEPROM.write(i, 0);
                if (i % 1000 == 0)
                {
                    Serial.println(i);
                }
            }
        }
    }

    void clearSystemSettings()
    {
        for (int i = 0; i < SYSTEM_CONFIG_INDEX + CONTROLLER_CONFIG_LENGTH; i++)
        {
            EEPROM.write(i, 0);
        }
    }

    bool isMemorySet()
    {
        return ((isExternalEEPROM && isSystemConfigSetExternalEEPROM()) || isSystemConfigSetESP32());
    }

    bool isSystemConfigSetESP32()
    {
        return EEPROM.read(SET_INDEX) == 1;
    }

    bool isSystemConfigSetExternalEEPROM()
    {

        return externalEEPROM.read(SET_INDEX) == 1;
    }

    bool isZoneControllerSetExternalEEPROM()
    {
        return externalEEPROM.read(ZONE_CONTROLLER_SET_INDEX) == 1;
    }

    void saveSystemConfig(SystemConfig systemConfig)
    {
        DynamicJsonDocument doc = systemConfig.toJSON();
        // Lastly, you can print the resulting JSON to a String
        std::string json;
        serializeJson(doc, json);
        Serial.println("Saving system config...");
        Serial.println(json.c_str());

        for (int i = 0; i < json.length(); ++i)
        {
            EEPROM.write(i + SYSTEM_CONFIG_INDEX, json[i]);
        }
        EEPROM.write(SET_INDEX, 1);

        Serial.println("Saved into ESP32 EEPROM [OK]");

        if (isExternalEEPROM)
        {
            for (int i = 0; i < json.length(); ++i)
            {
                externalEEPROM.write(i + SYSTEM_CONFIG_INDEX, json[i]);
            }
            externalEEPROM.write(SET_INDEX, 1);
            Serial.println("Saved into external EEPROM [OK]");
        }
    }

    SystemConfig loadSystemConfig()
    {

        if (wasSystemConfigLoaded)
        {
            return systemConfig;
        }

        if (isExternalEEPROM && isSystemConfigSetExternalEEPROM())
        {
            systemConfig = loadSystemConfigFromExternalEEPROM();
            wasSystemConfigLoaded = true;
            return systemConfig;
        }
        if (isSystemConfigSetESP32())
        {
            systemConfig = loadSystemConfigFromESP32();
            wasSystemConfigLoaded = true;
            return systemConfig;
        }
        return SystemConfig();
    }

    SystemConfig loadSystemConfigFromESP32()
    {
        SystemConfig config;

        Serial.println("Loading system config from ESP32 EEPROM...");

        char raw[CONTROLLER_CONFIG_LENGTH];

        for (int i = 0; i < CONTROLLER_CONFIG_LENGTH; ++i)
        {
            raw[i] = char(EEPROM.read(i + SYSTEM_CONFIG_INDEX));
        }
        Serial.println(raw);

        std::string json = std::string(raw);

        Serial.println(json.c_str());
        config = SystemConfig::fromJSON(json);

        return config;
    }

    SystemConfig loadSystemConfigFromExternalEEPROM()
    {
        SystemConfig config;

        Serial.println("Loading system config from external EEPROM...");
        char raw[CONTROLLER_CONFIG_LENGTH];

        for (int i = 0; i < CONTROLLER_CONFIG_LENGTH; ++i)
        {
            raw[i] = char(externalEEPROM.read(i + SYSTEM_CONFIG_INDEX));
        }
        // Serial.println(raw);

        std::string json = std::string(raw);

        Serial.println(json.c_str());
        config = SystemConfig::fromJSON(json);
        Serial.println("Loaded system config from external EEPROM [OK]");
        return config;
    }

    void saveZoneControllerTask(void *parameter)
    {
        isZoneControllerSaving = true;
        // Serial.println("Cleaning zone controller to eeprom before saving");
        // clearZoneController();
        Serial.println("saving zone controller to eeprom");
        DynamicJsonDocument doc = zoneController.toJSON();
        // Lastly, you can print the resulting JSON to a String
        std::string json;
        serializeJson(doc, json);
        Serial.println("Saving...");
        Serial.println(json.c_str());

        for (int i = 0; i < json.length(); ++i)
        {
            externalEEPROM.write(i + ZONE_CONTROLLER_INDEX, json[i]);
            // Serial.println(json[i]);
        }



        externalEEPROM.write(ZONE_CONTROLLER_SET_INDEX, 1);
        isZoneControllerSaving = false;
        Serial.println("Saving [OK]");
        //ESP.restart();
    }

    void saveZoneController(Zone::Controller config)
    {
        if (isExternalEEPROM)
        {
            Serial.println("setting config to pointer");
            zoneController = config;
            Serial.println("creating task to save into eeprom");
            xTaskCreatePinnedToCore(
                saveZoneControllerTask,
                "saveZoneControllerTask",
                1024 * 10,
                NULL,
                3,
                NULL,
                1);
        }
        else
        {
            Serial.println("External eeprom: No storage found");
        }
    }

    Zone::Controller loadZoneController()
    {
        if (isZoneControllerSaving) {
            for (int i = 0; i < 60; i++)
            {
                Serial.println("Loading zone controller - waiting");
                vTaskDelay(1 * 1000 / portTICK_PERIOD_MS);
                
                if (!isZoneControllerSaving)
                {
                    continue; // jump to loading step
                }
            }
        }

        Serial.println(isZoneControllerLoading);
        if (isZoneControllerLoading)
        {
            for (int i = 0; i < 60; i++)
            {
                vTaskDelay(1 * 1000 / portTICK_PERIOD_MS);
                if (wasZoneControllerLoaded)
                {
                    return zoneController;
                }
                // reboot?
            }
        }

        Serial.println("Loading climate config");

        if (wasZoneControllerLoaded)
        {
            return zoneController;
        }

        if (isExternalEEPROM && isZoneControllerSetExternalEEPROM())
        {
            isZoneControllerLoading = true;
            Serial.println("Loading zone controller from external eeprom");

            char raw[ZONE_CONTROLLER_LENGTH];

            for (int i = 0; i < ZONE_CONTROLLER_LENGTH; ++i)
            {
                raw[i] = char(externalEEPROM.read(i + ZONE_CONTROLLER_INDEX));
                // Serial.println(raw[i]);
            }

            Serial.println("Loaded raw");
            Serial.println(raw);

            std::string json = std::string(raw);

            // Serial.println(json.c_str());
            zoneController = Zone::Controller::fromJSON(json);
            wasZoneControllerLoaded = true;
            isZoneControllerLoading = false;
            Serial.println("Loaded zone controller from external eeprom [OK]");
        }
        else
        {
            // load empty config
            Serial.println("No storage found, loading initial zone controller");

            zoneController = Zone::Controller();
        }
        return zoneController;
    }
}
