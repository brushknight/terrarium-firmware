#include "utils.h"

namespace Utils
{

    const int IS_SYSTEM_SET_INDEX = 0;
    const int WIFI_SSID_INDEX = 1;
    const int WIFI_SSID_LEN = 32;
    const int WIFI_PASS_INDEX = WIFI_SSID_INDEX + WIFI_SSID_LEN;
    const int WIFI_PASS_LEN = 32;

    void log(const char str[])
    {
        Serial.println(str);
    }

    void TCA9548A(uint8_t bus, bool verbose)
    {
        Wire.beginTransmission(0x70); // TCA9548A address
        Wire.write(1 << bus);         // send byte to select bus
        int status = Wire.endTransmission();
        if (verbose)
        {
            Serial.printf("TCA9548A %d %d\n", bus, status);
        }
    }

    void scanForI2CLoop()
    {
        byte count = 0;
        for (byte i = 0; i < 120; i++)
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
                delay(1);
            }
        }
        Serial.printf("Found %d devices\n", count);
    }

    void scanForI2C()
    {
        Serial.println();
        Serial.println("I2C scanner. Scanning ...");

        scanForI2CLoop();

        for (int i = 0; i < 8; i++)
        {
            TCA9548A(i, true);
            delay(1);
            scanForI2CLoop();
        }
        Serial.println("------------");
    }

    std::string hourMinuteToString(int hour, int minute)
    {
        static char buffer[10];
        sprintf(buffer, "%02d:%02d", hour, minute);
        // Serial.println(buffer);
        return std::string(buffer);
    }

    bool checkScheduleTimeWindow(std::string now, std::string since, std::string until)
    {

        // Serial.printf("%s, %s, %s\n", since.c_str(), now.c_str(), until.c_str());
        if (since.compare(until) < 0)
        {
            // Serial.printf("since.compare(now) <= 0 && until.compare(now) > 0\n");
            return since.compare(now) <= 0 && until.compare(now) > 0;
        }
        else
        {
            // Serial.printf("since.compare(now) <= 0 || until.compare(now) > 0\n");
            return since.compare(now) <= 0 || until.compare(now) > 0;
        }
    }

    // Function that gets current epoch time
    unsigned long getTimestamp()
    {
        time_t now;
        struct tm timeinfo;
        if (!getLocalTime(&timeinfo))
        {
            // Serial.println("Failed to obtain time");
            return (0);
        }
        time(&now);
        return now;
    }

}
