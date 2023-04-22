#include "control.h"

namespace Control
{
    // refactor this to accept 0-255 
    void analogPinHigh(int pin)
    {
        ESP_LOGD(TAG, "GPIO %d to HIGH", pin);
        analogWrite(pin, 255);
    }
    void analogPinLow(int pin)
    {
        ESP_LOGD(TAG, "GPIO %d to LOW", pin);
        analogWrite(pin, 0);
    }
}