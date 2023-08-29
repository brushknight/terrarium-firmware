#include "hyt271s.h"

// Constructor
HYT271S::HYT271S(TwoWire *theWire)
{
  _wire = theWire;

  humidity = NAN;
  temp = NAN;
}

// Destructor
HYT271S::~HYT271S(void)
{
  if (i2c_dev)
  {
    delete i2c_dev; // remove old interface
  }
}

bool HYT271S::begin(uint8_t i2caddr)
{
  if (i2c_dev)
  {
    delete i2c_dev; // remove old interface
  }

  i2c_dev = new Adafruit_I2CDevice(i2caddr, _wire);

  if (!i2c_dev->begin())
  {
    return false;
  }

  return true;
}

float HYT271S::readTemperature(void)
{
  if (!readTempHum())
    return NAN;

  return temp;
}

float HYT271S::readHumidity(void)
{
  if (!readTempHum())
    return NAN;

  return humidity;
}

bool HYT271S::readTempHum(void)
{
  uint8_t readbuffer[4];

  if (!i2c_dev->read(readbuffer, 4, true))
    return false;

  // // Scale data to physical values
  humidity = (float)(((readbuffer[0] & 0x3F)<<8) +  readbuffer[1]) * (100.0 / 16383.0);
  temp = ((float)(((readbuffer[2] << 8) + readbuffer[3]) >> 2) * (165.0 / 16383.0)) - 40;

  return true;
}