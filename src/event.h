#ifndef TERRARIUM_EVENT
#define TERRARIUM_EVENT

#include "Arduino.h"
#include "utils.h"
#include "control.h"
#include "real_time.h"

namespace Event
{

    class Event
    {
    public:
        std::string since;
        std::string until;
        int durationSec;
        Event(std::string s, std::string u, int dSec)
        {
            since = s;
            until = u;
            durationSec = dSec;
        }
        bool isActive(std::string now)
        {
            return RealTime::checkScheduleTimeWindow(now, since, until);
        }
    };

    class LightEvent : public Event
    {
    public:
        int brightness;
        Control::Color color;
        LightEvent(std::string s, std::string u, int dSec, int b, Control::Color c) : Event(s, u, dSec)
        {
            brightness = b;
            color = c;
        }
    };

    class TemperatureEvent : public Event
    {
    public:
        float temperature;
        TemperatureEvent(std::string s, std::string u, int dSec, float t) : Event(s, u, dSec)
        {
            temperature = t;
        }
    };

    class WateringEvent : public Event
    {
    public:
        WateringEvent(std::string s, std::string u, int dSec, float t) : Event(s, u, dSec) {}
    };

}

#endif