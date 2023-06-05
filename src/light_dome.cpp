#include <Arduino.h>
#include "control.h"
#include "data_structures.h"
#include "i2c.h"

static const char *TAG = "light_dome";

Control::Controller controller;

void setupTask(void *parameter)
{
  Serial.printf("Max alloc heap: %d\n", ESP.getMaxAllocHeap());
  Serial.printf("Max alloc psram: %d\n", ESP.getMaxAllocPsram());

  controller.beginLightDome();

  controller.lightDomeStartupAnimation();


  // startup animation

  Serial.println("Initial reset performed");

  for (;;)
  {
    I2C::Message msg = I2C::read();

    if (msg.bytes == I2C::Message::jsonSize())
    {
      controller.setColorAndBrightness(0, msg.illumination.color, msg.illumination.brightness);
      controller.setColorAndBrightness(1, msg.illumination.color, msg.illumination.brightness);
      controller.setColorAndBrightness(2, msg.illumination.color, msg.illumination.brightness);
    }

    vTaskDelay(1 * 1000 / portTICK_PERIOD_MS);
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Light Dome is starting");

  // Wire.begin();

  ESP_ERROR_CHECK(I2C::i2c_slave_init());
  ESP_LOGI(TAG, "I2C Slave initialized successfully");

  xTaskCreatePinnedToCore(
      setupTask,
      "setupTask",
      1024 * 34,
      NULL,
      100,
      NULL,
      0);
}

void loop()
{
  delay(100000);
  // Serial.println("Debug");
}