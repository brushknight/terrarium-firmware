#include "eeprom.h"

namespace Eeprom
{

    const int SET_INDEX = 0;
    const int WIFI_SSID_INDEX = 1;
    const int WIFI_SSID_LEN = 32;
    const int WIFI_PASS_INDEX = WIFI_SSID_INDEX + WIFI_SSID_LEN;
    const int WIFI_PASS_LEN = 32;
    const int ID_INDEX = WIFI_PASS_INDEX + WIFI_PASS_LEN;
    const int ID_INDEX_LEN = 10;
    const int CONFIG_INDEX = ID_INDEX + ID_INDEX_LEN;

    const int CONFIG_LENGTH = 4096;

    const uint8_t externallAddress = 0x50;
    ExternalEEPROM externalEEPROM;

    bool isExternalEEPROM = false;

    void setup()
    {

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

    bool isMemorySet()
    {
        if (isExternalEEPROM)
        {
            return externalEEPROM.read(SET_INDEX) == 1;
        }
        return EEPROM.read(SET_INDEX) == 1;
    }

    void setMemory()
    {
        if (isExternalEEPROM)
        {
            externalEEPROM.put(SET_INDEX, 1);
        }
        else
        {
            EEPROM.write(SET_INDEX, 1);
            EEPROM.commit();
        }
    }

    void resetMemory()
    {
        if (isExternalEEPROM)
        {
            externalEEPROM.put(SET_INDEX, 0);
        }
        else
        {
            EEPROM.write(SET_INDEX, 0);
            EEPROM.commit();
        }
    }

    std::string readWiFiSSIDFromMemory()
    {
        if (isExternalEEPROM)
        {
            return loadConfig().wifiSSID;
        }
        else
        {
            char ssid[WIFI_SSID_LEN];

            for (int i = 0; i < WIFI_SSID_LEN; ++i)
            {
                ssid[i] = char(EEPROM.read(i + WIFI_SSID_INDEX));
            }

            Serial.print("SSID: ");
            Serial.println(ssid);
            return std::string(ssid);
        }
    }

    std::string readWiFiPassFromMemory()
    {
        if (isExternalEEPROM)
        {
            return loadConfig().wifiPassword;
        }
        else
        {
            char pass[WIFI_PASS_LEN];

            for (int i = 0; i < WIFI_PASS_LEN; ++i)
            {
                pass[i] = char(EEPROM.read(i + WIFI_PASS_INDEX));
            }
            Serial.print("PASS: ");
            Serial.println(pass);
            return std::string(pass);
        }
    }

    std::string readIDFromMemory()
    {

        if (isExternalEEPROM)
        {
            return loadConfig().id;
        }
        else
        {
            char id[ID_INDEX_LEN];

            for (int i = 0; i < ID_INDEX_LEN; ++i)
            {
                id[i] = char(EEPROM.read(i + ID_INDEX));
            }
            Serial.print("ID: ");
            Serial.println(id);
            return std::string(id);
        }
    }

    void writeWiFiSSIDToMemory(std::string ssid)
    {
        if (isExternalEEPROM)
        {
            Serial.println("Saving SSID...");
            Config config = loadConfig();
            config.wifiSSID = ssid;
            saveConfig(config);
            Serial.println("Saving SSID [OK]");
        }
        else
        {
            Serial.println("clearing eeprom");
            for (int i = 0; i < WIFI_SSID_LEN; ++i)
            {
                EEPROM.write(i + WIFI_SSID_INDEX, 0);
            }
            Serial.println("writing eeprom ssid:");
            Serial.println(ssid.c_str());
            // TODO add 32 max limit
            for (int i = 0; i < ssid.length(); ++i)
            {
                EEPROM.write(i + WIFI_SSID_INDEX, ssid[i]);
                // Serial.print("Wrote: ");
                // Serial.println(ssid[i]);
            }
            EEPROM.commit();
        }
    }

    void writeWiFiPassToMemory(std::string pass)
    {
        if (isExternalEEPROM)
        {
            Config config = loadConfig();
            config.wifiPassword = pass;
            saveConfig(config);
        }
        else
        {
            Serial.println("clearing eeprom");
            for (int i = 0; i < 32; ++i)
            {
                EEPROM.write(i + WIFI_PASS_INDEX, 0);
            }
            Serial.print("writing eeprom ssid:");
            Serial.println(pass.c_str());
            // TODO add 32 max limit
            for (int i = 0; i < pass.length(); ++i)
            {
                EEPROM.write(i + WIFI_PASS_INDEX, pass[i]);
                // Serial.print("Wrote: ");
                // Serial.println(pass[i]);
            }
            EEPROM.commit();
        }
    }

    void writeIDToMemory(std::string id)
    {
        if (isExternalEEPROM)
        {
            Config config = loadConfig();
            config.id = id;
            saveConfig(config);
        }
        else
        {
            Serial.println("clearing eeprom");
            for (int i = 0; i < ID_INDEX_LEN; ++i)
            {
                EEPROM.write(i + ID_INDEX, 0);
            }
            Serial.print("writing eeprom id:");
            Serial.println(id.c_str());
            for (int i = 0; i < id.length(); ++i)
            {
                EEPROM.write(i + ID_INDEX, id[i]);
            }
            EEPROM.commit();
        }
    }
    void saveConfig(Config config)
    {
        if (isExternalEEPROM)
        {

            DynamicJsonDocument doc = config.toJSON();
            // Lastly, you can print the resulting JSON to a String
            std::string json;
            serializeJson(doc, json);
            Serial.println("Saving...");
            Serial.println(json.c_str());

            for (int i = 0; i < json.length(); ++i)
            {
                externalEEPROM.write(i + CONFIG_INDEX, json[i]);
                // Serial.println(json[i]);
            }
            Serial.println("Saving [OK]");

            // externalEEPROM.put(CONFIG_INDEX, json);
        }
        else
        {
            Serial.println("No storage found");
        }
    }
    Config loadConfig()
    {
        Config config;
        if (isExternalEEPROM)
        {
            // todo: check if config written

            char raw[CONFIG_LENGTH];

            for (int i = 0; i < CONFIG_LENGTH; ++i)
            {
                raw[i] = char(externalEEPROM.read(i + CONFIG_INDEX));
            }
            Serial.println("Loading...");

            Serial.println(raw);

            std::string json = std::string(raw);

            Serial.println(json.c_str());
            config = Config::fromJSON(json);
        }
        else
        {
            // load empty config
            Serial.println("No storage found");
        }
        return config;
    }
}
