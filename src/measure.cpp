#include "measure.h"

namespace Measure
{

    bool scanBME280(int port)
    {
        Adafruit_BME280 bme;

        int bus = I2C_BUSES[port];

        Utils::TCA9548A(bus, false);
        bool statusBme = bme.begin(0x76);

        if (statusBme)
        {
            Serial.printf("BME280 at port %d, multiplexer bus %d\n", port, bus);

            float t = bme.readTemperature();
            float p = bme.readPressure() / 100.0F;
            float h = bme.readHumidity();

            Serial.printf("t: %.2f C, h: %.2f%%, p: %.0f hPa\n", t, h, p);
            return true;
        }

        return false;
    }

    bool scanDHT22(int port)
    {
        DHTStable DHT;

        int pin = SENSOR_PINS[port];

        int status = DHT.read22(pin);

        if (status == DHTLIB_OK)
        {
            Serial.printf("DHT22 at port %d, GPIO %d\n", port, pin);

            float h = DHT.getHumidity();
            float t = DHT.getTemperature();

            Serial.printf("t: %.2f C, h: %.2f%%\n", t, h);
            return true;
        }

        return false;
    }

    Sensors scan()
    {
        Sensors sensors;

        pinMode(SENSORS_ENABLE_PIN, OUTPUT);
        digitalWrite(SENSORS_ENABLE_PIN, HIGH);

        sleep(3);

        for (int i = 0; i < 6; i++)
        {
            if (scanDHT22(i))
            {
                sensors.list[i] = Sensor(i, SENSOR_TYPE_DHT22);
            }
            if (scanBME280(i))
            {
                sensors.list[i + 6] = Sensor(i, SENSOR_TYPE_BME280);
            }
        }

        return sensors;
    }

}