#include "http_server.h"

// fix for md5 error https://github.com/me-no-dev/ESPAsyncWebServer/issues/1085
// fix for freertos https://github.com/me-no-dev/AsyncTCP/pull/134
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
        // ssid, pass, id
        SystemConfig config = Eeprom::loadSystemConfig();

        int params = request->params();
        for (int i = 0; i < params; i++)
        {
            AsyncWebParameter *p = request->getParam(i);

            if (p->name().compareTo(String("wifi_ssid")) == 0)
            {

                config.wifiSSID = p->value().c_str();
            }

            if (p->name().compareTo(String("wifi_pass")) == 0)
            {
                config.wifiPassword = p->value().c_str();
            }

            if (p->name().compareTo(String("id")) == 0)
            {
                config.id = p->value().c_str();
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
