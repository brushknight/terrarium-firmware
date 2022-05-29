#include "net.h"
#include "secrets.h"
#include "eeprom.h"

namespace Net
{

    bool isConnected()
    {
        return WiFi.isConnected();
    }

    std::string setupAP()
    {
        WiFi.mode(WIFI_AP);
        IPAddress IP = {10, 0, 0, 1};
        IPAddress NMask = IPAddress (255, 255, 255, 0);
        WiFi.softAPConfig(IP, IP, NMask);
        // Connect to Wi - Fi network with SSID and password
        Serial.print("Setting AP (Access Point)…");
        // Remove the password parameter, if you want the AP (Access Point) to be open
        const char *ssid = "Terrarium Controller";
        const char *pass = "1234567890";
        WiFi.softAP(ssid, pass);

        IPAddress finalIp = WiFi.softAPIP();
        Serial.print("AP IP address: ");
        Serial.println(finalIp);
        return std::string(ssid);
    }

    void connect(Data *givenData, bool interactive)
    {

        if (WiFi.isConnected())
        {
            return;
        }

        WiFi.setAutoReconnect(true);

        Serial.println("Connection to wifi");

        // Status::setConnectingToWiFiStatus(Status::WORKING);

        WiFi.mode(WIFI_STA);
        WiFi.disconnect();
        delay(100);

        char buffer[100];
        sprintf(buffer, "%s#%s", "Terrarium controller ID", Eeprom::readIDFromMemory());

        WiFi.setHostname(buffer);

        int attempts = 0;

        // Serial.println(Eeprom::readWiFiSSIDFromMemory().c_str());
        // Serial.println(Eeprom::readWiFiPassFromMemory().c_str());

        (*givenData).metadata.wifiName = Eeprom::readWiFiSSIDFromMemory().c_str();

        WiFi.begin(Eeprom::readWiFiSSIDFromMemory().c_str(), Eeprom::readWiFiPassFromMemory().c_str());

        while (!WiFi.isConnected())
        {
            attempts++;
            if (interactive)
            {
                if (attempts % 10 == 0)
                {
                    // Display::renderConnectingToWifi(WIFI_SSID, attempts / 10);
                }
            }
            delay(1 * 100);

            Serial.println(statusToString(WiFi.status()));
            if (attempts >= 200)
            {
                attempts = 0;
                //WiFi.begin(WIFI_SSID, WIFI_PASS);
            }
        }

        // you're connected now, so print out the data:
        Serial.println("You're connected to the network");
        Serial.printf("Your IP is: %s\n", WiFi.localIP().toString());
        Serial.println(WiFi.localIP().toString());
        if (interactive)
        {
            // Display::renderConnectedToWifi(WIFI_SSID);
        }
        // Status::setConnectingToWiFiStatus(Status::IDLE);
    }

    char *statusToString(int code)
    {
        switch (code)
        {
        case WL_IDLE_STATUS:
            return "idle";
        case WL_NO_SSID_AVAIL:
            return "no SSID found";
        case WL_SCAN_COMPLETED:
            return "scan completed";
        case WL_CONNECTED:
            return "connected";
        case WL_CONNECT_FAILED:
            return "connection failed";
        case WL_CONNECTION_LOST:
            return "connection lost";
        case WL_DISCONNECTED:
            return "disconnected";

        default:
            return "undefined";
        }
    }
}