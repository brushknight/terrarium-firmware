#include "measure.h"

namespace Measure
{

    bool readDHT22(int port, float *t, float *h){
        DHTStable DHT;

        int pin = SENSOR_PINS[port];

        int status = DHT.read22(pin);

        if (status == DHTLIB_OK)
        {
            Serial.printf("DHT22 at port %d, GPIO %d\n", port, pin);

            *h = DHT.getHumidity();
            *t = DHT.getTemperature();

            return true;
        }

        return false;
    }

    bool readBME280(int port, float *t, float *h)
    {
        Adafruit_BME280 bme;

        int bus = I2C_BUSES[port];

        Utils::TCA9548A(bus, false);
        bool statusBme = bme.begin(0x76);

        if (statusBme)
        {
            Serial.printf("BME280 at port %d, multiplexer bus %d\n", port, bus);

            *t = bme.readTemperature();
            // float p = bme.readPressure() / 100.0F;
            *h = bme.readHumidity();

            return true;
        }

        return false;
    }

    bool scanBME280(int port)
    {
        float t;
        float h;

        bool success = readBME280(port, &t, &h);

        if (success)
        {
            Serial.printf("t: %.2f C, h: %.2f%%\n", t, h);
            return true;
        }

        return false;
    }

    bool scanDHT22(int port)
    {
        float t;
        float h;

        bool success = readDHT22(port, &t, &h);

        if (success)
        {
            Serial.printf("t: %.2f C, h: %.2f%%\n", t, h);
            return true;
        }

        return false;
    }

    EnvironmentSensors scan()
    {
        EnvironmentSensors sensors;

        pinMode(SENSORS_ENABLE_PIN, OUTPUT);
        digitalWrite(SENSORS_ENABLE_PIN, HIGH);

        sleep(3);

        for (int i = 0; i < 6; i++)
        {
            if (scanDHT22(i))
            {
                sensors.list[i] = DHT22(i);
            }
            if (scanBME280(i))
            {
                sensors.list[i + 6] = BME280(i);
            }
        }

        return sensors;
    }

}