#include <Arduino.h>
#include <analogWrite.h>

static const char *TAG = "board_test";

void setup()
{
    pinMode(4, OUTPUT);
    pinMode(25, OUTPUT);

    analogWriteFrequency(4, 21000);
    analogWriteFrequency(25, 21000);
}

void loop()
{

    for (int i = 0; i < 51; i++)
    {
        analogWrite(4, i * 5, 255);
        analogWrite(25, i * 5, 255);
        delay(100);
        ESP_LOGD(TAG, "analog write to 4, and 25 %d", i*5);

    }
}