#include <Arduino.h>
#include <EEPROM.h>
#include "display.h"
#include "data.h"
#include "climate.h"
#include "real_time.h"
#include "secrets.h"
#include "http_server.h"

Data data;

bool initialSetupMode = false;

void displayRender(void *parameter)
{
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
  EEPROM.begin(512);
  //Utils::resetMemory();
  Serial.begin(115200);

  Display::setup();

  xTaskCreatePinnedToCore(
      displayRender,
      "displayRender",
      2048,
      NULL,
      1,
      NULL,
      1);

  Serial.print("is controller set already");
  Serial.println(Utils::isMemorySet());

  if (!Utils::isMemorySet())
  {

    initialSetupMode = true;
    data.initialSetup.apName = Net::setupAP();
    data.initialSetup.isInSetupMode = true;
    data.initialSetup.ipAddr = HttpServer::setup();
    return;
  }

  // if (!Utils::isMemorySet())
  // {
  //   Utils::writeWiFiSSIDToMemory(WIFI_SSID);
  //   Utils::writeWiFiPassToMemory(WIFI_PASS);
  //   Utils::setMemory();
  // }

  Climate::setup();
  Climate::enableSensors();
  RealTime::setup(true);
  data = Data();

  data.climateZones[0].name = "hot corner";
  data.climateZones[0].humidity = 75.5;
  data.climateZones[0].temperature = 31.3;
  data.climateZones[0].heatingPhase = true;
  data.climateZones[0].heaterStatus = true;

  data.climateZones[1].name = "warm area";
  data.climateZones[1].humidity = 78.5;
  data.climateZones[1].temperature = 27.3;
  data.climateZones[1].heatingPhase = true;
  data.climateZones[1].heaterStatus = true;

  data.climateZones[2].name = "cold corner";
  data.climateZones[2].humidity = 85.1;
  data.climateZones[2].temperature = 23.4;
  data.climateZones[2].heatingPhase = false;
  data.climateZones[2].heaterStatus = false;

  data.WiFiStatus = false;
  data.BluetoothStatus = false;

  data.metadata.id = 5;

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
}

void loop()
{

  if (initialSetupMode)
  {
    HttpServer::handleClientLoop();
  }
  else
  {
    vTaskDelete(NULL);
  }
}