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

#include <sys/time.h>

// #include <AsyncElegantOTA.h>

Data data;

bool initialSetupMode = false;

void taskFetchSensors(void *parameter)
{
  // Measure::scan();

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
  Eeprom::setup();
  Eeprom::resetEepromChecker();

  // Display::setup();

  SystemConfig systemConfig = Eeprom::loadSystemConfig();
  data = Data();

  initialSetupMode = !Eeprom::isMemorySet();

  if (initialSetupMode)
  {
    Serial.println("Scanning for i2c devices");
    Utils::scanForI2C();
    Net::setupAP();
    Serial.println(data.initialSetup.apName.c_str());
    Serial.println(data.initialSetup.ipAddr.c_str());

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

// void setup()
// {

//   Serial.begin(115200);
//   Serial.println("Controller starting");

//   // Serial.printf("Max alloc heap: %d\n", ESP.getMaxAllocHeap());
//   // Serial.printf("Max alloc psram: %d\n", ESP.getMaxAllocPsram());

//   Wire.begin();
//   Measure::enable();
//   delay(5000);
//   Measure::scan();
//   delay(2000);
//   Measure::scan();

//   // setup initial time (from RTC and will be adjusted later)
//   RealTime::initRTC();
//   RealTime::syncFromRTC();
//   RealTime::printLocalTime();
//   delay(5000);

//   xTaskCreatePinnedToCore(
//       setupTask,
//       "setupTask",
//       1024 * 34,
//       NULL,
//       100,
//       NULL,
//       0);
// }

// void loop()
// {
//   delay(100000);
// }

int dimmerPin1 = 32;
int dimmerPin2 = 33;

void setup()
{

  Serial.begin(115200);
  Serial.println("Controller starting");
  pinMode(dimmerPin1, OUTPUT); // sets the pin as output
  pinMode(dimmerPin2, OUTPUT);
}

void loop()
{

  for (int i = 0; i < 255; i++)
  {
    Serial.println(i);
    analogWrite(dimmerPin1, i);
    analogWrite(dimmerPin2, i);
    delay(10);
  }
  for (int i = 255; i > 0; i--)
  {
    Serial.println(i);
    analogWrite(dimmerPin1, i);
    analogWrite(dimmerPin2, i);
    delay(10);
  }
}
