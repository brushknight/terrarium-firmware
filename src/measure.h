#ifndef TERRARIUM_MEASURE
#define TERRARIUM_MEASURE

#include "config.h"
#include "DHTStable.h"
#include "utils.h"
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>

namespace Measure
{
    const int SENSOR_TYPE_DHT22 = 22;
    const int SENSOR_TYPE_BME280 = 280;

    bool readBME280(int port, float *t, float *h);
    bool readDHT22(int port, float *t, float *h);

    class EnvironmentSensor
    {
    private:
        int port = -1;
        int type = -1;
        float t = 0;
        float h = 0;

    public:
        EnvironmentSensor() {}
        EnvironmentSensor(int p, int type)
        {
            port = p;
        }
        bool enabled()
        {
            return port > -1;
        }
        int getType()
        {
            return type;
        }
        bool read()
        {
            if (type == SENSOR_TYPE_BME280)
            {
                return readBME280(port, &t, &h);
            }
            else if (type == SENSOR_TYPE_DHT22)
            {
                return readDHT22(port, &t, &h);
            }
            return false;
        }
        bool temperature()
        {
            return t;
        }
        bool humidity()
        {
            return h;
        }
    };

    class BME280 : public EnvironmentSensor
    {

    public:
        BME280() {}
        BME280(int p) : EnvironmentSensor(p, 280)
        {
        }
    };

    class DHT22 : public EnvironmentSensor
    {
    public:
        DHT22() {}
        DHT22(int p) : EnvironmentSensor(p, 22)
        {
        }
    };

    class EnvironmentSensors
    {
    public:
        EnvironmentSensor list[12];
        EnvironmentSensor getDHT22(int port)
        {
            return list[port];
        };
        EnvironmentSensor getBME280(int port)
        {
            return list[port + 6];
        };
    };

    EnvironmentSensors scan();
}

#endif