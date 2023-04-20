#ifndef TERRARIUM_NET
#define TERRARIUM_NET

#include <SPI.h>
#include <WiFi.h>
// #include "display.h"
#include "data.h"
#include "eeprom_wrapper.h"
// #include "status.h"
#include <esp_wifi.h>

namespace Net
{
    static const char *TAG = "wifi";

    bool isConnected();
    char *statusToString(int code);
    void setWiFiName(Data *givenData);


    void startInStandAloneMode();
    void startInNormalMode();

}

#endif