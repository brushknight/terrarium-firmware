#include <Arduino.h>
#include "data.h"
#include "real_time.h"
#include "http_server.h"
#include "eeprom_wrapper.h"
#include "status.h"
#include "measure.h"
#include "actuator.h"
#include "zone.h"
#include "net.h"
#include "data_structures.h"

// #define __BSD_VISIBLE 1

static const char *TAG = "main";

RTC_DS3231 rtc;

Data data;
Net::Network *net;
Actuator::Controller *hardwareController;
Zone::ClimateService *zoneClimateService;
SystemConfig *systemConfig;
Measure::EnvironmentSensors *environmentSensors;
RealTime::RealTime *realTime;
Eeprom::Eeprom *eeprom;

void setup()
{

  Wire.begin(36, 35);

  // only in dev mode
  // ESP_LOGD(TAG, "Max alloc heap: %d", ESP.getMaxAllocHeap());
  // ESP_LOGD(TAG, "Max alloc psram: %d", ESP.getMaxAllocPsram());

  // enable only in debug mode
  // Utils::scanForI2C();

  Status::setup();
  Status::setOrange();

  ExternalEEPROM externalEEPROM;
  Eeprom::Eeprom eepromOriginal = Eeprom::Eeprom(&externalEEPROM, SystemConfig::jsonSize(), Zone::ClimateService::jsonSize());
  eeprom = &eepromOriginal;
  eeprom->setup();

  Actuator::HardwareLayer hwl = Actuator::HardwareLayer();
  Actuator::Controller hardwareControllerOrig = Actuator::Controller(&hwl);
  hardwareController = &hardwareControllerOrig;

  hardwareController->begin();
  hardwareController->resetPorts();

  Measure::enable(&hwl);

  Measure::EnvironmentSensors envSensors = Measure::EnvironmentSensors();
  environmentSensors = &envSensors;
  environmentSensors->scan();

  ESP_LOGD(TAG, "Hardware startup reset performed");

  hardwareController->test();

  // disabled for tests
  hardwareController->setBuzzer(0, NOTE_G3);
  vTaskDelay(0.1 * 1000 / portTICK_PERIOD_MS);
  hardwareController->setBuzzer(0, NOTE_G7);
  vTaskDelay(0.1 * 1000 / portTICK_PERIOD_MS);
  hardwareController->setBuzzer(0, 0);

  // hardwareController->test();

  Status::setGreen();
}

void loop()
{
  pinMode(BUTTON_RESET_EEPROM, INPUT);
  int resetButtonState = digitalRead(BUTTON_RESET_EEPROM);
  if (resetButtonState == 1)
  {
    Status::setWarning();
  }else{
    Status::setGreen();
  }
  delay(10);
}