#include "net.h"
#include "secrets.h"
#include "utils.h"

namespace Net
{

    bool isConnected()
    {
        return WiFi.isConnected();
    }

    void connect(bool interactive)
    {

        if (WiFi.isConnected())
        {
            return;
        }

        // Status::setConnectingToWiFiStatus(Status::WORKING);

        WiFi.mode(WIFI_STA);
        WiFi.disconnect();
        delay(100);

        char buffer[100];
        sprintf(buffer, "%s#%d", "Terrarium controller ID", TERRARIUM_ID);

        WiFi.setHostname(buffer);

        int attempts = 0;

        Serial.println(Utils::readWiFiSSIDFromMemory().c_str());
        Serial.println(Utils::readWiFiPassFromMemory().c_str());

        WiFi.begin(Utils::readWiFiSSIDFromMemory().c_str(), Utils::readWiFiPassFromMemory().c_str());

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