#include "http_server.h"

namespace HttpServer
{

    WebServer server(80);
    String header;
    Data *data;

    void handleRoot()
    {
        server.send(200, "text/html", WIFI_FORM);
    }

    void handleConfigForm()
    {
        server.send(200, "text/html", CONFIG_FORM);
    }

    void handleUpdateForm()
    {
        server.send(200, "text/html", UPDATE_FORM);
    }

    void updateConnectionClosed()
    {
        server.sendHeader("Connection", "close");
        server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
        ESP.restart();
    }

    void updateHandler()
    {
        HTTPUpload &upload = server.upload();
        if (upload.status == UPLOAD_FILE_START)
        {
            Serial.setDebugOutput(true);
            Serial.printf("Update: %s\n", upload.filename.c_str());
            if (!Update.begin())
            { //start with max available size
                Update.printError(Serial);
            }
        }
        else if (upload.status == UPLOAD_FILE_WRITE)
        {
            if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
            {
                Update.printError(Serial);
            }
        }
        else if (upload.status == UPLOAD_FILE_END)
        {
            if (Update.end(true))
            { //true to set the size to the current progress
                Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
            }
            else
            {
                Update.printError(Serial);
            }
            Serial.setDebugOutput(false);
        }
        else
        {
            Serial.printf("Update Failed Unexpectedly (likely broken connection): status=%d\n", upload.status);
        }
    }

    std::string setup(Data *givenData, bool isSetupMode)
    {
        data = givenData;
        server.on("/", handleRoot);
        server.on("/config", handleConfigForm);
        server.on("/metrics", handlePromMetrics);
        server.on("/api/metrics", handleAPIMetrics);
        server.on("/api/config", HTTPMethod::HTTP_POST, handleConfigSubmission);
        server.on("/set-wifi", handleWiFiFormSubmit);
        server.on("/update", handleUpdateForm);
        server.on("/api/update", HTTP_POST, updateConnectionClosed, updateHandler);

        server.begin();

        IPAddress ip;
        if (isSetupMode)
        {
            ip = WiFi.softAPIP();
        }
        else
        {
            ip = WiFi.localIP();
        }
        return std::string(ip.toString().c_str());
    }

    void handleAPIMetrics()
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

        server.send(200, "application/json", requestBody.c_str());
    }

    void handlePromMetrics()
    {
        String message = "wifi_network ";
        Serial.println((*data).metadata.wifiName.c_str());
        message += (*data).metadata.wifiName.c_str();
        message += "\n";
        server.send(200, "text/plain", message);
    }

    void handleConfigSubmission()
    {
        String id = server.arg("id");

        Eeprom::writeIDToMemory(std::string(id.c_str()));

        server.send(200, "text/plain", "Config saved, controller will reboot in 3 seconds and will connect to wifi");

        vTaskDelay(3 * 1000 / portTICK_PERIOD_MS);

        ESP.restart();
    }

    void handleWiFiFormSubmit()
    {
        // String response_success = "<h1>Success</h1>";
        // response_success += "<h2>Device will restart in 3 seconds</h2>";

        // String response_error = "<h1>Error</h1>";
        // response_error += "<h2><a href='/'>Go back</a>to try again";

        String ssid = server.arg("ssid");
        String pass = server.arg("password");
        String id = server.arg("id");

        Eeprom::writeIDToMemory(std::string(id.c_str()));
        Eeprom::writeWiFiPassToMemory(std::string(pass.c_str()));
        Eeprom::writeWiFiSSIDToMemory(std::string(ssid.c_str()));

        Eeprom::setMemory();

        server.send(200, "text/plain", "Wifi credentials saved, controller will reboot in 3 seconds and will connect to wifi");

        vTaskDelay(3 * 1000 / portTICK_PERIOD_MS);

        ESP.restart();
    }

    void handleNotFound()
    {
        String message = "File Not Found\n\n";
        message += "URI: ";
        message += server.uri();
        message += "\nMethod: ";
        message += (server.method() == HTTP_GET) ? "GET" : "POST";
        message += "\nArguments: ";
        message += server.args();
        message += "\n";
        for (uint8_t i = 0; i < server.args(); i++)
        {
            message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
        }
        server.send(404, "text/plain", message);
    }

    void handleClientLoop()
    {
        server.handleClient();
    }

    void sendForm(WiFiClient client)
    {
        client.println(WIFI_FORM);
        client.println();
    }
}
