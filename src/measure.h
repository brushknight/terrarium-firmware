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

    class SensorID
    {
    public:
        int port;
        int type;
        SensorID() {} // for empty arrays
        SensorID(int p, int t)
        {
            type = t;
            port = p;
        }
        bool isSet()
        {
            return type > 0;
        }
        static int jsonSize()
        {
            return 32;
        }

        DynamicJsonDocument toJSON()
        {
            DynamicJsonDocument doc(jsonSize());
            doc["port"] = port;
            doc["type"] = type;
            return doc;
        }

        static SensorID fromJSON(std::string json)
        {
            SensorID sensorID;

            DynamicJsonDocument doc(jsonSize());
            deserializeJson(doc, json);

            sensorID.port = doc["port"];
            sensorID.type = doc["type"];

            return sensorID;
        }
    };

    class EnvironmentSensor
    {
    private:
        int port = -1;
        int type = -1;
        float t = 0;
        float h = 0;

    public:
        EnvironmentSensor() {}
        EnvironmentSensor(int p, int t)
        {
            port = p;
            type = t;
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
        float temperature()
        {
            return t;
        }
        float humidity()
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
        EnvironmentSensor get(SensorID sID)
        {
            if (sID.type == SENSOR_TYPE_DHT22)
            {
                return getDHT22(sID.port);
            }
            else if (sID.type == SENSOR_TYPE_BME280)
            {
                return getBME280(sID.port);
            }
            Serial.println("ERROR: undefined sensor type");
            return EnvironmentSensor();
        };
        EnvironmentSensor getDHT22(int port)
        {
            return list[port];
        };
        EnvironmentSensor getBME280(int port)
        {
            return list[port + 6];
        };
        bool readDHT22(int port)
        {
            return list[port].read();
        };
        bool readBME280(int port)
        {
            return list[port + 6].read();
        };
    };

    bool scan();
    bool readSensors();
    EnvironmentSensors *getSharedSensors();
}

#endif