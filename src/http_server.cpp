#include "http_server.h"

namespace HttpServer
{

    WebServer server(80);
    String header;

    void handleRoot()
    {
        server.send(200, "text/html", WIFI_FORM);
    }

    std::string setup()
    {
        server.on("/", handleRoot);
        server.on("/set-wifi", handleWiFiFormSubmit);
        server.begin();
        IPAddress IP = WiFi.softAPIP();
        return std::string(IP.toString().c_str());
    }

    void handleWiFiFormSubmit()
    {
        // String response_success = "<h1>Success</h1>";
        // response_success += "<h2>Device will restart in 3 seconds</h2>";

        // String response_error = "<h1>Error</h1>";
        // response_error += "<h2><a href='/'>Go back</a>to try again";

        String ssid = server.arg("ssid");
        String pass = server.arg("password");

        Utils::writeWiFiPassToMemory(std::string(pass.c_str()));
        Utils::writeWiFiSSIDToMemory(std::string(ssid.c_str()));

        Utils::setMemory();

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
