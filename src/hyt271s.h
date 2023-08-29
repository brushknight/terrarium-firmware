#ifndef HYT271S_H
#define HYT271S_H

#include "Arduino.h"
#include <Adafruit_I2CDevice.h>

#define HYT271S_DEFAULT_ADDR 0x28 // HYT271-S I2C default address

extern TwoWire Wire; 

// Driver for the HYT271-S Temperature and Humidity sensor.
class HYT271S
{
public:
  HYT271S(TwoWire *theWire = &Wire);
  ~HYT271S(void);

  bool begin(uint8_t i2caddr = HYT271S_DEFAULT_ADDR);
  float readTemperature(void);
  float readHumidity(void);

private:

  float humidity;
  float temp;
  bool readTempHum(void);

protected:
  TwoWire *_wire;                     // Wire object
  Adafruit_I2CDevice *i2c_dev = NULL; // Pointer to I2C bus interface
};

#endif
