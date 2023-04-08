#include "http_server.h"

// fix for md5 error https://github.com/me-no-dev/ESPAsyncWebServer/issues/1085
/*

you modify in the library directory your_directory/Arduino/libraries/ESPAsyncWebServer-master/src/WebAuthentication.cpp

line 72 with this (the comment are the old line ......)

#ifdef ESP32
mbedtls_md5_init(&_ctx);
mbedtls_md5_update_ret (&_ctx,data,len);
mbedtls_md5_finish_ret(&_ctx,data);
mbedtls_internal_md5_process( &_ctx ,data);
// mbedtls_md5_starts(&_ctx);
// mbedtls_md5_update(&_ctx, data, len);
// mbedtls_md5_finish(&_ctx, _buf);

*/

// fix for freertos https://github.com/me-no-dev/AsyncTCP/pull/134
/*

libdeps/AsyncTCP/src/AsyncTCP.h

extern "C" {
    #include "freertos/FreeRTOS.h"
    #include "freertos/semphr.h"
    #include "lwip/pbuf.h"
}

In order to avoid #error "include FreeRTOS.h" must appear in source files before "include semphr.h"
semphr.h which located in include/freertos/freertos/semphr.h:74:3, "freertos/FreeRTOS.h"
library has to be included. PlatformIO on ubuntu-latest



*/
#include <AsyncElegantOTA.h>

namespace HttpServer
{
    AsyncWebServer server(80);

    Data *data;

    AsyncWebServer *getServer()
    {
        return &server;
    }

    void onPostReset(AsyncWebServerRequest *request)
    {
        Eeprom::clear();

        request->send(200, "text/plain", "Controller & climate configuration cleared, rebooting in 3 seconds");

        vTaskDelay(3 * 1000 / portTICK_PERIOD_MS);

        ESP.restart();
    }

    void onPostResetSystem(AsyncWebServerRequest *request)
    {
        Eeprom::clearSystemSettings();

        request->send(200, "text/plain", "System configuration cleared, rebooting in 3 seconds");

        vTaskDelay(3 * 1000 / portTICK_PERIOD_MS);

        ESP.restart();
    }

    void onPostResetClimate(AsyncWebServerRequest *request)
    {
        Eeprom::clearZoneController();

        request->send(200, "text/plain", "Climate configuration cleared, rebooting in 3 seconds");

        vTaskDelay(3 * 1000 / portTICK_PERIOD_MS);

        ESP.restart();
    }

    void notFound(AsyncWebServerRequest *request)
    {
        request->send(404, "text/plain", "Not found");
    }

    void onFormSettings(AsyncWebServerRequest *request)
    {
        request->send_P(200, "text/html", SETTINGS_FORM);
    }

    void onFormClimateConfig(AsyncWebServerRequest *request)
    {
        request->send_P(200, "text/html", CLIMATE_CONFIG_FORM);
    }

    void onGetSettings(AsyncWebServerRequest *request)
    {
        SystemConfig config = Eeprom::loadSystemConfig();
        DynamicJsonDocument json = config.toJSON();

        json["wifiPassword"] = "DELETED";

        std::string requestBody;
        serializeJson(json, requestBody);

        request->send(200, "application/json", requestBody.c_str());
    }

    void onGetClimateConfig(AsyncWebServerRequest *request)
    {
        Zone::Controller config = Eeprom::loadZoneController();
        DynamicJsonDocument json = config.toJSON();

        std::string requestBody;
        serializeJson(json, requestBody);

        request->send(200, "application/json", requestBody.c_str());
    }

    void onPostSettings(AsyncWebServerRequest *request)
    {

        // int params = request->params();
        // Serial.printf("Save settings, %d params", params);
        // for (int i = 0; i < params; i++)
        // {
        //     AsyncWebParameter *p = request->getParam(i);
        //     if (p->isFile())
        //     {
        //         Serial.printf("_FILE[%s]: %s, size: %u", p->name().c_str(), p->value().c_str(), p->size());
        //     }
        //     else if (p->isPost())
        //     {
        //         Serial.printf("_POST[%s]: %s", p->name().c_str(), p->value().c_str());
        //     }
        //     else
        //     {
        //         Serial.printf("_GET[%s]: %s", p->name().c_str(), p->value().c_str());
        //     }
        // }

        // if (request->hasParam("body", true))
        // {
        //     AsyncWebParameter *p = request->getParam("body", true);
        //     String json = p->value();
        //     Serial.println(json);
        // }
        // else
        // {
        //     AsyncWebServerResponse *response = request->beginResponse(400, "application/json", "{'msg':'No body'}");
        //     // handleCors(response);
        //     request->send(response);
        // }
        // return;


        // old settings code



        // ssid, pass, id
        SystemConfig config = Eeprom::loadSystemConfig();

        int params = request->params();
        for (int i = 0; i < params; i++)
        {
            AsyncWebParameter *p = request->getParam(i);

            if (p->name().compareTo(String("wifi_ssid")) == 0)
            {
                if (p->value().c_str() != "")
                {
                    Serial.printf("wifi_ssid: %s\n", p->value().c_str());
                    config.wifiSSID = p->value().c_str();
                }
            }

            if (p->name().compareTo(String("wifi_pass")) == 0)
            {
                if (p->value().c_str() != "")
                {
                    Serial.printf("wifi_pass: %s\n", p->value().c_str());
                    config.wifiPassword = p->value().c_str();
                }
            }

            if (p->name().compareTo(String("id")) == 0)
            {
                if (p->value().c_str() != "")
                {
                    Serial.printf("id: %s\n", p->value().c_str());
                    config.id = p->value().c_str();
                }
            }

            if (p->name().compareTo(String("name")) == 0)
            {
                if (p->value().c_str() != "")
                {
                    Serial.printf("name: %s\n", p->value().c_str());
                    config.name = p->value().c_str();
                }
            }
        }

        // do some validation

        Eeprom::saveSystemConfig(config);

        request->send(200, "text/plain", "Controller configuration updated, rebooting in 3 seconds");

        vTaskDelay(3 * 1000 / portTICK_PERIOD_MS);

        ESP.restart();
        
    }

    void onPostClimateConfig(AsyncWebServerRequest *request)
    {

        Zone::Controller config;
        int params = request->params();
        for (int i = 0; i < params; i++)
        {
            AsyncWebParameter *p = request->getParam(i);

            if (p->name().compareTo(String("json_config")) == 0)
            {

                Serial.println("POST: raw config");
                Serial.println(p->value().c_str());
                config = Zone::Controller::fromJSON(p->value().c_str());
                Eeprom::saveZoneController(config);

                request->send(200, "text/plain", "Controller configuration updated, rebooting soon");
            }
        }

        request->send(502, "text/plain", "Internal server error");
    }

    void onGetSensors(AsyncWebServerRequest *request)
    {
        DynamicJsonDocument doc(1024 * 2 + Measure::EnvironmentSensors::jsonSize());

        doc["sensors"] = (*data).sharedSensors.toJSON();

        std::string requestBody;
        serializeJson(doc, requestBody);

        request->send(200, "application/json", requestBody.c_str());
    }

    void onGetMetrics(AsyncWebServerRequest *request)
    {
        DynamicJsonDocument doc(1024 * 2 + Zone::ZonesStatuses::jsonSize());

        SystemConfig controllerConfig = Eeprom::loadSystemConfig();

        doc["metadata"]["wifi"] = (*data).metadata.wifiName.c_str();
        doc["metadata"]["mac"] = (*data).mac.c_str();
        doc["metadata"]["id"] = controllerConfig.id.c_str();
        doc["metadata"]["time"]["hour"] = RealTime::getHour();
        doc["metadata"]["time"]["minute"] = RealTime::getMinute();
        doc["metadata"]["time"]["uptime"] = RealTime::getUptimeSec();
        doc["metadata"]["build_time"] = BUILD_TIME;
        doc["system"]["rtc"]["percent"] = (*data).RtcBatteryPercent;
        doc["system"]["rtc"]["mV"] = (*data).RtcBatteryMilliVolt;

        doc["climate"] = (*data).zones.toJSON();

        std::string requestBody;
        serializeJson(doc, requestBody);

        request->send(200, "application/json", requestBody.c_str());
    }

    void onGetTerrispot(AsyncWebServerRequest *request)
    {
        DynamicJsonDocument doc(1024);

        SystemConfig controllerConfig = Eeprom::loadSystemConfig();

        doc["status"] = "ok";

        std::string requestBody;
        serializeJson(doc, requestBody);

        request->send(200, "application/json", requestBody.c_str());
    }

    void start(Data *givenData, bool isSetupMode)
    {
        data = givenData;

        server.on("/", HTTP_GET, onFormSettings);
        server.on("/settings", HTTP_GET, onFormSettings);
        server.on("/climate", HTTP_GET, onFormClimateConfig);
        server.on("/api/metrics", HTTP_GET, onGetMetrics);
        server.on("/api/settings", HTTP_GET, onGetSettings);
        server.on("/api/settings", HTTP_POST, onPostSettings);
        server.on("/api/reset", HTTP_POST, onPostReset);
        server.on("/api/sensors", HTTP_GET, onGetSensors);
        server.on("/api/reset-system", HTTP_POST, onPostResetSystem);
        server.on("/api/reset-climate", HTTP_POST, onPostResetClimate);
        server.on("/api/climate", HTTP_GET, onGetClimateConfig);
        server.on("/api/climate", HTTP_POST, onPostClimateConfig);
        server.on("/api/terrispot", HTTP_GET, onGetTerrispot);

        server.onNotFound(notFound);

        Serial.println("Starting server");

        if (isSetupMode)
        {
            Serial.println("Setup mode");
        }

        AsyncElegantOTA.begin(&server);

        server.begin();
        Serial.println("Server started [OK]");
    }
}
