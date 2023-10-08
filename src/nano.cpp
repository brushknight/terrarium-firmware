#include <Arduino.h>
// #include "data.h"
// #include "real_time.h"
// #include "http_server.h"
// #include "eeprom_wrapper.h"
// #include "status.h"
#include "measure.h"
#include "actuator.h"
// #include "zone.h"
// #include "net.h"
// #include "data_structures.h"

// #define __BSD_VISIBLE 1

static const char *TAG = "main";


// Data data;
// Net::Network *net;
Actuator::Controller *hardwareController;
// Zone::ClimateService *zoneClimateService;
// SystemConfig *systemConfig;
Measure::EnvironmentSensors *environmentSensors;
// RealTime::RealTime *realTime;
// Eeprom::Eeprom *eeprom;

// void saveClimateConfig(void *parameter)
// {

//   for (;;)
//   {
//     if (zoneClimateService != NULL && zoneClimateService->toBePersisted())
//     {
//       zoneClimateService->pause();

//       DynamicJsonDocument doc = zoneClimateService->toJSON();
//       std::string json;
//       serializeJson(doc, json);

//       Status::setWarning();
//       eeprom->saveClimateConfig(&json);
//       zoneClimateService->persisted();
//       zoneClimateService->resume();
//       Status::setGreen();
//       delay(1000);
//     }

//     vTaskDelay(10 * 1000 / portTICK_PERIOD_MS);
//   }
// }

// void saveSystemConfig(void *parameter)
// {
//   for (;;)
//   {
//     ESP_LOGD(TAG, "save system config tick");
//     if (systemConfig != NULL && systemConfig->toBePersisted())
//     {
//       DynamicJsonDocument doc = systemConfig->toJSON();
//       std::string json;
//       serializeJson(doc, json);

//       ESP_LOGD(TAG, "save system config triggered %s", json.c_str());

//       Status::setWarning();
//       eeprom->saveSystemConfig(&json);
//       systemConfig->persisted();
//       Status::setGreen();
//       delay(1000);

//       // TODO do system reinit
//       realTime->updateTimeZOne(systemConfig->timeZone);
//       realTime->setNTPStatus(systemConfig->ntpEnabled);

//       if (net->isReconnectNeeded())
//       {
//         ESP_LOGD(TAG, "net reconnect required");
//         // disconnect
//         net->disconnect();
//         Status::setWarning();
//         // connect agian
//         net->connect();
//         Status::setGreen();
//       }
//     }

//     vTaskDelay(2 * 1000 / portTICK_PERIOD_MS);
//   }
// }

// void taskZoneControl(void *parameter)
// {
//   hardwareController->resetPorts();
//   zoneClimateService->begin();

//   for (;;)
//   {
//     Time time = realTime->getTimeObj();
//     ESP_LOGD(TAG, "zone control tick: %s", time.toString().c_str());
//     data.zones = zoneClimateService->loopTick(time, environmentSensors, hardwareController);

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

// void taskSyncRTCfromNTP(void *parameter)
// {
//   for (;;)
//   {
//     if (realTime->isNTP() && net->isConnected())
//     {

//       realTime->syncFromNTP(true);
//       realTime->saveTimeToRTC();
//     }
//     vTaskDelay(SYNC_RTC_SEC * 1000 / portTICK_PERIOD_MS);
//   }
// }

// void taskSyncFromRTC(void *parameter)
// {
//   // rtc.begin();
//   ESP_LOGD(TAG, "task to sync time from RTC stated");
//   realTime->syncFromRTC();
//   realTime->printTime();
//   vTaskDelete(NULL);
// }

// void taskWatchNetworkStatus(void *parameter)
// {
//   for (;;)
//   {
//     data.WiFiStatus = net->isConnected();
//     vTaskDelay(10 * 1000 / portTICK_PERIOD_MS);
//   }
// }

// void startWiFi()
// {
//   net->connect();

//   if (systemConfig->wifiAPMode)
//   {
//     Status::setPurple();
//   }
//   data.metadata.wifiName = systemConfig->wifiSSID;
// }

void startTasks()
{

//   xTaskCreatePinnedToCore(
//       saveClimateConfig,
//       "saveClimateConfig",
//       1024 * 32,
//       NULL,
//       100,
//       NULL,
//       0);

//   xTaskCreatePinnedToCore(
//       saveSystemConfig,
//       "savSystemConfig",
//       1024 * 32,
//       NULL,
//       100,
//       NULL,
//       0);
//   xTaskCreatePinnedToCore(
//       taskCheckRtcBattery,
//       "taskCheckRtcBattery",
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
}

// void setupTask(void *parameter)
// {
//   data = Data();

//   bool wasWiFiStarted = false;

//   SystemConfig systemConfigOriginal = SystemConfig(Utils::getMac());
//   systemConfig = &systemConfigOriginal;

//   int isSystemSet = eeprom->isSystemConfigSet();

//   ESP_LOGD(TAG, "is system set %d", isSystemSet);

//   if (eeprom->isSystemConfigSet())
//   {
//     std::string systemConfigJSON = eeprom->loadSystemConfg();
//     ESP_LOGD(TAG, "loaded json for system config: %s", systemConfigJSON.c_str());
//     systemConfig->updateFromJSON(&systemConfigJSON, false);
//   }

//   Net::Network netOriginal = Net::Network(systemConfig);
//   net = &netOriginal;

//   RealTime::RealTime rtcOriginal = RealTime::RealTime(systemConfig->timeZone, systemConfig->ntpEnabled, &rtc);
//   realTime = &rtcOriginal;

//   xTaskCreatePinnedToCore(
//       taskSyncFromRTC,
//       "taskSyncFromRTC",
//       1024 * 2,
//       NULL,
//       100,
//       NULL,
//       1);

//   Zone::ClimateService zoneClimateServiceOriginal = Zone::ClimateService();
//   zoneClimateService = &zoneClimateServiceOriginal;

//   // check if wifi required and start it
//   if (realTime->isRtcSyncRequired() || systemConfig->wifiAPMode)
//   {
//     startWiFi();
//     wasWiFiStarted = true;
//   }

//   while (!realTime->wasRTCChecked())
//   {
//     vTaskDelay(0.1 * 1000 / portTICK_PERIOD_MS);
//   }

//   // sync time from NTP if requires
//   if (realTime->isRtcSyncRequired() && !systemConfig->wifiAPMode && wasWiFiStarted)
//   {
//     realTime->syncFromNTP(false);
//     realTime->saveTimeToRTC();
//   }

//   if (eeprom->isClimateConfigSet())
//   {
//     std::string zoneControllerJSON = eeprom->loadClimateConfig();
//     zoneClimateService->initFromJSON(&zoneControllerJSON);
//   }

//   // ESP_LOGD(TAG, "%s", zoneController->getTemperatureZone(0).slug.c_str());

//   data.metadata.id = systemConfig->id;

//   environmentSensors->readSensors();

//   startTasks();

//   if (!wasWiFiStarted)
//   {
//     startWiFi();
//     wasWiFiStarted = true;
//   }

//   data.mac = Utils::getMac();

//   // Net::setWiFiName(&data);
//   HttpServer::start(&data, systemConfig, realTime, zoneClimateService, environmentSensors, eeprom, false);

//   if (systemConfig->wifiAPMode)
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

  Wire.begin(35, 36);

//   Eeprom::Eeprom eepromOriginal = Eeprom::Eeprom(&externalEEPROM, SystemConfig::jsonSize(), Zone::ClimateService::jsonSize());
//   eeprom = &eepromOriginal;
//   eeprom->setup();
//   if (eeprom->resetEepromsOnBootChecker())
//   {
//     Status::setGreen();
//     vTaskDelay(1 * 1000 / portTICK_PERIOD_MS);
//   }

  Actuator::HardwareLayer hwl = Actuator::HardwareLayer();
  Actuator::Controller hardwareControllerOrig = Actuator::Controller(&hwl);
  hardwareController = &hardwareControllerOrig;

  hardwareController->beginNano();
//   hardwareController->resetPorts();

hardwareController->turnSwitchOn(0);
delay(1000);
hardwareController->turnSwitchOff(0);
delay(1000);
hardwareController->turnSwitchOn(0);

//   // disabled for tests
//   // hardwareController->setBuzzer(0, NOTE_G3);
//   // vTaskDelay(0.1 * 1000 / portTICK_PERIOD_MS);
//   // hardwareController->setBuzzer(0, NOTE_G7);
//   // vTaskDelay(0.1 * 1000 / portTICK_PERIOD_MS);
//   // hardwareController->setBuzzer(0, 0);
//   // hardwareController->test();
//   // hardwareController->setFan(1, 50);

//   Measure::enable(&hwl);

//   ESP_LOGD(TAG, "Hardware startup reset performed");

  Measure::EnvironmentSensors envSensors = Measure::EnvironmentSensors();
  environmentSensors = &envSensors;
  environmentSensors->scan();

//   // delay(2000);
//   // environmentSensors->scan();

//   xTaskCreatePinnedToCore(
//       setupTask,
//       "setupTask",
//       1024 * 38,
//       NULL,
//       100,
//       NULL,
//       0);

  for (;;)
  {
    vTaskDelay(60 * 1000 / portTICK_PERIOD_MS);
  }
}

void loop()
{
  delay(100000);
}