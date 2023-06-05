#include "measure.h"

namespace Measure
{

    bool readDHT22(int port, float *t, float *h)
    {
        DHTStable DHT;

        int pin = SENSOR_PINS[port];

        ESP_LOGD(TAG, "[..] Reading DHT22 at port %d, GPIO %d", port, pin);

        int status = DHT.read22(pin);

        if (status == DHTLIB_OK)
        {
            ESP_LOGD(TAG, "Found DHT22 at port %d, GPIO %d", port, pin);

            *h = DHT.getHumidity();
            *t = DHT.getTemperature();

            ESP_LOGD(TAG, "[OK] Read DHT22 at port %d, GPIO %d | t:%.2f, h:%.2f", port, pin, *t, *h);

            return true;
        }

        return false;
    }

    bool readBME280(int port, float *t, float *h)
    {
        Adafruit_BME280 bme;

        int bus = I2C_BUSES[port];

        ESP_LOGD(TAG, "[..] Reading BME280 at port %d, multiplexer bus %d", port, bus);

        Utils::TCA9548A(bus, false);
        bool statusBme = bme.begin(0x76);

        if (statusBme)
        {

            ESP_LOGD(TAG, "Found BME280 at port %d, multiplexer bus %d", port, bus);

            *t = bme.readTemperature();
            // float p = bme.readPressure() / 100.0F;
            *h = bme.readHumidity();

            ESP_LOGD(TAG, "[OK] Read BME280 at port %d, bus %d | t:%.2f, h:%.2f", port, bus, *t, *h);

            return true;
        }

        return false;
    }

    bool readSHT31(int port, float *t, float *h)
    {
        Adafruit_SHT31 sht31;

        int bus = I2C_BUSES[port];

        ESP_LOGD(TAG, "[..] Reading SHT31 at port %d, multiplexer bus %d", port, bus);

        Utils::TCA9548A(bus, false);
        bool sensorStatus = sht31.begin(0x44);

        if (sensorStatus)
        {
            ESP_LOGD(TAG, "Found SHT31 at port %d, multiplexer bus %d", port, bus);

            *t = sht31.readTemperature();
            *h = sht31.readHumidity();

            ESP_LOGD(TAG, "[OK] Read SHT31 at port %d, bus %d | t:%.2f, h:%.2f", port, bus, *t, *h);

            return true;
        }

        return false;
    }

    bool readDS18B20(int port, float *t)
    {

        int pin = SENSOR_PINS[port];

        ESP_LOGD(TAG, "[..] Reading DS18B20 at port %d, GPIO %d", port, pin);

        OneWire oneWire(pin);
        DallasTemperature ds18b20(&oneWire);

        ds18b20.begin();
        ds18b20.requestTemperatures();
        // Serial.printf("reading DS18B20 at port %d, GPIO %d\n", port, pin);

        // DeviceAddress addr;
        // sensors.getAddress(addr, 0);
        // Serial.printf("DS18B20 DEBUG: address t:%d\n", addr);
        // float temperature = sensors.getTempC(addr);

        float temperature = ds18b20.getTempCByIndex(0);

        if (temperature == 85.0 || temperature == -127.0)
        {
            ESP_LOGE(TAG, "[FAIL] Reading DS18B20 at port %d, GPIO %d, error code: %d", port, pin, temperature);
            return false;
        }

        *t = temperature;

        ESP_LOGD(TAG, "[OK] Read DS18B20 at port %d, GPIO %d | t:%.2f", port, pin, *t);

        return true;
    }

    bool scanBME280(int port)
    {
        float t;
        float h;

        bool success = readBME280(port, &t, &h);

        if (success)
        {
            return true;
        }

        return false;
    }

    bool scanSHT31(int port)
    {
        float t;
        float h;

        bool success = readSHT31(port, &t, &h);

        if (success)
        {
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
            return true;
        }

        return false;
    }

    bool scanDS18B20(int port)
    {
        float t;

        bool success = readDS18B20(port, &t);

        if (success)
        {
            return true;
        }

        return false;
    }

    void enable(Actuator::HardwareLayer *hl)
    {
        ESP_LOGI(TAG, "[..] Enabling sensors");
        hl->setExpanderPin(5, HIGH);
        ESP_LOGI(TAG, "[OK] Enabling sensors");
    }


}