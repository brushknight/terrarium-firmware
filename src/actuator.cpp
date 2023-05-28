#include "actuator.h"

namespace Actuator
{
    // refactor this to accept 0-255 
    void analogPinHigh(int pin)
    {
        ESP_LOGD(TAG, "GPIO %d to HIGH", pin);
        digitalWrite(pin, 255);
        // analogWrite(pin, 255);
    }
    void analogPinLow(int pin)
    {
        ESP_LOGD(TAG, "GPIO %d to LOW", pin);
        // analogWrite(pin, 0);
        digitalWrite(pin, 0);
    }
}