#include "control.h"

namespace Control
{
    void analogPinHigh(int pin)
    {
        digitalWrite(pin, HIGH);
    }
    void analogPinLow(int pin)
    {
        digitalWrite(pin, LOW);
    }
}