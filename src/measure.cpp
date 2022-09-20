#include "measure.h"

namespace Measure
{

    EnvironmentSensors sharedSensors = EnvironmentSensors();

    EnvironmentSensors * getSharedSensors()
    {
        return &sharedSensors;
    }

    // run in the loop and check each 5s
    bool readSensors()
    {
        for (int i = 0; i < 6; i++)
        {
            // Serial.printf("DHT %d check\n", i);
            if (sharedSensors.getDHT22(i).enabled())
            {
                if (!sharedSensors.readDHT22(i))
                {
                    Serial.printf("DHT %d failed\n", i);
                }
            }

            // Serial.printf("BME280 %d check\n", i);
            if (sharedSensors.getBME280(i).enabled())
            {

                if (!sharedSensors.readBME280(i))
                {
                    Serial.printf("BME280 %d failed\n", i);
                }
            }
        }
        return true;
    }

    bool readDHT22(int port, float *t, float *h)
    {
        DHTStable DHT;

        int pin = SENSOR_PINS[port];

        Serial.printf("reading DHT22 at port %d, GPIO %d\n", port, pin);

        int status = DHT.read22(pin);

        if (status == DHTLIB_OK)
        {
            Serial.printf("DHT22 at port %d, GPIO %d\n", port, pin);

            *h = DHT.getHumidity();
            *t = DHT.getTemperature();

            Serial.printf("DEBUG: t:%.2f, h:%.2f\n", *t, *h);

            return true;
        }

        return false;
    }

    bool readBME280(int port, float *t, float *h)
    {
        Adafruit_BME280 bme;

        int bus = I2C_BUSES[port];

        Serial.printf("reading BME280 at port %d, multiplexer bus %d\n", port, bus);

        Utils::TCA9548A(bus, false);
        bool statusBme = bme.begin(0x76);

        if (statusBme)
        {
            Serial.printf("BME280 at port %d, multiplexer bus %d\n", port, bus);

            *t = bme.readTemperature();
            // float p = bme.readPressure() / 100.0F;
            *h = bme.readHumidity();

            Serial.printf("DEBUG: t:%.2f, h:%.2f\n", *t, *h);

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

    bool scan()
    {
        pinMode(SENSORS_ENABLE_PIN, OUTPUT);
        digitalWrite(SENSORS_ENABLE_PIN, HIGH);

        sleep(3);

        for (int i = 0; i < 6; i++)
        {
            if (scanDHT22(i))
            {
                sharedSensors.list[i] = DHT22(i);
            }
            if (scanBME280(i))
            {
                sharedSensors.list[i + 6] = BME280(i);
            }
        }

        return true;
    }

}