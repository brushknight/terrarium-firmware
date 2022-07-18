#include "sensor.h"

namespace Sensor
{

    void scanBME280(int port)
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
        }
    }

    void scanDHT22(int port)
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
        }
    }

    void scan()
    {

        DHTStable DHT;

        pinMode(SENSORS_ENABLE_PIN, OUTPUT);
        digitalWrite(SENSORS_ENABLE_PIN, HIGH);

        sleep(3);

        scanDHT22(0);
        scanDHT22(1);
        scanDHT22(2);
        scanDHT22(3);
        scanDHT22(4);
        scanDHT22(5);

        // Enable i2c multiplexer and test each address as BME280
        scanBME280(0);
        scanBME280(1);
        scanBME280(2);
        scanBME280(3);
        scanBME280(4);
        scanBME280(5);
    }

}