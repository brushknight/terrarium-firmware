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
    Zone::Controller *zoneController;
    SystemConfig *systemConfig;
    Measure::EnvironmentSensors *environmentSensors;
    RealTime::RealTime *realTime;
    Eeprom::Eeprom *eeprom;

    bool wasClimateControllerSaved = false;

    AsyncWebServer *getServer()
    {
        return &server;
    }

    void onPostReset(AsyncWebServerRequest *request)
    {
        // Eeprom::clear();

        request->send(200, "text/plain", "Controller & climate configuration cleared, rebooting in 3 seconds");

        vTaskDelay(3 * 1000 / portTICK_PERIOD_MS);

        ESP.restart();
    }

    void onPostResetSystem(AsyncWebServerRequest *request)
    {
        // Eeprom::clearSystemSettings();

        request->send(200, "text/plain", "System configuration cleared, rebooting in 3 seconds");

        vTaskDelay(3 * 1000 / portTICK_PERIOD_MS);

        ESP.restart();
    }

    void onPostResetClimate(AsyncWebServerRequest *request)
    {
        // Eeprom::clearZoneController();

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
        ESP_LOGD(TAG, "From requested");
        request->send_P(200, "text/html", SETTINGS_FORM);
    }

    void onFormClimateConfig(AsyncWebServerRequest *request)
    {
        request->send_P(200, "text/html", CLIMATE_CONFIG_FORM);
    }

    void onGetSettings(AsyncWebServerRequest *request)
    {
        DynamicJsonDocument json = systemConfig->toJSON();

        json["wifi_password"] = "************";

        std::string requestBody;
        serializeJson(json, requestBody);

        request->send(200, "application/json", requestBody.c_str());
    }

    void onGetClimateConfig(AsyncWebServerRequest *request)
    {
        // Zone::Controller *config = Eeprom::loadZoneController();
        ESP_LOGD(TAG, "[..] Climate confor requested");
        zoneController->pause();
        DynamicJsonDocument json = zoneController->toJSON();
        ESP_LOGD(TAG, "Converted to json");

        std::string requestBody;
        serializeJson(json, requestBody);

        ESP_LOGD(TAG, "Serialized to string");
        ESP_LOGD(TAG, "%s", requestBody.c_str());

        request->send(200, "application/json", requestBody.c_str());
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        zoneController->resume();
        ESP_LOGD(TAG, "[OK] Climate confor requested");
    }

    void onPostSettings(AsyncWebServerRequest *request)
    {
        int params = request->params();
        for (int i = 0; i < params; i++)
        {
            AsyncWebParameter *p = request->getParam(i);

            // ESP_LOGI(TAG, "resieved param = %s -> %s", p->name(), p->value().c_str());

            if (p->name().compareTo(String("json")) == 0)
            {

                // Serial.println("POST: raw config");
                // Serial.println(p->value().c_str());

                std::string json = p->value().c_str();

                DynamicJsonDocument doc(SystemConfig::jsonSize());
                deserializeJson(doc, json);

                uint32_t timestamp = doc["timestamp"];
                // ESP_LOGI(TAG, "timestamp %d", timestamp);

                realTime->setTimestamp(timestamp, systemConfig->timeZone);

                systemConfig->updateFromJSON(&json);

                vTaskDelay(5 * 1000 / portTICK_PERIOD_MS);

                request->send(200, "text/plain", "Controller configuration updated, rebooting in 3 seconds");

                ESP.restart();
            }
        }

        request->send(502, "text/plain", "Internal server error");
    }

    void onPostClimateConfig(AsyncWebServerRequest *request)
    {
        ESP_LOGD(TAG, "incoming request");
        int params = request->params();
        zoneController->pause();

        for (int i = 0; i < params; i++)
        {
            AsyncWebParameter *p = request->getParam(i);

            ESP_LOGE(TAG, "%p, %d", p, p->size());

            if (p == NULL)
            {
                ESP_LOGE(TAG, "data is NULL");
            }
            else
            {

                const String param = p->name();

                if (param == NULL)
                {
                    ESP_LOGE(TAG, "data is NULL");
                }

                ESP_LOGD(TAG, "%s", p->name().c_str());

                if (p->name().compareTo(String("json_config")) == 0)
                {

                    if (p == NULL)
                    {
                        ESP_LOGE(TAG, "data is NULL");
                    }
                    else
                    {
                        std::string json = p->value().c_str();

                        ESP_LOGD(TAG, "%s", json.c_str());
                        // Serial.println("POST: raw config");
                        // Serial.println(p->value().c_str());
                        zoneController->updateFromJSON(&json);
                        // Eeprom::updateZoneControllerFromJson(&json);
                        // config = Zone::Controller::fromJSON(p->value().c_str());
                        // Eeprom::saveZoneController();
                        request->send(200, "text/plain", "Controller configuration updated, rebooting soon");
                    }
                }
            }
        }
        zoneController->resume();
        // this is probably wrong
        request->send(502, "text/plain", "Internal server error");
    }

    void onGetSensors(AsyncWebServerRequest *request)
    {
        DynamicJsonDocument doc(1024 * 2 + Measure::EnvironmentSensors::jsonSize());

        doc["sensors"] = environmentSensors->toJSON();

        std::string requestBody;
        serializeJson(doc, requestBody);

        request->send(200, "application/json", requestBody.c_str());
    }

    void onGetMetrics(AsyncWebServerRequest *request)
    {
        DynamicJsonDocument doc(1024 * 2 + Zone::ZonesStatuses::jsonSize());

        Time time = realTime->getTimeObj();

        doc["metadata"]["wifi"] = (*data).metadata.wifiName.c_str();
        doc["metadata"]["mac"] = (*data).mac.c_str();
        // doc["metadata"]["id"] = controllerConfig.id.c_str();
        doc["metadata"]["time"]["hour"] = time.hours;
        doc["metadata"]["time"]["minute"] = time.minutes;
        doc["metadata"]["time"]["uptime"] = realTime->getUptimeSec();
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

        doc["status"] = "ok";

        std::string requestBody;
        serializeJson(doc, requestBody);

        request->send(200, "application/json", requestBody.c_str());
    }

    void start(Data *givenData,
               SystemConfig *givenSystemConfig,
               RealTime::RealTime *giventRealTime,
               Zone::Controller *givenZoneController,
               Measure::EnvironmentSensors *givenEnvironmentSensors,
               Eeprom::Eeprom *givenEeprom,
               bool isSetupMode)
    {
        data = givenData;
        systemConfig = givenSystemConfig;
        zoneController = givenZoneController;
        environmentSensors = givenEnvironmentSensors;
        realTime = giventRealTime;
        eeprom = givenEeprom;

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

        ESP_LOGI(TAG, "[..] Starting server");

        if (isSetupMode)
        {
            ESP_LOGI(TAG, "Initial setup mode");
        }

        AsyncElegantOTA.begin(&server);

        server.begin();
        ESP_LOGI(TAG, "[OK] Starting server");
    }
}
