#include <Arduino.h>
#include <EEPROM.h>
#include "data.h"
#include "real_time.h"
#include "http_server.h"
#include "eeprom_wrapper.h"
#include "status.h"
#include "measure.h"
#include "control.h"
#include "zone.h"
#include "data_structures.h"
#include <Adafruit_BME280.h>
#include <sys/time.h>

Data data;
Control::Controller hardwareController;
Zone::Controller zoneController = Zone::Controller();
SystemConfig systemConfig;
Measure::EnvironmentSensors environmentSensors = Measure::EnvironmentSensors();

bool initialSetupMode = false;

static const char *TAG = "main";

// void taskFetchSensors(void *parameter)
// {

//   // read sensors once
//   environmentSensors.readSensors();

//   // // Measure::scan();
//   // for (;;)
//   // {

//   //   environmentSensors.readSensors();

//   //   data.sharedSensors = environmentSensors;

//   //   vTaskDelay(5 * 1000 / portTICK_PERIOD_MS);
//   // }
// }

void saveClimateConfig(void *parameter)
{

  for (;;)
  {
    if (zoneController.toBePersisted())
    {
      zoneController.pause();
      Eeprom::saveZoneControllerJSON(&zoneController);
      zoneController.persisted();
      zoneController.resume();
    }

    vTaskDelay(10 * 1000 / portTICK_PERIOD_MS);
  }
}

void taskZoneControl(void *parameter)
{
  // Control::Controller controller = Control::Controller();
  hardwareController.resetPorts();

  // Zone::Controller *zoneController = Eeprom::loadZoneController();
  // Eeprom::loadZoneController()->begin();

  zoneController.begin();

  for (;;)
  {
    Time time = RealTime::getTimeObj();
    ESP_LOGD(TAG, "zone control tick: %s", time.toString().c_str());
    data.zones = zoneController.loopTick(time, &environmentSensors, &hardwareController);

    vTaskDelay(10 * 1000 / portTICK_PERIOD_MS);
  }
}

void taskCheckRtcBattery(void *parameter)
{
  for (;;)
  {
    data.RtcBatteryPercent = RealTime::getBatteryPercent();
    data.RtcBatteryMilliVolt = RealTime::getBatteryVoltage();
    ESP_LOGD(TAG, "RTC battery: %dmV %d%%", data.RtcBatteryMilliVolt, data.RtcBatteryPercent);

    vTaskDelay(BATTERY_CHECK_INTERVAL_SEC * 1000 / portTICK_PERIOD_MS);
  }
}

void taskResetEepromChecker(void *parameter)
{
  for (;;)
  {
    if (Eeprom::resetEepromChecker())
    {
      ESP.restart();
    }
    vTaskDelay(1 * 1000 / portTICK_PERIOD_MS);
  }
}

void taskSyncRTCfromNTP(void *parameter)
{
  for (;;)
  {
    if (Net::isConnected())
    {
      RealTime::syncFromNTPOnce();
      vTaskDelay(SYNC_RTC_SEC * 1000 / portTICK_PERIOD_MS);
    }
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

void startWiFi(bool wifiAPMode)
{
  if (wifiAPMode)
  {
    // stand alone mode
    Net::startInStandAloneMode();
    Status::setPurple();
  }
  else
  {
    // normal wifi client mode
    Net::startInNormalMode();
    Net::setWiFiName(&data);
  }
}

void startTasks()
{
  xTaskCreatePinnedToCore(
      taskCheckRtcBattery,
      "taskCheckRtcBattery",
      1024 * 2,
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

  // xTaskCreatePinnedToCore(
  //     taskFetchSensors,
  //     "taskFetchSensors",
  //     4096,
  //     NULL,
  //     2,
  //     NULL,
  //     1);

  xTaskCreatePinnedToCore(
      taskZoneControl,
      "taskZoneControl",
      1024 * 32,
      NULL,
      2,
      NULL,
      1);
}

void setupTask(void *parameter)
{
  Status::setup();
  Status::setOrange();

  ESP_LOGD(TAG, "Max alloc heap: %d", ESP.getMaxAllocHeap());
  ESP_LOGD(TAG, "Max alloc psram: %d", ESP.getMaxAllocPsram());

  Utils::scanForI2C();

  hardwareController.begin();
  hardwareController.resetPorts();

  ESP_LOGD(TAG, "Hardware startup reset performed");

  data = Data();

  initialSetupMode = !Eeprom::isMemorySet();

  bool wasWiFiStarted = false;

  // check if wifi required and start it
  if (RealTime::isWiFiRequired() || systemConfig.wifiAPMode)
  {
    startWiFi(systemConfig.wifiAPMode);
    wasWiFiStarted = true;
  }

  // sync time from NTP if requires
  if (RealTime::isRtcSyncRequired())
  {
    RealTime::syncFromNTP();
    RealTime::saveTimeToRTC();
  }

  std::string zoneControllerJSON = Eeprom::loadZoneControllerJSON();

  zoneController.initFromJSON(&zoneControllerJSON);
  ESP_LOGD(TAG, "%s", zoneController.getTemperatureZone(0).slug.c_str());

  data.metadata.id = Eeprom::loadSystemConfig().id;

  environmentSensors.readSensors();

  startTasks();

  if (!wasWiFiStarted)
  {
    startWiFi(systemConfig.wifiAPMode);
    wasWiFiStarted = true;
  }

  Net::setWiFiName(&data);
  HttpServer::start(&data, &zoneController, &environmentSensors, false);
  data.mac = Utils::getMac();

  if (systemConfig.wifiAPMode)
  {
    Status::setPurple();
  }
  else
  {
    Status::setGreen();
  }

  ESP_LOGI(TAG, "[OK] Booting");

  for (;;)
  {
    vTaskDelay(60 * 1000 / portTICK_PERIOD_MS);
  }
}

void setup()
{
  Serial.begin(115200); // to be removed?
  ESP_LOGI(TAG, "[..] Booting");

  Wire.begin();
  Measure::enable();
  delay(5000);
  environmentSensors.scan();
  delay(2000);
  environmentSensors.scan();

  Eeprom::setup();
  systemConfig = Eeprom::loadSystemConfig();

  // setup initial time (from RTC and will be adjusted later)
  RealTime::initRTC(systemConfig.timeZone, systemConfig.ntpEnabled);
  RealTime::syncFromRTC();
  RealTime::printLocalTime();
  delay(5000);

  xTaskCreatePinnedToCore(
      setupTask,
      "setupTask",
      1024 * 32,
      NULL,
      100,
      NULL,
      0);

  xTaskCreatePinnedToCore(
      saveClimateConfig,
      "saveClimateConfig",
      1024 * 32,
      NULL,
      100,
      NULL,
      0);
}

void loop()
{
  delay(100000);
}