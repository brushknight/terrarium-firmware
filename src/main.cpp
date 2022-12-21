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

//#include <AsyncElegantOTA.h>

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
    Event::Time time = RealTime::getTimeObj();
    // debug info
    Serial.println(time.toString().c_str());
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
    data.RtcBatteryMilliVolt = RealTime::getBatteryVoltage();
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

void setupTask(void *parameter)
{
  Serial.begin(115200);
  Serial.println("Controller starting");

  Serial.printf("Max alloc heap: %d\n", ESP.getMaxAllocHeap());
  Serial.printf("Max alloc psram: %d\n", ESP.getMaxAllocPsram());

  Wire.begin();

  Serial.println("Scanning for i2c devices");
  Utils::scanForI2C();

  Status::setup();

  Eeprom::setup();
  // Eeprom::clearZoneControllerFull();
  Eeprom::resetEepromChecker();

  // Display::setup();

  SystemConfig systemConfig = Eeprom::loadSystemConfig();
  data = Data();

  initialSetupMode = !Eeprom::isMemorySet();

  if (initialSetupMode)
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
        1024 * 4,
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
    data.mac = Utils::getMac();
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
      1024 * 34,
      NULL,
      100,
      NULL,
      0);
}

void loop()
{
  delay(100000);
}
