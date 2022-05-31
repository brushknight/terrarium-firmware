#include "http_server.h"

namespace HttpServer
{
    AsyncWebServer server(80);

    Data *data;

    void onPostReset(AsyncWebServerRequest *request)
    {
        Eeprom::clear();
    }

    void notFound(AsyncWebServerRequest *request)
    {
        request->send(404, "text/plain", "Not found");
    }

    void onRoot(AsyncWebServerRequest *request)
    {
        request->send_P(200, "text/html", CONTROLLER_CONFIG_FORM);
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
            
            Serial.println(p->name().c_str() == String("wifi_ssid").c_str());
            Serial.println(p->name().c_str() == String("wifi_pass").c_str());
            Serial.println(p->name().c_str() == String("id").c_str());

            if (p->name().c_str() == String("wifi_ssid").c_str())
            {

                config.wifiSSID = p->value().c_str();
            }

            if (p->name().c_str() == String("wifi_pass").c_str())
            {
                Serial.println(p->name().c_str() == String("wifi_pass").c_str());
                config.wifiPassword = p->value().c_str();
            }

            if (p->name().c_str() == String("id").c_str())
            {
                Serial.println(p->name().c_str() == String("id").c_str());
                config.id = p->value().c_str();
            }

            Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        }

        Eeprom::saveControllerConfig(config);

        // temporary

        DynamicJsonDocument json = config.toJSON();

        std::string requestBody;
        serializeJson(json, requestBody);

        request->send(200, "application/json", requestBody.c_str());

        // print success form

        // reboot

        // vTaskDelay(3 * 1000 / portTICK_PERIOD_MS);

        // ESP.restart();
    }

    void onPostClimateConfig(AsyncWebServerRequest *request)
    {
        // fetch request data

        // fullfill config

        // save config

        ClimateConfig config = Eeprom::loadClimateConfig();
        DynamicJsonDocument json = config.toJSON();

        std::string requestBody;
        serializeJson(json, requestBody);

        request->send(200, "application/json", requestBody.c_str());
    }

    void onGetMetrics(AsyncWebServerRequest *request)
    {
        DynamicJsonDocument doc(1024);

        doc["metadata"]["wifi"] = (*data).metadata.wifiName.c_str();
        doc["metadata"]["id"] = (*data).metadata.id.c_str();
        doc["metadata"]["time"]["hour"] = RealTime::getHour();
        doc["metadata"]["time"]["minute"] = RealTime::getMinute();
        doc["metadata"]["time"]["uptime"] = RealTime::getUptimeSec();
        doc["metadata"]["build_time"] = BUILD_TIME;

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

        std::string requestBody;
        serializeJson(doc, requestBody);

        request->send(200, "application/json", requestBody.c_str());
    }

    void start(Data *givenData, bool isSetupMode)
    {
        data = givenData;

        server.on("/", HTTP_GET, onRoot);
        server.on("/api/metrics", HTTP_GET, onGetMetrics);
        server.on("/api/config-controller", HTTP_GET, onGetControllerConfig);
        server.on("/api/config-controller", HTTP_POST, onPostControllerConfig);
        server.on("/api/reset", HTTP_POST, onPostReset);
        server.on("/api/config-climate", HTTP_GET, onGetClimateConfig);

        server.onNotFound(notFound);

        server.begin();
    }
}
