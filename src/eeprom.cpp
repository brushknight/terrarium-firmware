#include "eeprom.h"

namespace Eeprom
{

    const int SET_INDEX = 0;
    const int WIFI_SSID_INDEX = 1;
    const int WIFI_SSID_LEN = 32;
    const int WIFI_PASS_INDEX = WIFI_SSID_INDEX + WIFI_SSID_LEN;
    const int WIFI_PASS_LEN = 32;

    bool isMemorySet()
    {
        return EEPROM.read(SET_INDEX) == 1;
    }

    void setMemory()
    {
        EEPROM.write(SET_INDEX, 1);
        EEPROM.commit();
    }

    void resetMemory()
    {
        EEPROM.write(SET_INDEX, 0);
        EEPROM.commit();
    }

    std::string readWiFiSSIDFromMemory()
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

    std::string readWiFiPassFromMemory()
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

    void writeWiFiSSIDToMemory(std::string ssid)
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
            //Serial.print("Wrote: ");
            //Serial.println(ssid[i]);
        }
        EEPROM.commit();
    }

    void writeWiFiPassToMemory(std::string pass)
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
            //Serial.print("Wrote: ");
            //Serial.println(pass[i]);
        }
        EEPROM.commit();
    }
}
