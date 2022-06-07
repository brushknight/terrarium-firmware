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
#include "light.h"

Data data;
ClimateConfig config;

bool initialSetupMode = false;

void displayRender(void *parameter)
{
  // add display reset if needed each N minutes
  vTaskDelay(1 * 1000 / portTICK_PERIOD_MS);
  int frameCounter = 0;
  for (;;)
  {
    Display::render(data);
    vTaskDelay(DISPLAY_RENDER_INTERVAL_SEC * 1000 / portTICK_PERIOD_MS);
    frameCounter++;

    if (frameCounter == 60 * 10)
    {
      Display::registerIcons();
      frameCounter = 0;
    }
  }
}

void climateControl(void *parameter)
{
  Climate::setup(Eeprom::loadClimateConfig());
  Climate::enableSensors();
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

void lightControl(void *parameter)
{
  Serial.println("Starting Light control");
  ClimateConfig config = Eeprom::loadClimateConfig();
  Light::setup(config);
  Light::EventData eventData;

  for (;;)
  {
    eventData = Light::control(RealTime::getHour(), RealTime::getMinute());

    for (int i = 0; i < MAX_LIGHT_EVENTS; i++)
    {
      data.lightEvents[i] = eventData.active[i];
    }

    vTaskDelay(LIGHT_LOOP_INTERVAL_SEC * 1000 / portTICK_PERIOD_MS);
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
  ClimateConfig config = loadInitClimateConfig();
  Climate::setup(config);
  Climate::enableSensors();
  Status::setup();
  Utils::scanForI2C();

  // Eeprom::clear();
}

void demoLoop(void *parameter)
{
  ClimateConfig config = loadInitClimateConfig();
  config.climateZoneConfigs[0].name = "test demo zone";
  Eeprom::saveClimateConfig(config);

  for (;;)
  {
    Serial.println("loop starts");
    // heap_caps_check_integrity_all(true);
    config = Eeprom::loadClimateConfig();

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

void setupTask(void *parameter)
{
  Serial.begin(115200);
  Serial.println("Booting...");
  Wire.begin();
  Eeprom::setup();
  Eeprom::loadControllerConfig();

  data = Data();

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
  }
  else if (!Eeprom::isMemorySet())
  {
    // Setup mode
    initialSetupMode = true;
    data.initialSetup.apName = Net::setupAP();
    data.initialSetup.isInSetupMode = true;
    Serial.println(data.initialSetup.apName.c_str());
    Serial.println(data.initialSetup.ipAddr.c_str());

    data.initialSetup.ipAddr = std::string(WiFi.softAPIP().toString().c_str());
    HttpServer::start(&data, true);
    return;
  }
  else
  {

    // Eeprom::clearClimate();
    //  warmup
    Eeprom::loadClimateConfig();

    // Eeprom::resetMemory();

    Display::setup();
    xTaskCreatePinnedToCore(
        displayRender,
        "displayRender",
        4192,
        NULL,
        2,
        NULL,
        1);

    data.metadata.id = Eeprom::loadControllerConfig().id;

    if (RealTime::isWiFiRequired())
    {
      Net::connect();
      Net::setWiFiName(&data);
    }
    RealTime::setup(true);

    if (Eeprom::isClimateConfigSetExternalEEPROM())
    {

      xTaskCreatePinnedToCore(
          climateControl,
          "climateControl",
          1024 * 16,
          NULL,
          1,
          NULL,
          0);

      xTaskCreatePinnedToCore(
          lightControl,
          "lightControl",
          1024 * 16,
          NULL,
          1,
          NULL,
          0);
    }

    xTaskCreatePinnedToCore(
        netWatcher,
        "netWatcher",
        1024,
        NULL,
        2,
        NULL,
        0);
    Net::connect();
    Net::setWiFiName(&data);
    HttpServer::start(&data, false);
  }

  for (;;)
  {
    vTaskDelay(60 * 1000 / portTICK_PERIOD_MS);
  }
}

void setup()
{
  xTaskCreatePinnedToCore(
      setupTask,
      "setupTask",
      1024 * 16,
      NULL,
      100,
      NULL,
      0);
}

void loop()
{
  if (DEMO_BOARD)
  {
    Serial.println(F("Loop function"));
    delay(100000);
  }
  else
  {
    delay(100000);
  }
}
