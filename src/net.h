#ifndef TERRARIUM_NET
#define TERRARIUM_NET

#include <SPI.h>
#include <WiFi.h>
#include "display.h"
#include "eeprom_wrapper.h"
// #include "status.h"

namespace Net
{
    bool isConnected();
    std::string setupAP();
    void connect();
    char *statusToString(int code);
    void setWiFiName(Data *givenData);

}

#endif