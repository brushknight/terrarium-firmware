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

bool initialSetupMode = false;

static const char *TAG = "main";

void taskFetchSensors(void *parameter)
{
  //Measure::scan();
  for (;;)
  {

    Measure::readSensors();

    data.sharedSensors = *Measure::getSharedSensors();

    vTaskDelay(1 * 1000 / portTICK_PERIOD_MS);
  }
}

void taskZoneControl(void *parameter)
{
  // Control::Controller controller = Control::Controller();
  hardwareController.resetPorts();

  Zone::Controller zoneControllerToSave = Zone::Controller();

  Zone::Controller zoneController = Eeprom::loadZoneController();
  // zoneController.begin();

  for (;;)
  {
    Time time = RealTime::getTimeObj();
    ESP_LOGD(TAG, "zone control tick: %s", time.toString().c_str());
    data.zones = zoneController.loopTick(time, Measure::getSharedSensors(), &hardwareController);

    vTaskDelay(5 * 1000 / portTICK_PERIOD_MS);
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
  Status::setup();
  Status::setOrange();

  ESP_LOGD(TAG, "Max alloc heap: %d", ESP.getMaxAllocHeap());
  ESP_LOGD(TAG, "Max alloc psram: %d", ESP.getMaxAllocPsram());

  Utils::scanForI2C();

  hardwareController.begin();
  hardwareController.resetPorts();

  ESP_LOGD(TAG, "Hardware reset performed");
  
  SystemConfig systemConfig = Eeprom::loadSystemConfig();
  data = Data();

  initialSetupMode = !Eeprom::isMemorySet();

  if (initialSetupMode)
  {
    // Utils::scanForI2C();
    Net::setupAP();
    ESP_LOGI(TAG, "Initial AP: %s, ip  to connect: %s", data.initialSetup.apName.c_str(), data.initialSetup.ipAddr.c_str());

    HttpServer::start(&data, true);
    Status::setPurple();
  }
  else
  {
    if (RealTime::isWiFiRequired())
    {
      Net::connect();
      Net::setWiFiName(&data);
      RealTime::syncFromNTP();
      RealTime::saveTimeToRTC();
    }

    Eeprom::loadZoneController();

    data.metadata.id = Eeprom::loadSystemConfig().id;

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
        1024 * 32,
        NULL,
        2,
        NULL,
        1);

    Net::connect();
    Net::setWiFiName(&data);
    HttpServer::start(&data, false);
    data.mac = Utils::getMac();
    ESP_LOGD(TAG, "MAC: %s", data.mac);
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
  Measure::scan();
  delay(2000);
  Measure::scan();

  Eeprom::setup();
  SystemConfig systemConfig = Eeprom::loadSystemConfig();
  systemConfig.timeZone;

  // setup initial time (from RTC and will be adjusted later)
  RealTime::initRTC(systemConfig.timeZone, systemConfig.ntpEnabled);
  RealTime::syncFromRTC();
  RealTime::printLocalTime();
  delay(5000);

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