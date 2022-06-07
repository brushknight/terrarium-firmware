#ifndef TERRARIUM_LIGHT
#define TERRARIUM_LIGHT

#include "Arduino.h"
#include "config.h"
#include "real_time.h"

namespace Light
{

    class EventData
    {
    public:
        LightEvent active[MAX_LIGHT_EVENTS];
    };

    void setup(ClimateConfig config);
    EventData control(int hour, int minute);

}

#endif