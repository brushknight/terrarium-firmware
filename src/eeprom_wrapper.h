#ifndef TERRARIUM_EEPROM
#define TERRARIUM_EEPROM

#include <EEPROM.h>
#include "SparkFun_External_EEPROM.h"
#include "config.h"
#include "zone.h"
#include "status.h"

namespace Eeprom
{
    static const char *TAG = "eeprom";

    static const int RESERVED_FOR_FLAGS = 10;
    static const int INDEX_SYSTEM_SET = 0;
    static const int INDEX_CLIMATE_SET = 1;

    static const int INDEX_SYSTEM_JSON = RESERVED_FOR_FLAGS;
    static const int INDEX_CLIMATE_JSON = INDEX_SYSTEM_JSON + 2000;

    static const uint8_t externallAddress = 0x50;

    // redo storage of the config

    // eeprom checked, move task here

    class Eeprom
    {
    private:
        ExternalEEPROM *externalEEPROM;
        int systemConfigMaxLength = 0;
        int climateConfigMaxLength = 0;
        bool isEEPROM = false;
        bool isExternalEEPROM = false;

    public:
        Eeprom(ExternalEEPROM *extEEPROM, int givenSystemConfigMaxLength, int givenClimateConfigMaxLength)
        {
            externalEEPROM = extEEPROM;
            systemConfigMaxLength = givenSystemConfigMaxLength;
            climateConfigMaxLength = givenClimateConfigMaxLength;
        }
        void setup()
        {
            ESP_LOGD(TAG, "[..] starting ESP32 onboard EEPROM");
            if (EEPROM.begin(4000))
            {
                isEEPROM = true;
                ESP_LOGD(TAG, "[OK] ESP32 onboard EEPROM initialised");
            }
            else
            {
                ESP_LOGE(TAG, "[FAIL] ESP32 onboard EEPROM failed to initialised");
            }

            ESP_LOGD(TAG, "[..] starting external EEPROM");
            if (externalEEPROM->begin(externallAddress))
            {
                isExternalEEPROM = true;
                // TODO transform into kb
                ESP_LOGD(TAG, "[OK] External EEPROM detected, mem size in bytes: %d", externalEEPROM->length());
            }
            else
            {
                ESP_LOGE(TAG, "[FAIL] External EEPROM not detected");
            }
        }
        bool isClimateConfigSet()
        {
            if (!isExternalEEPROM)
            {
                ESP_LOGE(TAG, "[FAIL] Loading climate config is not possible without external EEPROM");
                return false;
            }
            return externalEEPROM->read(INDEX_CLIMATE_SET) == 1;
        }
        // 0 - not set, 1 - set in esp32 eeprom, 2 - set in external eeprom
        int isSystemConfigSet()
        {
            if (isExternalEEPROM && externalEEPROM->read(INDEX_SYSTEM_JSON) == 1)
            {
                return 2;
            }
            return EEPROM.read(INDEX_SYSTEM_JSON) == 1 ? 1 : 0;
        }
        void saveSystemConfig(std::string *json)
        {
            ESP_LOGI(TAG, "[..] Saving system config");
            ESP_LOGD(TAG, "%s", json->c_str());

            // clearSystemSettings();

            int cellsToSave = json->length();

            for (int i = 0; i < cellsToSave; ++i)
            {
                EEPROM.write(i + INDEX_SYSTEM_JSON, (*json)[i]);
                if (isExternalEEPROM)
                {
                    externalEEPROM->write(i + INDEX_SYSTEM_JSON, (*json)[i]);
                }
            }
            // TODO clear rest of cells

            if (isExternalEEPROM)
            {
                externalEEPROM->write(INDEX_SYSTEM_SET, 1);
            }
            EEPROM.write(INDEX_SYSTEM_SET, 1);

            ESP_LOGI(TAG, "[OK] Saving system config");
        }
        void saveClimateConfig(std::string *json)
        {
            ESP_LOGI(TAG, "[..] Saving climate config");
            ESP_LOGD(TAG, "%s", json->c_str());

            if (!isExternalEEPROM)
            {
                ESP_LOGE(TAG, "[FAIL] Saving climate config is not possible without external EEPROM");
                return;
            }

            int cellsToSave = json->length();

            for (int i = 0; i < cellsToSave; ++i)
            {
                externalEEPROM->write(i + INDEX_CLIMATE_JSON, (*json)[i]);
            }
            // TODO clear rest of cells
            externalEEPROM->write(INDEX_CLIMATE_SET, 1);
            EEPROM.write(INDEX_CLIMATE_SET, 1);

            ESP_LOGI(TAG, "[OK] Saving system config");
        }
        std::string loadSystemConfg()
        {
            std::string json = "{}";

            int systemConfigStatus = isSystemConfigSet();

            if (systemConfigStatus == 2)
            {
                ESP_LOGI(TAG, "[..] Loading system config from external EEPROM");

                char raw[systemConfigMaxLength];

                for (int i = 0; i < systemConfigMaxLength; ++i)
                {
                    raw[i] = char(externalEEPROM->read(i + INDEX_CLIMATE_JSON));
                }

                ESP_LOGD(TAG, "%s", raw);

                json = std::string(raw);

                ESP_LOGI(TAG, "[OK] Loading system config from external EEPROM");
                return json;
            }

            if (systemConfigStatus == 1)
            {
                ESP_LOGI(TAG, "[..] Loading system config from ESP32 EEPROM");

                char raw[systemConfigMaxLength];

                for (int i = 0; i < systemConfigMaxLength; ++i)
                {
                    raw[i] = char(EEPROM.read(i + INDEX_CLIMATE_JSON));
                }

                ESP_LOGD(TAG, "%s", raw);

                json = std::string(raw);

                ESP_LOGI(TAG, "[OK] Loading system config from ESP32 EEPROM");
                return json;
            }

            ESP_LOGI(TAG, "[FAILED] System config not found");

            return json;
        }
        std::string loadClimateConfig()
        {
            std::string json = "{}";

            if (isClimateConfigSet())
            {
                ESP_LOGI(TAG, "[..] Loading climate config from external EEPROM");

                char raw[climateConfigMaxLength];

                for (int i = 0; i < climateConfigMaxLength; ++i)
                {
                    raw[i] = char(externalEEPROM->read(i + INDEX_CLIMATE_JSON));
                }

                ESP_LOGD(TAG, "%s", raw);

                json = std::string(raw);

                ESP_LOGI(TAG, "[OK] Loading climate config from external EEPROM");
            }

            return json;
        }
        void resetSystemConfig()
        {
            ESP_LOGI(TAG, "[..] Clearing system config from both EEPROMs ");
            int cellsToClean = systemConfigMaxLength;
            for (int i = 0; i < cellsToClean; i++)
            {
                EEPROM.write(i + INDEX_SYSTEM_JSON, 0);
                if (isExternalEEPROM)
                {
                    externalEEPROM->write(i + INDEX_SYSTEM_JSON, 0);
                }
                if (i % 100 == 0)
                {
                    int percent = float(i) / float(cellsToClean) * 100;
                    ESP_LOGI(TAG, "Cleaned %.2f%%", percent);
                }
            }

            if (isExternalEEPROM)
            {
                externalEEPROM->write(INDEX_SYSTEM_JSON, 0);
            }

            EEPROM.write(INDEX_SYSTEM_JSON, 0);

            ESP_LOGI(TAG, "[OK] Clearing system config from both EEPROMs ");
        }
        void resetClimateConfig()
        {
            ESP_LOGI(TAG, "[..] Clearing climate config from both EEPROMs ");
            if (!isExternalEEPROM)
            {
                ESP_LOGE(TAG, "[FAIL] Clearing climate config is not possible without external EEPROM");
                return;
            }
            int cellsToClean = climateConfigMaxLength;
            for (int i = 0; i < cellsToClean; i++)
            {
                externalEEPROM->write(i + INDEX_CLIMATE_JSON, 0);

                if (i % 100 == 0)
                {
                    int percent = float(i) / float(cellsToClean) * 100;
                    ESP_LOGI(TAG, "Cleaned %.2f%%", percent);
                }
            }

            EEPROM.write(INDEX_CLIMATE_JSON, 0);

            ESP_LOGI(TAG, "[OK] Clearing climate config from external EEPROM");
        }
        bool resetWatcher()
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
            // clearZoneController();
            return true;
        }
    };

    // void setup(ExternalEEPROM *extEEPROM);
    // bool isMemorySet();
    // void clear();
    // void clearZoneController();
    // void clearZoneControllerFull();
    // void clearSystemSettings();
    // void saveSystemConfig(SystemConfig config);
    // // void updateZoneControllerFromJson(std::string *json);
    // void saveZoneController();
    // void saveZoneControllerJSON(Zone::Controller *zoneController);
    // // Zone::Controller *loadZoneController();
    // SystemConfig loadSystemConfig();
    // SystemConfig loadSystemConfigFromESP32();
    // SystemConfig loadSystemConfigFromExternalEEPROM();
    // bool isSystemConfigSetESP32();
    // bool isSystemConfigSetExternalEEPROM();
    // bool isZoneControllerSetExternalEEPROM();
    // void saveZoneControllerTask(void *parameter);
    // bool resetEepromChecker();

    // // refactoring
    // std::string loadZoneControllerJSON();
}

#endif