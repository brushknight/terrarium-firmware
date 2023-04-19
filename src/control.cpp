#include "control.h"

namespace Control
{
    void analogPinHigh(int pin)
    {
        ESP_LOGD(TAG, "GPIO %d HIGH", pin);
        digitalWrite(pin, HIGH);
    }
    void analogPinLow(int pin)
    {
        ESP_LOGD(TAG, "GPIO %d LOW", pin);
        digitalWrite(pin, LOW);
    }
}