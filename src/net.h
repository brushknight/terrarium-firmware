#ifndef TERRARIUM_NET
#define TERRARIUM_NET

#include <SPI.h>
#include <WiFi.h>
#include "display.h"
// #include "status.h"

namespace Net
{
    bool isConnected();
    std::string setupAP();
    void connect(Data *givenData, bool interactive);
    char *statusToString(int code);

}

#endif