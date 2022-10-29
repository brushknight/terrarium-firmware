#ifndef TERRARIUM_HTTP_SERVER
#define TERRARIUM_HTTP_SERVER

#include <ESPAsyncWebServer.h>
#include "http_server_html.h"
#include "eeprom.h"
#include "data.h"
#include "ArduinoJson.h"
#include "real_time.h"
#include "zone.h"
#include "measure.h"
#include <Update.h>

namespace HttpServer
{
    void start(Data *givenData, bool isSetupMode);
    void notFound(AsyncWebServerRequest *request);
    void onFormControllerConfig(AsyncWebServerRequest *request);
    void onGetSystemConfig(AsyncWebServerRequest *request);
    void onPostSystemConfig(AsyncWebServerRequest *request);
    void onGetClimateConfig(AsyncWebServerRequest *request);
    void onGetSensors(AsyncWebServerRequest *request);
    void onPostReset(AsyncWebServerRequest *request);
    void onPostResetSystem(AsyncWebServerRequest *request);
    void onPostResetClimate(AsyncWebServerRequest *request);
    void onFormClimateConfig(AsyncWebServerRequest *request);
}

#endif