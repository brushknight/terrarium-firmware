#ifndef TERRARIUM_SENSOR
#define TERRARIUM_SENSOR

#include "config.h"
#include "DHTStable.h"
#include "utils.h"
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>

namespace Sensor
{
    void scan();
}

#endif