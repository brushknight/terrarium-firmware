#include "http_server.h"

// #include <AsyncElegantOTA.h>

namespace HttpServer
{
    AsyncWebServer server(80);

    Data *data;

    void onPostReset(AsyncWebServerRequest *request)
    {
        Eeprom::clear();

        request->send(200, "text/plain", "Controller & climate configuration cleared, rebooting in 3 seconds");

        vTaskDelay(3 * 1000 / portTICK_PERIOD_MS);

        ESP.restart();
    }

    void onPostResetController(AsyncWebServerRequest *request)
    {
        Eeprom::clearController();

        request->send(200, "text/plain", "Controller configuration cleared, rebooting in 3 seconds");

        vTaskDelay(3 * 1000 / portTICK_PERIOD_MS);

        ESP.restart();
    }

    void onPostResetClimate(AsyncWebServerRequest *request)
    {
        Eeprom::clearClimate();

        request->send(200, "text/plain", "Climate configuration cleared, rebooting in 3 seconds");

        vTaskDelay(3 * 1000 / portTICK_PERIOD_MS);

        ESP.restart();
    }

    void notFound(AsyncWebServerRequest *request)
    {
        request->send(404, "text/plain", "Not found");
    }

    void onFormControllerConfig(AsyncWebServerRequest *request)
    {
        request->send_P(200, "text/html", CONTROLLER_CONFIG_FORM);
    }

    void onFormClimateConfig(AsyncWebServerRequest *request)
    {
        request->send_P(200, "text/html", CLIMATE_CONFIG_FORM);
    }

    void onGetControllerConfig(AsyncWebServerRequest *request)
    {
        ClimateConfig config = Eeprom::loadClimateConfig();
        DynamicJsonDocument json = config.toJSON();

        std::string requestBody;
        serializeJson(json, requestBody);

        request->send(200, "application/json", requestBody.c_str());
    }

    void onGetClimateConfig(AsyncWebServerRequest *request)
    {
        ClimateConfig config = Eeprom::loadClimateConfig();
        DynamicJsonDocument json = config.toJSON();

        std::string requestBody;
        serializeJson(json, requestBody);

        request->send(200, "application/json", requestBody.c_str());
    }

    void onPostControllerConfig(AsyncWebServerRequest *request)
    {
        // ssid, pass, id
        ControllerConfig config = Eeprom::loadControllerConfig();

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

        Eeprom::saveControllerConfig(config);

        request->send(200, "text/plain", "Controller configuration updated, rebooting in 3 seconds");

        vTaskDelay(3 * 1000 / portTICK_PERIOD_MS);

        ESP.restart();
    }

    void onPostClimateConfig(AsyncWebServerRequest *request)
    {

        ClimateConfig config;
        int params = request->params();
        for (int i = 0; i < params; i++)
        {
            AsyncWebParameter *p = request->getParam(i);

            if (p->name().compareTo(String("json_config")) == 0)
            {

                config = ClimateConfig::fromJSON(p->value().c_str());
                Eeprom::saveClimateConfig(config);

                request->send(200, "text/plain", "Controller configuration updated, rebooting soon");
            }
        }

        request->send(502, "text/plain", "Internal server error");
    }

    void onGetMetrics(AsyncWebServerRequest *request)
    {
        DynamicJsonDocument doc(1024 + MAX_LIGHT_EVENTS * LightEvent::jsonSize());

        ControllerConfig controllerConfig = Eeprom::loadControllerConfig();

        doc["metadata"]["wifi"] = (*data).metadata.wifiName.c_str();
        doc["metadata"]["id"] = controllerConfig.id.c_str();
        doc["metadata"]["time"]["hour"] = RealTime::getHour();
        doc["metadata"]["time"]["minute"] = RealTime::getMinute();
        doc["metadata"]["time"]["uptime"] = RealTime::getUptimeSec();
        doc["metadata"]["build_time"] = BUILD_TIME;
        doc["system"]["rtc_battery"] = (*data).RtcBatteryPercent;

        for (int i = 0; i < MAX_CLIMATE_ZONES; i++)
        {
            if ((*data).climateZones[i].isSet)
            {
                std::string slug = (*data).climateZones[i].slug;

                doc["climate"][slug]["name"] = (*data).climateZones[i].name;
                doc["climate"][slug]["heating"] = (*data).climateZones[i].heatingPhase;
                doc["climate"][slug]["temperature"] = (*data).climateZones[i].temperature;
                doc["climate"][slug]["targetTemperature"] = (*data).climateZones[i].targetTemperature;
                doc["climate"][slug]["humidity"] = (*data).climateZones[i].humidity;
            }
        }

        int lightEventIndex = 0;
        for (int i = 0; i < MAX_LIGHT_EVENTS; i++)
        {
            LightEvent event = (*data).lightEvents[i];
            if (event.isSet())
            {
                doc["light"][lightEventIndex] = event.toJSON();
                lightEventIndex++;
            }
        }

        std::string requestBody;
        serializeJson(doc, requestBody);

        request->send(200, "application/json", requestBody.c_str());
    }

    void start(Data *givenData, bool isSetupMode)
    {
        data = givenData;

        server.on("/", HTTP_GET, onFormControllerConfig);
        server.on("/climate", HTTP_GET, onFormClimateConfig);

        server.on("/api/metrics", HTTP_GET, onGetMetrics);
        server.on("/api/config-controller", HTTP_GET, onGetControllerConfig);
        server.on("/api/config-controller", HTTP_POST, onPostControllerConfig);
        server.on("/api/reset", HTTP_POST, onPostReset);
        server.on("/api/reset-controller", HTTP_POST, onPostResetController);
        server.on("/api/reset-climate", HTTP_POST, onPostResetClimate);
        server.on("/api/config-climate", HTTP_GET, onGetClimateConfig);
        server.on("/api/config-climate", HTTP_POST, onPostClimateConfig);

        server.onNotFound(notFound);

        Serial.println("Starting server");

        if (isSetupMode)
        {
            Serial.println("Setup mode");
        }

        // AsyncElegantOTA.begin(&server);

        server.begin();
        Serial.println("Server started [OK]");
    }
}
