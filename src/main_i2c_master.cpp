#include <Arduino.h>
#include "i2c.h"

static const char *TAG = "i2c-master";

void setup()
{
    I2C::Message msg;
    msg.type = 1;
    msg.illumination = I2C::IlluminationCommand(1, Color(255,0,0));

    I2C::Message msg2;
    msg2.type = 1;
    msg2.illumination = I2C::IlluminationCommand(1, Color(0,255,0));

    I2C::Message msg3;
    msg3.type = 1;
    msg3.illumination = I2C::IlluminationCommand(1, Color(0,0,255));


    ESP_ERROR_CHECK(I2C::i2c_master_init());
    ESP_LOGI(TAG, "I2C initialized successfully");

    while (1)
    {
        // ESP_LOGI(TAG, "JSON size %d", msg.jsonSize());
        // ESP_LOGI(TAG, "std length %d", messageBody.length());
        // ESP_LOGI(TAG, "std size %d", messageBody.size());
        // ESP_LOGI(TAG, "char size %d", sizeof(messageBody.c_str()));
        // ESP_LOGI(TAG, "uint8 size %d", sizeof((uint8_t*)messageBody.c_str()));

        I2C::write(msg);
        vTaskDelay(3000 / portTICK_PERIOD_MS);

        I2C::write(msg2);
        vTaskDelay(3000 / portTICK_PERIOD_MS);

        I2C::write(msg3);
        vTaskDelay(3000 / portTICK_PERIOD_MS);

    }
}

void loop()
{
    delay(100000);
    // Serial.println("Debug");
}