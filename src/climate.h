#ifndef TERRARIUM_CLIMATE
#define TERRARIUM_CLIMATE

#include "Arduino.h"
#include "DHTStable.h"
#include "climate_data.h"
#include "config.h"
#include "data.h"
#include "utils.h"
#include "real_time.h"
//#include "thermistor.h"
//#include "HardwareSerial.h"

namespace Climate
{
    void setup(Config config);
    DataClimateZone *control(int hour, int minute);
    void enableSensors();
    void disableSensors();
    void rebootSensorsWithWait();
}

#endif