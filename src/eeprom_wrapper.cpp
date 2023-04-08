#include "eeprom_wrapper.h"

namespace Eeprom
{

    // new indexes
    const int IS_SYSTEM_SET_INDEX = 0;
    const int SYSTEM_CONFIG_INDEX = 1;
    const int CONTROLLER_CONFIG_LENGTH = 2048;

    const int ZONE_CONTROLLER_SET_INDEX = SYSTEM_CONFIG_INDEX + CONTROLLER_CONFIG_LENGTH;
    const int ZONE_CONTROLLER_INDEX = ZONE_CONTROLLER_SET_INDEX + 1;

    const uint8_t externallAddress = 0x50;
    ExternalEEPROM externalEEPROM;

    bool isExternalEEPROM = false;

    Zone::Controller zoneController;
    SystemConfig systemConfig;

    bool wasZoneControllerLoaded = false;
    bool isZoneControllerLoading = false;
    bool isZoneControllerSaving = false;
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

    bool resetEepromChecker()
    {
        pinMode(BUTTON_RESET_EEPROM, INPUT);
        int resetButtonState = digitalRead(BUTTON_RESET_EEPROM);
        int previousResetButtonState = 0;
        for (int i = 0; i < 3; i++)
        {
            resetButtonState = digitalRead(BUTTON_RESET_EEPROM);
            // Serial.printf("Reset button %f\n", resetButtonState);
            if (resetButtonState == 1)
            {
                Status::setWarning();
                vTaskDelay(1 * 1000 / portTICK_PERIOD_MS);
                previousResetButtonState = 1;
            }
            else
            {
                if (previousResetButtonState == 1)
                {
                    Status::turnLedOff();
                }
                previousResetButtonState = 0;
                return false;
            }
        }
        for (int i = 0; i < 3; i++)
        {
            resetButtonState = digitalRead(BUTTON_RESET_EEPROM);
            // Serial.printf("Reset button %f\n", resetButtonState);
            if (resetButtonState == 1)
            {
                Status::setError();
                vTaskDelay(1 * 1000 / portTICK_PERIOD_MS);
                previousResetButtonState = 1;
            }
            else
            {
                if (previousResetButtonState == 1)
                {
                    Status::turnLedOff();
                }
                previousResetButtonState = 0;
                return false;
            }
        }
        clearZoneController();
        return true;
    }

    void clear()
    {
        clearSystemSettings();
        clearZoneController();
    }

    void clearZoneControllerFull()
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

    void clearZoneController()
    {
        if (isExternalEEPROM)
        {
            Serial.println("Clearing external EEPROM");
            for (int i = 0; i < Zone::Controller::jsonSize(); i++)
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
        Serial.println("Clearing system settings");
        for (int i = 0; i < SYSTEM_CONFIG_INDEX + CONTROLLER_CONFIG_LENGTH; i++)
        {
            EEPROM.write(i, 0);
            if (isExternalEEPROM)
            {
                externalEEPROM.write(i, 0);
            }
        }
    }

    bool isMemorySet()
    {
        return ((isExternalEEPROM && isSystemConfigSetExternalEEPROM()) || isSystemConfigSetESP32());
    }

    bool isSystemConfigSetESP32()
    {
        return EEPROM.read(IS_SYSTEM_SET_INDEX) == 1;
    }

    bool isSystemConfigSetExternalEEPROM()
    {
        return externalEEPROM.read(IS_SYSTEM_SET_INDEX) == 1;
    }

    bool isZoneControllerSetExternalEEPROM()
    {
        return externalEEPROM.read(ZONE_CONTROLLER_SET_INDEX) == 1;
    }

    void saveSystemConfig(SystemConfig systemConfig)
    {
        DynamicJsonDocument doc = systemConfig.toJSON();
        std::string json;
        serializeJson(doc, json);
        Serial.println("[..] System config | saving");
        Serial.println(json.c_str());

        clearSystemSettings();

        for (int i = 0; i < json.length(); ++i)
        {
            EEPROM.write(i + SYSTEM_CONFIG_INDEX, json[i]);
            if (isExternalEEPROM)
            {
                externalEEPROM.write(i + SYSTEM_CONFIG_INDEX, json[i]);
            }
        }
        EEPROM.write(IS_SYSTEM_SET_INDEX, 1);

        if (isExternalEEPROM)
        {
            externalEEPROM.write(IS_SYSTEM_SET_INDEX, 1);
        }

        Serial.println("[OK] System config | saved");
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
        Serial.println(zoneController.getTemperatureZone(0).slug.c_str());
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
        Serial.println("Restarting in 3s");
        vTaskDelay(3 * 1000 / portTICK_PERIOD_MS);
        ESP.restart();
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
                1024 * 32,
                NULL,
                3,
                NULL,
                0);
        }
        else
        {
            Serial.println("External eeprom: No storage found");
        }
    }

    Zone::Controller loadZoneController()
    {
        if (isZoneControllerSaving)
        {
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

        Serial.println("Loading zone controller");

        if (wasZoneControllerLoaded)
        {
            return zoneController;
        }

        if (isExternalEEPROM && isZoneControllerSetExternalEEPROM())
        {
            isZoneControllerLoading = true;
            Serial.println("Loading zone controller from external eeprom");

            int zoneControllerSize = Zone::Controller::jsonSize();

            char raw[zoneControllerSize];

            for (int i = 0; i < zoneControllerSize; ++i)
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
