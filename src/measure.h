#ifndef TERRARIUM_MEASURE
#define TERRARIUM_MEASURE

#include "config.h"
#include "DHTStable.h"
#include "utils.h"
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>

namespace Measure
{
    const int SENSOR_TYPE_DHT22 = 0;
    const int SENSOR_TYPE_BME280 = 1;

    class Sensor
    {
    private:
        bool enabled = false;
        int port;
        int type;

    public:
        Sensor() {}
        Sensor(int p, int type)
        {
            port = p;
            enabled = true;
        }
        bool isSet()
        {
            return enabled;
        }
        bool isDHT22()
        {
            return type == SENSOR_TYPE_DHT22;
        };
        bool isBME280()
        {
            return type == SENSOR_TYPE_BME280;
        };
    };

    class Sensors
    {
    public:
        Sensor list[12];
    };

    Sensors scan();
}

#endif