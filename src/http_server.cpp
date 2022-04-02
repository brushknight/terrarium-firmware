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

    std::string setup(Data *givenData, bool isSetupMode)
    {
        data = givenData;
        server.on("/", handleRoot);
        server.on("/metrics", handlePromMetrics);
        server.on("/set-wifi", handleWiFiFormSubmit);
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

    void handlePromMetrics()
    {
        String message = "wifi_network ";
        Serial.println((*data).metadata.wifiName.c_str());
        message += (*data).metadata.wifiName.c_str();
        message += "\n";
        server.send(200, "text/plain", message);
    }

    void handleWiFiFormSubmit()
    {
        // String response_success = "<h1>Success</h1>";
        // response_success += "<h2>Device will restart in 3 seconds</h2>";

        // String response_error = "<h1>Error</h1>";
        // response_error += "<h2><a href='/'>Go back</a>to try again";

        String ssid = server.arg("ssid");
        String pass = server.arg("password");

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
