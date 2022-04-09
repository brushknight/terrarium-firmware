#ifndef TERRARIUM_HTTP_SERVER
#define TERRARIUM_HTTP_SERVER

#include <WebServer.h>
#include "http_server_html.h"
#include "eeprom.h"
#include "data.h"
#include "ArduinoJson.h"

namespace HttpServer
{
    void handleRoot();
    void handleWiFiFormSubmit();
    void handlePromMetrics();
    void handleAPIMetrics();
    void sendForm(WiFiClient client);
    void handleClientLoop();
    std::string setup(Data *givenData, bool isSetupMode);

}

#endif