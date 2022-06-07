#ifndef TERRARIUM_DISPLAY
#define TERRARIUM_DISPLAY

#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
#include "config.h"
#include "data.h"
#include "real_time.h"

namespace Display
{

     void setup();
     void registerIcons();
     void render(Data data);
     void renderClimateZone(DataClimateZone data, int offset);
     void renderInitialSetup(InitialSetup data);
     char *floatToString(double value);
}
#endif