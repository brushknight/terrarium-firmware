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
            // transform into kb
            ESP_LOGD(TAG, "External EEPROM detected, mem size in bytes: %d", externalEEPROM.length());
        }
        else
        {
            ESP_LOGW(TAG, "External EEPROM not detected");
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
            ESP_LOGI(TAG, "[..] Full clearing external EEPROM ");
            int eepromLength = externalEEPROM.length();
            for (int i = 0; i < eepromLength; i++)
            {
                externalEEPROM.write(i, 0);
                if (i % 1000 == 0)
                {
                    int percent = i / eepromLength;
                    ESP_LOGI(TAG, "Cleaned %d%%", percent);
                }
            }
            ESP_LOGI(TAG, "[OK] Full clearing external EEPROM");
        }
    }

    void clearZoneController()
    {
        if (isExternalEEPROM)
        {
            ESP_LOGI(TAG, "[..] Patrial clearing external EEPROM ");
            int cellsToClean = Zone::Controller::jsonSize();
            for (int i = 0; i < cellsToClean; i++)
            {
                externalEEPROM.write(i, 0);
                if (i % 1000 == 0)
                {
                    int percent = i / cellsToClean;
                    ESP_LOGI(TAG, "Cleaned %d%%", percent);
                }
            }
            ESP_LOGI(TAG, "[OK] Patrial clearing external EEPROM ");
        }
    }

    void clearSystemSettings()
    {
        ESP_LOGI(TAG, "[..] Clearing system settings from both EEPROMs ");
        int cellsToClean = SYSTEM_CONFIG_INDEX + CONTROLLER_CONFIG_LENGTH;
        for (int i = 0; i < cellsToClean; i++)
        {
            EEPROM.write(i, 0);
            if (isExternalEEPROM)
            {
                externalEEPROM.write(i, 0);
            }
            if (i % 10 == 0)
            {
                int percent = i / cellsToClean;
                ESP_LOGI(TAG, "Cleaned %d%%", percent);
            }
        }
        ESP_LOGI(TAG, "[OK] Clearing system settings from both EEPROMs ");
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
        ESP_LOGI(TAG, "[..] Saving system config");
        ESP_LOGD(TAG, "%s", json.c_str());

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

        ESP_LOGI(TAG, "[OK] Saving system config");
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

        ESP_LOGI(TAG, "[..] Loading system config from ESP32 EEPROM");

        char raw[CONTROLLER_CONFIG_LENGTH];

        for (int i = 0; i < CONTROLLER_CONFIG_LENGTH; ++i)
        {
            raw[i] = char(EEPROM.read(i + SYSTEM_CONFIG_INDEX));
        }

        ESP_LOGD(TAG, "Raw config: %s", raw);

        std::string json = std::string(raw);

        config = SystemConfig::fromJSON(json);

        ESP_LOGI(TAG, "[OK] Loading system config from ESP32 EEPROM");


        return config;
    }

    SystemConfig loadSystemConfigFromExternalEEPROM()
    {
        SystemConfig config;

        ESP_LOGI(TAG, "[..] Loading system config from external EEPROM");
        char raw[CONTROLLER_CONFIG_LENGTH];

        for (int i = 0; i < CONTROLLER_CONFIG_LENGTH; ++i)
        {
            raw[i] = char(externalEEPROM.read(i + SYSTEM_CONFIG_INDEX));
        }
        ESP_LOGD(TAG, "Raw config: %s", raw);

        std::string json = std::string(raw);

        config = SystemConfig::fromJSON(json);
        ESP_LOGI(TAG, "[OK] Loading system config from external EEPROM");
        return config;
    }

    void saveZoneControllerTask(void *parameter)
    {
        isZoneControllerSaving = true;
        // Serial.println("Cleaning zone controller to eeprom before saving");
        // clearZoneController();
        ESP_LOGI(TAG, "[..] Saving zone controller into external EEPROM");
        DynamicJsonDocument doc = zoneController.toJSON();
        std::string json;
        serializeJson(doc, json);
        ESP_LOGD(TAG, "%s", json.c_str());

        for (int i = 0; i < json.length(); ++i)
        {
            externalEEPROM.write(i + ZONE_CONTROLLER_INDEX, json[i]);
        }

        externalEEPROM.write(ZONE_CONTROLLER_SET_INDEX, 1);
        isZoneControllerSaving = false;
        ESP_LOGI(TAG, "[OK] Saving zone controller into external EEPROM");
        ESP_LOGI(TAG, "Restarting in 3 seconds");
        vTaskDelay(3 * 1000 / portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "Restarting now");
        ESP.restart();
    }

    void saveZoneController(Zone::Controller config)
    {
        if (isExternalEEPROM)
        {
            zoneController = config;
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
            ESP_LOGW(TAG, "External EEPROM is empty");
        }
    }

    Zone::Controller loadZoneController()
    {
        if (isZoneControllerSaving)
        {
            for (int i = 0; i < 60; i++)
            {
                ESP_LOGI(TAG, "[..] Loading zone controller - waiting for another thread");
                vTaskDelay(1 * 1000 / portTICK_PERIOD_MS);

                if (!isZoneControllerSaving)
                {
                    continue; // jump to loading step
                }
            }
        }

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

        ESP_LOGD(TAG, "[..] Loading zone controller");


        if (wasZoneControllerLoaded)
        {
            return zoneController;
        }

        if (isExternalEEPROM && isZoneControllerSetExternalEEPROM())
        {
            isZoneControllerLoading = true;
            ESP_LOGI(TAG, "[..] Loading zone controller from external EEPROM");

            int zoneControllerSize = Zone::Controller::jsonSize();

            char raw[zoneControllerSize];

            for (int i = 0; i < zoneControllerSize; ++i)
            {
                raw[i] = char(externalEEPROM.read(i + ZONE_CONTROLLER_INDEX));
            }

            ESP_LOGD(TAG, "%s", raw);

            std::string json = std::string(raw);

            zoneController = Zone::Controller::fromJSON(json);
            wasZoneControllerLoaded = true;
            isZoneControllerLoading = false;
            ESP_LOGI(TAG, "[OK] Loading zone controller from external EEPROM");
        }
        else
        {
            // load empty config
            ESP_LOGI(TAG, "[KO] No storage found, loading default zone controller");

            zoneController = Zone::Controller();
        }
        return zoneController;
    }
}
