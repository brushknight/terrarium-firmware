#include <Arduino.h>
#include <EEPROM.h>
#include "display.h"
#include "data.h"
#include "climate.h"
#include "real_time.h"
#include "secrets.h"
#include "http_server.h"
#include "eeprom.h"
#include "status.h"

Data data;
Config config;

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

void demoSetup()
{
  Config config = loadInitConfig();
  Climate::setup(config);
  Climate::enableSensors();
  Status::setup();
  Utils::scanForI2C();

  Eeprom::resetMemory();
}

void demoLoop(void *parameter)
{
  Config config = loadInitConfig();
  config.climateZoneConfigs[0].name = "test demo zone";
  Eeprom::saveConfig(config);
  Eeprom::writeWiFiSSIDToMemory("DEMO Board");

  for (;;)
  {
    Serial.println("loop starts");
    // heap_caps_check_integrity_all(true);
    config = Eeprom::loadConfig();

    Serial.println();
    Serial.println("config loaded");
    // Serial.println(config.climateZoneConfigs[0].name.c_str());
    // Serial.println(config.climateZoneConfigs[0].dht22SensorPins[0]);
    vTaskDelay(1 * 1000 / portTICK_PERIOD_MS);

    // Serial.println(Eeprom::readWiFiSSIDFromMemory().c_str());
    // Status::setError();
    // vTaskDelay(0.5 * 1000 / portTICK_PERIOD_MS);
    // Status::setWarning();
    // vTaskDelay(0.5 * 1000 / portTICK_PERIOD_MS);
    // Status::setPink();
    // vTaskDelay(0.5 * 1000 / portTICK_PERIOD_MS);
    // Climate::disableSensors();
    // Status::setBlue();
    // vTaskDelay(0.5 * 1000 / portTICK_PERIOD_MS);
    // Status::setPurple();
    // vTaskDelay(0.5 * 1000 / portTICK_PERIOD_MS);

    // digitalWrite(RELAY_0_PIN, HIGH);
    // digitalWrite(RELAY_1_PIN, HIGH);
    // digitalWrite(RELAY_2_PIN, HIGH);

    // Status::setPurple();
    // vTaskDelay(0.5 * 1000 / portTICK_PERIOD_MS);
    // Status::setBlue();
    // vTaskDelay(0.5 * 1000 / portTICK_PERIOD_MS);
    // Status::setPink();
    // vTaskDelay(0.5 * 1000 / portTICK_PERIOD_MS);
    // Climate::enableSensors();
    // Status::setWarning();
    // vTaskDelay(0.5 * 1000 / portTICK_PERIOD_MS);
    // Status::setError();
    // vTaskDelay(0.5 * 1000 / portTICK_PERIOD_MS);

    // digitalWrite(RELAY_0_PIN, LOW);
    // digitalWrite(RELAY_1_PIN, LOW);
    // digitalWrite(RELAY_2_PIN, LOW);
    Serial.println("loop ends");
  }
}

void httpServer(void *parameter)
{
  for (;;)
  {
    HttpServer::handleClientLoop();
  }
}

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  EEPROM.begin(512);
  Eeprom::setup();

  if (DEMO_BOARD)
  {
    demoSetup();
    xTaskCreatePinnedToCore(
        demoLoop,
        "demoLoop",
        50000,
        NULL,
        1,
        NULL,
        0);
    return;
  }

  data = Data();

  // Eeprom::resetMemory();

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

  Climate::setup(loadInitConfig());
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

  xTaskCreatePinnedToCore(
      httpServer,
      "httpServer",
      16384,
      NULL,
      2,
      NULL,
      1);
}

void loop()
{
  if (DEMO_BOARD)
  {
    Serial.println(F("Loop function"));
    delay(10000000000000);
  }
  else
  {
    delay(10000000000000);
  }
}
