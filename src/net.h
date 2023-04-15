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
    bool isConnected();
    std::string setupAP();
    void connect();
    char *statusToString(int code);
    void setWiFiName(Data *givenData);

}

#endif