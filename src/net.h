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

    bool isConnected();
    char *statusToString(int code);

    void startInStandAloneMode(SystemConfig *systemConfig);
    void startInNormalMode(SystemConfig *systemConfig);
}

#endif