#include <Arduino.h>
#include <EEPROM.h>
#include "display.h"
#include "data.h"
#include "real_time.h"
#include "http_server.h"
#include "eeprom_wrapper.h"
#include "status.h"
#include "measure.h"
#include "control.h"
#include "zone.h"

#include <Adafruit_BME280.h>

Data data;

bool initialSetupMode = false;

void taskFetchSensors(void *parameter)
{
  Measure::scan();

  for (;;)
  {
    Measure::readSensors();

    data.sharedSensors = *Measure::getSharedSensors();

    vTaskDelay(1 * 1000 / portTICK_PERIOD_MS);
  }
}

void taskZoneControl(void *parameter)
{
  Control::Controller controller = Control::Controller();
  controller.resetPorts();

  Zone::Controller zoneControllerToSave = Zone::Controller();

  Zone::Controller zoneController = Eeprom::loadZoneController();

  // DynamicJsonDocument doc = zoneController.toJSON();
  // std::string json;
  // serializeJson(doc, json);
  // Serial.println("Loaded: ");
  // Serial.println(json.c_str());

  for (;;)
  {
    std::string time = RealTime::getTime();
    data.zones = zoneController.loopTick(time, Measure::getSharedSensors(), &controller);

    vTaskDelay(5 * 1000 / portTICK_PERIOD_MS);
  }
}

void taskCheckRtcBattery(void *parameter)
{
  // add display reset if needed each N minutes

  for (;;)
  {
    data.RtcBatteryPercent = RealTime::getBatteryPercent();
    vTaskDelay(BATTERY_CHECK_INTERVAL_SEC * 1000 / portTICK_PERIOD_MS);
  }
}

void taskResetEepromChecker(void *parameter)
{

  for (;;)
  {
    if (Eeprom::resetEepromChecker())
    {
      Serial.println("EEPROM reset");
      ESP.restart();
    }
    vTaskDelay(1 * 1000 / portTICK_PERIOD_MS);
  }
}

// void taskDisplayRender(void *parameter)
// {
//   // add display reset if needed each N minutes
//   vTaskDelay(1 * 1000 / portTICK_PERIOD_MS);
//   int frameCounter = 0;
//   for (;;)
//   {
//     Display::render(data);
//     vTaskDelay(DISPLAY_RENDER_INTERVAL_SEC * 1000 / portTICK_PERIOD_MS);
//     frameCounter++;

//     if (frameCounter == 60 * 10)
//     {
//       Display::registerIcons();
//       frameCounter = 0;
//     }
//   }
// }

void taskSyncRTCfromNTP(void *parameter)
{
  for (;;)
  {
    RealTime::syncFromNTPOnce();
    vTaskDelay(SYNC_RTC_SEC * 1000 / portTICK_PERIOD_MS);
  }
}

void taskWatchNetworkStatus(void *parameter)
{
  for (;;)
  {
    data.WiFiStatus = Net::isConnected();
    vTaskDelay(10 * 1000 / portTICK_PERIOD_MS);
  }
}

void demoSetup()
{
  Utils::scanForI2C();
  Status::setup();
  RealTime::setup(true);
  Measure::scan();
  // Eeprom::clear();
}

void demoLoop(void *parameter)
{
  Zone::Controller config = Zone::Controller();
  Eeprom::saveZoneController(config);

  for (;;)
  {
    Serial.println("loop starts");
    // heap_caps_check_integrity_all(true);
    config = Eeprom::loadZoneController();

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
  Serial.println("Controller starting");
  Wire.begin();

  Serial.println("Scanning for i2c devices");
  Utils::scanForI2C();

  Status::setup();

  Eeprom::setup();
  Eeprom::resetEepromChecker();

  Display::setup();

  SystemConfig systemConfig = Eeprom::loadSystemConfig();
  data = Data();

  initialSetupMode = !Eeprom::isMemorySet();

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
  else if (initialSetupMode)
  {

    // Setup mode
    // data.initialSetup.apName =
    Net::setupAP();
    // data.initialSetup.isInSetupMode = true;
    Serial.println(data.initialSetup.apName.c_str());
    Serial.println(data.initialSetup.ipAddr.c_str());

    // data.initialSetup.ipAddr = std::string(WiFi.softAPIP().toString().c_str());
    HttpServer::start(&data, true);
    Status::setPurple();
  }
  else
  {
    Eeprom::loadZoneController();

    data.metadata.id = Eeprom::loadSystemConfig().id;

    // xTaskCreatePinnedToCore(
    //     taskDisplayRender,
    //     "taskDisplayRender",
    //     4192,
    //     NULL,
    //     2,
    //     NULL,
    //     1);

    xTaskCreatePinnedToCore(
        taskCheckRtcBattery,
        "taskCheckRtcBattery",
        1024,
        NULL,
        1,
        NULL,
        1);

    xTaskCreatePinnedToCore(
        taskResetEepromChecker,
        "taskResetEepromChecker",
        1024 * 2,
        NULL,
        1,
        NULL,
        1);

    xTaskCreatePinnedToCore(
        taskSyncRTCfromNTP,
        "taskSyncRTCfromNTP",
        1024 * 2,
        NULL,
        1,
        NULL,
        1);

    xTaskCreatePinnedToCore(
        taskWatchNetworkStatus,
        "taskWatchNetworkStatus",
        1024,
        NULL,
        2,
        NULL,
        0);

    if (RealTime::isWiFiRequired())
    {
      Net::connect();
      Net::setWiFiName(&data);
    }
    RealTime::setup(true);

    xTaskCreatePinnedToCore(
        taskFetchSensors,
        "taskFetchSensors",
        4096,
        NULL,
        2,
        NULL,
        1);

    xTaskCreatePinnedToCore(
        taskZoneControl,
        "taskZoneControl",
        1024 * 24,
        NULL,
        2,
        NULL,
        1);

    Net::connect();
    Net::setWiFiName(&data);
    HttpServer::start(&data, false);
    Serial.println("Controller started [OK]");
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
      1024 * 24,
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
