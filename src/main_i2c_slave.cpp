#include <Arduino.h>
#include "i2c.h"

static const char *TAG = "i2c-slave";

void setup()
{

    // uint8_t received_data[I2C_SLAVE_RX_BUF_LEN] = {0};

    ESP_ERROR_CHECK(I2C::i2c_slave_init());
    ESP_LOGI(TAG, "I2C Slave initialized successfully");

    while (1)
    {

        I2C::Message msg = I2C::read();

        std::string requestBody;
        serializeJson(msg.toJSON(), requestBody);

        ESP_LOGI(TAG, "Message type %d", msg.type);
        ESP_LOGI(TAG, "Message brightness %d", msg.illumination.brightness);
        ESP_LOGI(TAG, "Message red %d", msg.illumination.color.red);
        ESP_LOGI(TAG, "Message green %d", msg.illumination.color.red);
        ESP_LOGI(TAG, "Message blue %d", msg.illumination.color.red);

        delay(1000 * 5);

        // int bytesRead = i2c_slave_read_buffer(I2C::i2c_slave_port, received_data, I2C_SLAVE_RX_BUF_LEN, 1000 / portTICK_PERIOD_MS);
        // // i2c_reset_rx_fifo(I2C::i2c_slave_port);

        // ESP_LOGI(TAG, "Raw data recived %d bytes, = %s", bytesRead, received_data);

        // if (strncmp((const char *)received_data, "LED_ON", 6) == 0)
        // {
        //     ESP_LOGI(TAG, "Data Recived = %s", received_data);
        //     // gpio_set_level(LED_PIN, 1);
        // }
        // else if (strncmp((const char *)received_data, "LED_OFF", 7) == 0)
        // {
        //     ESP_LOGI(TAG, "Data Recived = %s", received_data);
        //     // gpio_set_level(LED_PIN, 0);
        // }

        // for(int i = 0; i < I2C_SLAVE_RX_BUF_LEN; i++){
        //     received_data[i] = 0;
        // }

        // memset(received_data, 0, I2C_SLAVE_RX_BUF_LEN);
    }
}

void loop()
{
    delay(100000);
    // Serial.println("Debug");
}