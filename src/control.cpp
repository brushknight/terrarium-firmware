#include "control.h"

namespace Control
{
    void analogPinHigh(int pin)
    {
        Serial.printf("GPIO %d HIGH\n", pin);
        digitalWrite(pin, HIGH);
    }
    void analogPinLow(int pin)
    {
        Serial.printf("GPIO %d LOW\n", pin);
        digitalWrite(pin, LOW);
    }
}