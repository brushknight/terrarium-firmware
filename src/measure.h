#ifndef TERRARIUM_MEASURE
#define TERRARIUM_MEASURE

#include "config.h"
#include "DHTStable.h"
#include "utils.h"
#include <Adafruit_BME280.h>
#include <Adafruit_SHT31.h>
#include <Adafruit_Sensor.h>
#include "DallasTemperature.h"

namespace Measure
{
    const int SENSOR_TYPE_DHT22 = 22;
    const int SENSOR_TYPE_BME280 = 280;
    const int SENSOR_TYPE_DS18B20 = 1820;
    const int SENSOR_TYPE_SHT31 = 31;

    const int SENSOR_OFFSET_DHT22 = 0;
    const int SENSOR_OFFSET_BME280 = 6;
    const int SENSOR_OFFSET_DS18B20 = 12;
    const int SENSOR_OFFSET_SHT31 = 18;

    bool readBME280(int port, float *t, float *h);
    bool readSHT31(int port, float *t, float *h);
    bool readDHT22(int port, float *t, float *h);
    bool readDS18B20(int port, float *t);

    const int sensorPortCount = 6;
    const int sensorTypesSupported = 4;

    class SensorID
    {
    public:
        int port = -1;
        int type = -1;
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
            DynamicJsonDocument doc(jsonSize());
            deserializeJson(doc, json);

            return SensorID::fromJSONObj(doc);
        }

        static SensorID fromJSONObj(DynamicJsonDocument doc)
        {
            SensorID sensorID;

            sensorID.port = doc["port"];
            sensorID.type = doc["type"];

            return sensorID;
        }
        static std::string typeToHuman(int type)
        {
            if (type == SENSOR_TYPE_BME280)
            {
                return "BME280";
            }
            if (type == SENSOR_TYPE_SHT31)
            {
                return "SHT31";
            }
            else if (type == SENSOR_TYPE_DHT22)
            {
                return "DHT22";
            }
            else if (type == SENSOR_TYPE_DS18B20)
            {
                return "DS18B20";
            }
            return "unknown";
        }
    };

    class EnvironmentSensor
    {
    private:
        SensorID id;
        float t = 0;
        float h = 0;

    public:
        EnvironmentSensor() {}
        EnvironmentSensor(SensorID givenID)
        {
            id = givenID;

        }
        bool enabled()
        {
            return id.isSet();
        }
        int getType()
        {
            return id.type;
        }
        bool read()
        {
            if (id.type == SENSOR_TYPE_BME280)
            {
                return readBME280(id.port, &t, &h);
            }
            else if (id.type == SENSOR_TYPE_DHT22)
            {
                return readDHT22(id.port, &t, &h);
            }
            else if (id.type == SENSOR_TYPE_DS18B20)
            {
                return readDS18B20(id.port, &t);
            }
            else if (type == SENSOR_TYPE_SHT31)
            {
                return readSHT31(port, &t, &h);
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
        static int jsonSize()
        {
            return 96;
        }
        DynamicJsonDocument toJSON()
        {
            DynamicJsonDocument doc(jsonSize());

            doc["port"] = id.port;
            doc["type"] = SensorID::typeToHuman(id.type);
            doc["humidity"] = h;
            doc["temperature"] = t;

            return doc;
        }
    };

    class BME280 : public EnvironmentSensor
    {

    public:
        BME280() {}
        BME280(int p) : EnvironmentSensor(SensorID(p, SENSOR_TYPE_BME280))
        {
        }
    };

    class SHT31 : public EnvironmentSensor
    {

    public:
        SHT31() {}
        SHT31(int p) : EnvironmentSensor(p, SENSOR_TYPE_SHT31)
        {
        }
    };

    class DHT22 : public EnvironmentSensor
    {
    public:
        DHT22() {}
        DHT22(int p) : EnvironmentSensor(SensorID(p, SENSOR_TYPE_DHT22))
        {
        }
    };

    class DS18B20 : public EnvironmentSensor
    {
    public:
        DS18B20() {}
        DS18B20(int p) : EnvironmentSensor(SensorID(p, SENSOR_TYPE_DS18B20))
        {
        }
    };

    class EnvironmentSensors
    {
    public:
        EnvironmentSensor list[sensorPortCount * sensorTypesSupported];
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
            else if (sID.type == SENSOR_TYPE_DS18B20)
            {
                return getDS18B20(sID.port);
            }
            else if (sID.type == SENSOR_TYPE_SHT31)
            {
                return getSHT31(sID.port);
            }
            Serial.println("ERROR: undefined sensor type");
            return EnvironmentSensor();
        };
        EnvironmentSensor getDHT22(int port)
        {
            return list[port + SENSOR_OFFSET_DHT22];
        };
        EnvironmentSensor getBME280(int port)
        {
            return list[port + SENSOR_OFFSET_BME280];
        };
        EnvironmentSensor getDS18B20(int port)
        {
            return list[port + SENSOR_OFFSET_DS18B20];
        };
        EnvironmentSensor getSHT31(int port)
        {
            return list[port + SENSOR_OFFSET_SHT31];
        };
        bool readDHT22(int port)
        {
            return list[port + SENSOR_OFFSET_DHT22].read();
        };
        bool readBME280(int port)
        {
            return list[port + SENSOR_OFFSET_BME280].read();
        };
        bool readDS18B20(int port)
        {
            return list[port + SENSOR_OFFSET_DS18B20].read();
        };
        bool readSHT31(int port)
        {
            return list[port + SENSOR_OFFSET_SHT31].read();
        };
        static int jsonSize()
        {
            return 64 + EnvironmentSensor::jsonSize() * sensorPortCount * sensorTypesSupported;
        }
        DynamicJsonDocument toJSON()
        {
            DynamicJsonDocument doc(jsonSize());
            int index = 0;
            for (int i = 0; i < sensorPortCount * sensorTypesSupported; i++)
            {
                if (list[i].enabled())
                {
                    doc[index] = list[i].toJSON();
                    index++;
                }
            }

            return doc;
        }
    };

    bool scan();
    void enable();
    bool readSensors();
    EnvironmentSensors *getSharedSensors();
}

#endif