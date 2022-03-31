#include "climate.h"

namespace Climate
{

#define DHTTYPE DHT22
#define SENSORS_ENABLE_PIN 32

    void setup()
    {
        pinMode(SENSORS_ENABLE_PIN, OUTPUT);
    }

    void enableSensors()
    {
        digitalWrite(SENSORS_ENABLE_PIN, HIGH);
    }

    void disableSensors()
    {
        digitalWrite(SENSORS_ENABLE_PIN, LOW);
    }

}
