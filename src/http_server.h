#ifndef TERRARIUM_HTTP_SERVER
#define TERRARIUM_HTTP_SERVER

#include <ESPAsyncWebServer.h>
#include "http_server_html.h"
#include "eeprom.h"
#include "data.h"
#include "ArduinoJson.h"
#include "real_time.h"
#include <Update.h>

namespace HttpServer
{
    void start(Data *givenData, bool isSetupMode);
    void notFound(AsyncWebServerRequest *request);
    void onRoot(AsyncWebServerRequest *request);
    void onGetControllerConfig(AsyncWebServerRequest *request);
    void onPostControllerConfig(AsyncWebServerRequest *request);
    void onGetClimateConfig(AsyncWebServerRequest *request);
    void onPostReset(AsyncWebServerRequest *request);
    void onFormClimateConfig(AsyncWebServerRequest *request);
}

#endif