#include <Arduino.h>
#include "display.h"
#include "data.h"
#include "climate.h"

Data data;

void TCA9548A(uint8_t bus)
{
  Wire.beginTransmission(0x70); // TCA9548A address is 0x70
  Wire.write(1 << bus);         // send byte to select bus
  Wire.endTransmission();
  Serial.print(bus);
}

void scanForI2C()
{
  Serial.println();
  Serial.println("I2C scanner. Scanning ...");
  byte count = 0;

  Wire.begin();
  for (byte i = 1; i < 120; i++)
  {
    Wire.beginTransmission(i);
    if (Wire.endTransmission() == 0)
    {
      Serial.print("Found address: ");
      Serial.print(i, DEC);
      Serial.print(" (0x");
      Serial.print(i, HEX);
      Serial.println(")");
      count++;
      delay(1); // maybe unneeded?
    }           // end of good response
  }             // end of for loop
  Serial.println("Done.");
  Serial.print("Found ");
  Serial.print(count, DEC);
  Serial.println(" device(s).");
}

void setup()
{
  Serial.begin(115200);
  Display::setup();
  Climate::setup();
  Climate::enableSensors();
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
  Display::render(data);
  sleep(1);
}