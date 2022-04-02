#ifndef TERRARIUM_HTTP_SERVER
#define TERRARIUM_HTTP_SERVER

#include <WebServer.h>
#include "http_server_html.h"
#include "eeprom.h"

namespace HttpServer
{
    void handleRoot();
    void handleWiFiFormSubmit();
    void sendForm(WiFiClient client);
    void handleClientLoop();
    std::string setup();

}

#endif