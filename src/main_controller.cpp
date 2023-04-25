#include <Arduino.h>
// #include "data.h"
#include "real_time.h"
// #include "http_server.h"
// #include "eeprom_wrapper.h"
// #include "status.h"
// #include "measure.h"
// #include "control.h"
// #include "zone.h"
// #include "net.h"
// #include "data_structures.h"

#define __BSD_VISIBLE 1

#include <sys/time.h>

static const char *TAG = "main";

// Data data;
// Control::Controller *hardwareController;
// Zone::Controller *zoneController;
// SystemConfig systemConfig;
// Measure::EnvironmentSensors *environmentSensors;

// bool initialSetupMode = false;

// void saveClimateConfig(void *parameter)
// {

//   for (;;)
//   {
//     if (zoneController->toBePersisted())
//     {
//       zoneController->pause();
//       Eeprom::saveZoneControllerJSON(zoneController);
//       zoneController->persisted();
//       zoneController->resume();
//     }

//     vTaskDelay(10 * 1000 / portTICK_PERIOD_MS);
//   }
// }

// void taskZoneControl(void *parameter)
// {
//   hardwareController->resetPorts();
//   zoneController->begin();

//   for (;;)
//   {
//     Time time = RealTime::getTimeObj();
//     ESP_LOGD(TAG, "zone control tick: %s", time.toString().c_str());
//     data.zones = zoneController->loopTick(time, environmentSensors, hardwareController);

//     vTaskDelay(10 * 1000 / portTICK_PERIOD_MS);
//   }
// }

// void taskCheckRtcBattery(void *parameter)
// {
//   for (;;)
//   {
//     data.RtcBatteryPercent = RealTime::getBatteryPercent();
//     data.RtcBatteryMilliVolt = RealTime::getBatteryVoltage();
//     ESP_LOGD(TAG, "RTC battery: %dmV %d%%", data.RtcBatteryMilliVolt, data.RtcBatteryPercent);

//     vTaskDelay(BATTERY_CHECK_INTERVAL_SEC * 1000 / portTICK_PERIOD_MS);
//   }
// }

// void taskResetEepromChecker(void *parameter)
// {
//   for (;;)
//   {
//     if (Eeprom::resetEepromChecker())
//     {
//       ESP.restart();
//     }
//     vTaskDelay(1 * 1000 / portTICK_PERIOD_MS);
//   }
// }

// void taskSyncRTCfromNTP(void *parameter)
// {
//   for (;;)
//   {
//     if (Net::isConnected())
//     {
//       RealTime::syncFromNTPOnce();
//       vTaskDelay(SYNC_RTC_SEC * 1000 / portTICK_PERIOD_MS);
//     }
//   }
// }

// void taskWatchNetworkStatus(void *parameter)
// {
//   for (;;)
//   {
//     data.WiFiStatus = Net::isConnected();
//     vTaskDelay(10 * 1000 / portTICK_PERIOD_MS);
//   }
// }

// void startWiFi(bool wifiAPMode)
// {
//   if (wifiAPMode)
//   {
//     // stand alone mode
//     Net::startInStandAloneMode();
//     Status::setPurple();
//   }
//   else
//   {
//     // normal wifi client mode
//     Net::startInNormalMode();
//     Net::setWiFiName(&data);
//   }
// }

// void startTasks()
// {
//   xTaskCreatePinnedToCore(
//       taskCheckRtcBattery,
//       "taskCheckRtcBattery",
//       1024 * 2,
//       NULL,
//       1,
//       NULL,
//       1);

//   xTaskCreatePinnedToCore(
//       taskResetEepromChecker,
//       "taskResetEepromChecker",
//       1024 * 2,
//       NULL,
//       1,
//       NULL,
//       1);

//   xTaskCreatePinnedToCore(
//       taskSyncRTCfromNTP,
//       "taskSyncRTCfromNTP",
//       1024 * 4,
//       NULL,
//       1,
//       NULL,
//       1);

//   xTaskCreatePinnedToCore(
//       taskWatchNetworkStatus,
//       "taskWatchNetworkStatus",
//       1024,
//       NULL,
//       2,
//       NULL,
//       0);

//   xTaskCreatePinnedToCore(
//       taskZoneControl,
//       "taskZoneControl",
//       1024 * 32,
//       NULL,
//       2,
//       NULL,
//       1);
// }

// void setupTask(void *parameter)
// {
//   Status::setup();
//   Status::setOrange();

//   ESP_LOGD(TAG, "Max alloc heap: %d", ESP.getMaxAllocHeap());
//   ESP_LOGD(TAG, "Max alloc psram: %d", ESP.getMaxAllocPsram());

//   Utils::scanForI2C();

//   // Think how to avoid this
//   Control::HardwareLayer hwl = Control::HardwareLayer();
//   Control::Controller hardwareControllerOrig = Control::Controller(&hwl);
//   hardwareController = &hardwareControllerOrig;

//   hardwareController->begin();
//   hardwareController->resetPorts();

//   ESP_LOGD(TAG, "Hardware startup reset performed");

//   data = Data();

//   initialSetupMode = !Eeprom::isMemorySet();

//   bool wasWiFiStarted = false;

//   // check if wifi required and start it
//   if (RealTime::isWiFiRequired() || systemConfig.wifiAPMode)
//   {
//     startWiFi(systemConfig.wifiAPMode);
//     wasWiFiStarted = true;
//   }

//   // sync time from NTP if requires
//   if (RealTime::isRtcSyncRequired())
//   {
//     RealTime::syncFromNTP();
//     RealTime::saveTimeToRTC();
//   }

//   std::string zoneControllerJSON = Eeprom::loadZoneControllerJSON();

//   zoneController->initFromJSON(&zoneControllerJSON);
//   // ESP_LOGD(TAG, "%s", zoneController->getTemperatureZone(0).slug.c_str());

//   data.metadata.id = Eeprom::loadSystemConfig().id;

//   environmentSensors->readSensors();

//   startTasks();

//   if (!wasWiFiStarted)
//   {
//     startWiFi(systemConfig.wifiAPMode);
//     wasWiFiStarted = true;
//   }

//   Net::setWiFiName(&data);
//   HttpServer::start(&data, zoneController, environmentSensors, false);
//   data.mac = Utils::getMac();

//   if (systemConfig.wifiAPMode)
//   {
//     Status::setPurple();
//   }
//   else
//   {
//     Status::setGreen();
//   }

//   ESP_LOGI(TAG, "[OK] Booting");

//   for (;;)
//   {
//     vTaskDelay(60 * 1000 / portTICK_PERIOD_MS);
//   }
// }

void setup()
{

  // Wire.begin();
  // delay(5000);

  delay(1000);
  // setup initial time (from RTC and will be adjusted later)
  //  RealTime::initRTC(systemConfig.timeZone, systemConfig.ntpEnabled);
  // RealTime::initRTC("UTC", false);
  // RealTime::syncFromRTC();
  // RealTime::printLocalTime();

        struct timeval tv;
        tv.tv_sec = 1682452286;// rtcDateTime.unixtime(); //  9:51:26 PM

        ESP_LOGD(TAG, "Timestamp from RTC: %d", tv.tv_sec);

        settimeofday(&tv, NULL);
        setenv("TZ", "UTC", 1); // https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
        // tzset();

        ESP_LOGD(TAG, "timestamp after saving: %d", tv.tv_sec);

        struct timeval tv2;
        time_t t;
        struct tm *info;

        gettimeofday(&tv2, NULL);
        t = tv2.tv_sec;

        info = localtime(&t);

        ESP_LOGI(TAG, "%d:%d:%d", info->tm_hour, info->tm_min, info->tm_sec);

  delay(5000);

  // Measure::enable();
  // Measure::EnvironmentSensors envSensors = Measure::EnvironmentSensors();
  // environmentSensors = &envSensors;
  // environmentSensors->scan();
  // delay(2000);
  // environmentSensors->scan();

  // Zone::Controller zoneControllerOriginal = Zone::Controller();
  // zoneController = &zoneControllerOriginal;

  // ExternalEEPROM externalEEPROM;

  // Eeprom::setup(&externalEEPROM);
  // systemConfig = Eeprom::loadSystemConfig();
  // RealTime::updateTimeZone(systemConfig.timeZone);

  // xTaskCreatePinnedToCore(
  //     setupTask,
  //     "setupTask",
  //     1024 * 38,
  //     NULL,
  //     100,
  //     NULL,
  //     0);

  // xTaskCreatePinnedToCore(
  //     saveClimateConfig,
  //     "saveClimateConfig",
  //     1024 * 32,
  //     NULL,
  //     100,
  //     NULL,
  //     0);


  for (;;)
  {
    vTaskDelay(60 * 1000 / portTICK_PERIOD_MS);
  }
}

void loop()
{
  delay(100000);
}