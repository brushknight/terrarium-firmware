#include <Arduino.h>
#include <EEPROM.h>
#include "display.h"
#include "data.h"
#include "climate.h"
#include "real_time.h"
#include "secrets.h"
#include "http_server.h"
#include "eeprom.h"

Data data;

bool initialSetupMode = false;

void displayRender(void *parameter)
{
  // add display reset if needed each N minutes
  vTaskDelay(1 * 1000 / portTICK_PERIOD_MS);
  for (;;)
  {
    Display::render(data);
    vTaskDelay(DISPLAY_RENDER_INTERVAL_SEC * 1000 / portTICK_PERIOD_MS);
  }
}

void climateControl(void *parameter)
{
  for (;;)
  {
    DataClimateZone *result = Climate::control(RealTime::getHour(), RealTime::getMinute());

    for (int i = 0; i < MAX_CLIMATE_ZONES; i++)
    {
      data.climateZones[i] = result[i];
    }

    vTaskDelay(CLIMATE_LOOP_INTERVAL_SEC * 1000 / portTICK_PERIOD_MS);
  }
}

void netWatcher(void *parameter)
{
  for (;;)
  {
    data.WiFiStatus = Net::isConnected();
    vTaskDelay(10 * 1000 / portTICK_PERIOD_MS);
  }
}

void setup()
{
  data = Data();
  EEPROM.begin(512);
  //Eeprom::resetMemory();
  Serial.begin(115200);
  Display::setup();
  xTaskCreatePinnedToCore(
      displayRender,
      "displayRender",
      4192,
      NULL,
      1,
      NULL,
      1);

  if (!Eeprom::isMemorySet())
  {
    initialSetupMode = true;
    data.initialSetup.apName = Net::setupAP();
    data.initialSetup.isInSetupMode = true;
    data.initialSetup.ipAddr = HttpServer::setup(&data, true);
    Serial.println(data.initialSetup.apName.c_str());
    Serial.println(data.initialSetup.ipAddr.c_str());
    return;
  }

  data.metadata.id = Eeprom::readIDFromMemory();

  // if (!Utils::isMemorySet())
  // {
  //   Utils::writeWiFiSSIDToMemory(WIFI_SSID);
  //   Utils::writeWiFiPassToMemory(WIFI_PASS);
  //   Utils::setMemory();
  // }

  Climate::setup(loadConfig());
  Climate::enableSensors();
  if (RealTime::isWiFiRequired())
  {
    Net::connect(&data, false);
  }
  RealTime::setup(true);

  xTaskCreatePinnedToCore(
      climateControl,
      "climateControl",
      4192,
      NULL,
      1,
      NULL,
      0);

  xTaskCreatePinnedToCore(
      netWatcher,
      "netWatcher",
      1024,
      NULL,
      2,
      NULL,
      0);

  Net::connect(&data, false);
  HttpServer::setup(&data, false);
}

void loop()
{
  HttpServer::handleClientLoop();
}