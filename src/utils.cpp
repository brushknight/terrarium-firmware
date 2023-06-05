#include "utils.h"

namespace Utils
{

    const int IS_SYSTEM_SET_INDEX = 0;
    const int WIFI_SSID_INDEX = 1;
    const int WIFI_SSID_LEN = 32;
    const int WIFI_PASS_INDEX = WIFI_SSID_INDEX + WIFI_SSID_LEN;
    const int WIFI_PASS_LEN = 32;

    void TCA9548A(uint8_t bus, bool verbose)
    {
        Wire.beginTransmission(0x70); // TCA9548A address
        Wire.write(1 << bus);         // send byte to select bus
        int status = Wire.endTransmission();
        if (verbose)
        {
            ESP_LOGD(TAG, "TCA9548A %d %d", bus, status);
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
                ESP_LOGD(TAG, "Found address: %d (0x%08x)", i, i);
                // Serial.print("Found address: ");
                // Serial.print(i, DEC);
                // Serial.print(" (0x");
                // Serial.print(i, HEX);
                // Serial.println(")");
                count++;
                delay(0.2);
            }
        }
        ESP_LOGD(TAG, "Found %d devices", count);
    }

    void scanForI2C()
    {
        ESP_LOGD(TAG, "[..] Scanning for I2C devices");
        scanForI2CLoop();
        for (int i = 0; i < 8; i++)
        {
            TCA9548A(i, true);
            delay(0.2);
            scanForI2CLoop();
        }
        ESP_LOGD(TAG, "[OK] Scanning for I2C devices");
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

    std::string getMac()
    {
        unsigned char mac[6] = {0};
        esp_efuse_mac_get_default(mac);
        esp_read_mac(mac, ESP_MAC_WIFI_STA);

        static char buffer[12];
        sprintf(buffer, "%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

        ESP_LOGD(TAG, "WiFi MAC: %s", buffer);

        return std::string(buffer);
    }

}
