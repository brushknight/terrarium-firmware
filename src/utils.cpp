#include "utils.h"

namespace Utils
{

    const int SET_INDEX = 0;
    const int WIFI_SSID_INDEX = 1;
    const int WIFI_SSID_LEN = 32;
    const int WIFI_PASS_INDEX = WIFI_SSID_INDEX + WIFI_SSID_LEN;
    const int WIFI_PASS_LEN = 32;

    void log(const char str[])
    {
        Serial.print(str);
    }

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

    void TCA9548A(uint8_t bus)
    {
        Wire.beginTransmission(0x70); // TCA9548A address is 0x70
        Wire.write(1 << bus);         // send byte to select bus
        Wire.endTransmission();
        Serial.print(bus);
    }

    void scanForI2C()
    {
        Serial.println();
        Serial.println("I2C scanner. Scanning ...");
        byte count = 0;

        Wire.begin();
        for (byte i = 1; i < 120; i++)
        {
            Wire.beginTransmission(i);
            if (Wire.endTransmission() == 0)
            {
                Serial.print("Found address: ");
                Serial.print(i, DEC);
                Serial.print(" (0x");
                Serial.print(i, HEX);
                Serial.println(")");
                count++;
                delay(1); // maybe unneeded?
            }             // end of good response
        }                 // end of for loop
        Serial.println("Done.");
        Serial.print("Found ");
        Serial.print(count, DEC);
        Serial.println(" device(s).");
    }

}
