#include <Arduino.h>
#include "display.h"
#include "data.h"
#include "climate.h"
#include "real_time.h"

Data data;


void setup()
{
  Serial.begin(115200);
  Display::setup();
  Climate::setup();
  Climate::enableSensors();
  RealTime::setup(true);
  data = Data();

  data.climateZones[0].name = "hot corner";
  data.climateZones[0].humidity = 75.5;
  data.climateZones[0].temperature = 31.3;
  data.climateZones[0].heatingPhase = true;
  data.climateZones[0].heaterStatus = true;

  data.climateZones[1].name = "warm area";
  data.climateZones[1].humidity = 78.5;
  data.climateZones[1].temperature = 27.3;
  data.climateZones[1].heatingPhase = true;
  data.climateZones[1].heaterStatus = true;

  data.climateZones[2].name = "cold corner";
  data.climateZones[2].humidity = 85.1;
  data.climateZones[2].temperature = 23.4;
  data.climateZones[2].heatingPhase = false;
  data.climateZones[2].heaterStatus = false;

  data.WiFiStatus = true;
  data.BluetoothStatus = true;

  data.metadata.id = 5;
}

void loop()
{
  DataClimateZone *result = Climate::control(RealTime::getHour(), RealTime::getMinute());

  for (int i = 0; i < MAX_CLIMATE_ZONES; i++)
  {
    data.climateZones[i] = result[i];
  }

  Display::render(data);
  sleep(1);
}