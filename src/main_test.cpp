#include <Arduino.h>
#include <analogWrite.h>
#include <PCF8574.h>

static const char *TAG = "board_test";

PCF8574 pcf(0x20);

void setup()
{
    pinMode(4, OUTPUT);
    pinMode(25, OUTPUT);
    pinMode(13, OUTPUT);
    
    
    

    // analogWriteFrequency(4, 21000);
    // analogWriteFrequency(25, 21000);

}

void loop()
{

digitalWrite(13, 1);
delay(1000);
digitalWrite(13, 0);
delay(1000);

    // for (int i = 0; i < 51; i++)
    // {
    //     analogWrite(4, i * 5, 255);
    //     analogWrite(25, i * 5, 255);
    //     delay(5000);
    //     ESP_LOGD(TAG, "analog write to 4, and 25 %d", i*5);

    // }
}