#ifndef TERRARIUM_LIGHT
#define TERRARIUM_LIGHT

#include "Arduino.h"
#include "config.h"
#include "real_time.h"

namespace Light
{
    void setup(ClimateConfig config);
    void control(int hour, int minute);

}

#endif