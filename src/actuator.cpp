#include "actuator.h"

namespace Actuator
{
    // refactor this to accept 0-255
    void analogPinHigh(int pin)
    {
        ESP_LOGD(TAG, "GPIO %d to HIGH", pin);
        // digitalWrite(pin, 255);
        analogWrite(pin, 255);
    }
    void analogPinLow(int pin)
    {
        ESP_LOGD(TAG, "GPIO %d to LOW", pin);
        analogWrite(pin, 0);
        // digitalWrite(pin, 0);
    }
    void analogPinSet(int pin, int value)
    {
        ESP_LOGD(TAG, "GPIO %d to %d", pin, value);
        // digitalWrite(pin, 255);
        analogWrite(pin, value);
    }
    void digitalPinSet(int pin, int value)
    {
        ESP_LOGD(TAG, "GPIO %d to %d", pin, value);
        // digitalWrite(pin, 255);
        digitalWrite(pin, value);
    }
}