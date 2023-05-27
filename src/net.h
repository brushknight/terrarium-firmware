#ifndef TERRARIUM_NET
#define TERRARIUM_NET

#include <SPI.h>
#include <WiFi.h>
#include "data.h"
#include "config.h"
#include <esp_wifi.h>

namespace Net
{
    static const char *TAG = "wifi";

    static const int secondsToWaitForWifiCheck = 5;

    class Network
    {
    private:
        SystemConfig *systemConfig;
        SystemConfig lastAppliedConfig;
        bool isConnectingStarted = false;

        void startAsClient()
        {
            ESP_LOGI(TAG, "[..] Connecting to wifi");

            if (WiFi.isConnected())
            {
                ESP_LOGI(TAG, "[OK] Connecting to wifi | IP: %s", WiFi.localIP().toString());
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
                    ESP_LOGI(TAG, "[..] Connecting to wifi | Another thread is connecting, waiting %d seconds", secondsToWaitForWifiCheck);
                    delay(1000 * secondsToWaitForWifiCheck);
                }
            }

            isConnectingStarted = true;

            char buffer[100];
            sprintf(buffer, "%s %s", "Terrarium controller", systemConfig->id.c_str());

            ESP_LOGD(TAG, "Hostname: %s", buffer);

            WiFi.setHostname(buffer);
            WiFi.setAutoReconnect(true);
            esp_wifi_set_ps(WIFI_PS_NONE);

            WiFi.mode(WIFI_STA);
            delay(5);
            WiFi.disconnect();
            delay(100);

            WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);

            int attempts = 0;

            std::string wifiSSID = systemConfig->wifiSSID;
            std::string wifiPassword = systemConfig->wifiPassword;

            ESP_LOGD(TAG, "SSID: %s, Passphrase: %s", wifiSSID.c_str(), wifiPassword.c_str());

            WiFi.begin(wifiSSID.c_str(), wifiPassword.c_str());

            while (!WiFi.isConnected())
            {
                ESP_LOGI(TAG, "[..] Connecting to wifi | 30 seconds no connection, reconecting");
                attempts++;
                delay(1 * 1000);

                if (attempts % 10 == 0)
                {
                    ESP_LOGI(TAG, "WiFi: %s", WiFi.status());
                    Serial.println(statusToString(WiFi.status()));
                }
                if (attempts > 30)
                {
                    ESP_LOGI(TAG, "[..] Connecting to wifi | 30 seconds no connection, reconecting");
                    attempts = 0;
                    WiFi.begin(wifiSSID.c_str(), wifiPassword.c_str());
                }
            }

            lastAppliedConfig = *systemConfig;

            isConnectingStarted = false;

            ESP_LOGI(TAG, "[OK] Connecting to wifi | IP: %s", WiFi.localIP().toString());
        }
        void startAsAccessPoint()
        {
            ESP_LOGI(TAG, "[..] Starting access point");

            // if wifiPassword is too short < 8, start without password to not break env
            std::string wifiPassphrase = systemConfig->wifiPassword;
            if (wifiPassphrase.length() < 8)
            {
                wifiPassphrase = "";
            }
            WiFi.softAP(systemConfig->wifiSSID.c_str(), wifiPassphrase.c_str());

            IPAddress finalIp = WiFi.softAPIP();
            ESP_LOGI(TAG, "IP Address: %s", finalIp.toString());

            lastAppliedConfig = *systemConfig;

            ESP_LOGI(TAG, "[OK] Starting access point");
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

    public:
        Network(SystemConfig *config)
        {
            systemConfig = config;
            lastAppliedConfig = *config;
        }
        bool isReconnectNeeded()
        {
            return lastAppliedConfig.isNetConfigEqual(systemConfig);
        }
        bool isConnected()
        {
            return WiFi.isConnected();
        }
        bool disconnect()
        {
            if (lastAppliedConfig.wifiAPMode)
            {
                return WiFi.softAPdisconnect(true);
            }
            else
            {
                return WiFi.disconnect(true, true);
            }
        }
        void connect()
        {
            if (systemConfig->wifiAPMode)
            {
                startAsAccessPoint();
            }
            else
            {
                startAsClient();
            }
        }
    };
}

#endif