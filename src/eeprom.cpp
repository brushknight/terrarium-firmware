#include "eeprom.h"

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
    const int CONTROLLER_CONFIG_INDEX = 1;
    const int CONTROLLER_CONFIG_LENGTH = 2048;

    const int CLIMATE_CONFIG_SET_INDEX = CONTROLLER_CONFIG_INDEX + CONTROLLER_CONFIG_LENGTH;

    const int CLIMATE_CONFIG_INDEX = CLIMATE_CONFIG_SET_INDEX + 1;
    const int CLIMATE_CONFIG_LENGTH = 4096;

    const uint8_t externallAddress = 0x50;
    ExternalEEPROM externalEEPROM;

    bool isExternalEEPROM = false;

    ClimateConfig climateConfig;
    bool wasClimateConfigLoaded = false;
    bool isClimateConfigLoading = false;
    ControllerConfig controllerConfig;
    bool wasControllerConfigLoaded = false;

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
        clearController();
        clearClimate();
    }

    void clearClimate()
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

    void clearController()
    {
        for (int i = 0; i < CONTROLLER_CONFIG_INDEX + CONTROLLER_CONFIG_LENGTH; i++)
        {
            EEPROM.write(i, 0);
        }
    }

    bool isMemorySet()
    {
        return ((isExternalEEPROM && isControllerConfigSetExternalEEPROM()) || isControllerConfigSetESP32());
    }

    bool isControllerConfigSetESP32()
    {
        return EEPROM.read(SET_INDEX) == 1;
    }

    bool isControllerConfigSetExternalEEPROM()
    {

        return externalEEPROM.read(SET_INDEX) == 1;
    }

    bool isClimateConfigSetExternalEEPROM()
    {
        return externalEEPROM.read(CLIMATE_CONFIG_SET_INDEX) == 1;
    }

    void saveControllerConfig(ControllerConfig config)
    {

        DynamicJsonDocument doc = config.toJSON();
        // Lastly, you can print the resulting JSON to a String
        std::string json;
        serializeJson(doc, json);
        Serial.println("Saving controller config...");
        Serial.println(json.c_str());

        for (int i = 0; i < json.length(); ++i)
        {
            EEPROM.write(i + CONTROLLER_CONFIG_INDEX, json[i]);
        }
        EEPROM.write(SET_INDEX, 1);

        Serial.println("Saved into ESP32 EEPROM [OK]");

        if (isExternalEEPROM)
        {

            for (int i = 0; i < json.length(); ++i)
            {
                externalEEPROM.write(i + CONTROLLER_CONFIG_INDEX, json[i]);
            }
            externalEEPROM.write(SET_INDEX, 1);
            Serial.println("Saved into external EEPROM [OK]");
        }
    }

    ControllerConfig loadControllerConfig()
    {

        if (wasControllerConfigLoaded)
        {
            return controllerConfig;
        }

        if (isExternalEEPROM && isControllerConfigSetExternalEEPROM())
        {
            controllerConfig = loadControllerConfigFromExternalEEPROM();
            wasControllerConfigLoaded = true;
            return controllerConfig;
        }
        if (isControllerConfigSetESP32())
        {
            controllerConfig = loadControllerConfigFromESP32();
            wasControllerConfigLoaded = true;
            return controllerConfig;
        }
        return ControllerConfig();
    }

    ControllerConfig loadControllerConfigFromESP32()
    {
        ControllerConfig config;

        Serial.println("Loading controller config from ESP32 EEPROM...");

        char raw[CONTROLLER_CONFIG_LENGTH];

        for (int i = 0; i < CONTROLLER_CONFIG_LENGTH; ++i)
        {
            raw[i] = char(EEPROM.read(i + CONTROLLER_CONFIG_INDEX));
        }
        Serial.println(raw);

        std::string json = std::string(raw);

        Serial.println(json.c_str());
        config = ControllerConfig::fromJSON(json);

        return config;
    }

    ControllerConfig loadControllerConfigFromExternalEEPROM()
    {
        ControllerConfig config;

        Serial.println("Loading controller config from external EEPROM...");
        char raw[CONTROLLER_CONFIG_LENGTH];

        for (int i = 0; i < CONTROLLER_CONFIG_LENGTH; ++i)
        {
            raw[i] = char(externalEEPROM.read(i + CONTROLLER_CONFIG_INDEX));
        }
        //Serial.println(raw);

        std::string json = std::string(raw);

        //Serial.println(json.c_str());
        config = ControllerConfig::fromJSON(json);
        Serial.println("Loaded controller config from external EEPROM [OK]");
        return config;
    }

    void saveClimateConfigTask(void *parameter)
    {
        Serial.println("saving to eeprom");
        DynamicJsonDocument doc = climateConfig.toJSON();
        // Lastly, you can print the resulting JSON to a String
        std::string json;
        serializeJson(doc, json);
        Serial.println("Saving...");
        Serial.println(json.c_str());

        for (int i = 0; i < json.length(); ++i)
        {
            externalEEPROM.write(i + CLIMATE_CONFIG_INDEX, json[i]);
            // Serial.println(json[i]);
        }
        Serial.println("Saving [OK]");

        externalEEPROM.write(CLIMATE_CONFIG_SET_INDEX, 1);

        ESP.restart();
    }

    void saveClimateConfig(ClimateConfig config)
    {
        if (isExternalEEPROM)
        {
            Serial.println("setting config to pointer");
            climateConfig = config;
            Serial.println("creating task to save into eeprom");
            xTaskCreatePinnedToCore(
                saveClimateConfigTask,
                "saveClimateConfigTask",
                1024 * 8,
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

    ClimateConfig loadClimateConfig()
    {

        if (isClimateConfigLoading)
        {
            for (int i = 0; i < 60; i++)
            {
                vTaskDelay(1 * 1000 / portTICK_PERIOD_MS);
                if (wasClimateConfigLoaded)
                {
                    return climateConfig;
                }
                // reboot?
            }
        }

        Serial.println("Loading climate config");

        if (wasClimateConfigLoaded)
        {
            return climateConfig;
        }

        if (isExternalEEPROM && isClimateConfigSetExternalEEPROM())
        {
            isClimateConfigLoading = true;
            Serial.println("Loading climate config from external eeprom");

            char raw[CLIMATE_CONFIG_LENGTH];

            for (int i = 0; i < CLIMATE_CONFIG_LENGTH; ++i)
            {
                raw[i] = char(externalEEPROM.read(i + CLIMATE_CONFIG_INDEX));
                // Serial.println(raw[i]);
            }

            //Serial.println(raw);

            std::string json = std::string(raw);

            //Serial.println(json.c_str());
            climateConfig = ClimateConfig::fromJSON(json);
            wasClimateConfigLoaded = true;
            isClimateConfigLoading = false;
            Serial.println("Loaded climate config from external eeprom [OK]");
        }
        else
        {
            // load empty config
            Serial.println("No storage found");

            climateConfig = loadInitClimateConfig();
        }
        return climateConfig;
    }
}
