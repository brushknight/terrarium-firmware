#ifndef TERRARIUM_DISPLAY
#define TERRARIUM_DISPLAY

#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
#include "config.h"
#include "data.h"

namespace Display
{

     void setup();
     void render(Data data);
     void renderClimateZone(DataClimateZone data, int offset);
     char *floatToString(double value);
}
#endif