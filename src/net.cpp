#include "net.h"

namespace Net
{

    bool isConnectingStarted = false;

    bool isConnected()
    {
        return WiFi.isConnected();
    }

    std::string setupAP()
    {
        // WiFi.mode(WIFI_AP);

        // IPAddress local_ip = {10, 0, 0, 1};
        // IPAddress gateway = {10, 0, 0, 2};
        // IPAddress subnet = IPAddress(255, 255, 255, 0);
        // WiFi.softAPConfig(local_ip, gateway, subnet);
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

    void setWiFiName(Data *givenData)
    {
        std::string wifiSSID = Eeprom::loadSystemConfig().wifiSSID;

        (*givenData).metadata.wifiName = wifiSSID.c_str();
    }

    void connect()
    {

        if (WiFi.isConnected())
        {
            return;
        }

        if (isConnectingStarted)
        {
            while (true)
            {
                if (WiFi.isConnected())
                {
                    return;
                }
                Serial.println("Another thread is connecting, waiting.");
                delay(1000 * 5); // each 5s
            }
        }

        isConnectingStarted = true;

        char buffer[100];
        sprintf(buffer, "%s %s", "Terrarium controller", Eeprom::loadSystemConfig().id.c_str());
        WiFi.setHostname(buffer);

        WiFi.setAutoReconnect(true);

        Serial.println("Connection to wifi");

        // Status::setConnectingToWiFiStatus(Status::WORKING);

        WiFi.mode(WIFI_STA);
        delay(5);
        WiFi.disconnect();
        delay(100);

        WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);

        int attempts = 0;

        // Serial.println(Eeprom::readWiFiSSIDFromMemory().c_str());
        // Serial.println(Eeprom::readWiFiPassFromMemory().c_str());

        std::string wifiSSID = Eeprom::loadSystemConfig().wifiSSID;
        std::string wifiPassword = Eeprom::loadSystemConfig().wifiPassword;

        // Serial.println(wifiPassword.c_str());

        WiFi.begin(wifiSSID.c_str(), wifiPassword.c_str());

        while (!WiFi.isConnected())
        {
            attempts++;
            delay(1 * 500);

            if (attempts >= 10)
            {
                Serial.println(statusToString(WiFi.status()));
                delay(1 * 1000);
                attempts = 0;
                // WiFi.begin(WIFI_SSID, WIFI_PASS);
            }
        }

        // you're connected now, so print out the data:
        Serial.println("You're connected to the network");
        Serial.printf("Your IP is: %s\n", WiFi.localIP().toString());
        Serial.println(WiFi.localIP().toString());
        Serial.println(WiFi.getHostname());

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