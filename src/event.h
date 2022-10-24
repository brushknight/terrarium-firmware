#ifndef TERRARIUM_EVENT
#define TERRARIUM_EVENT

#include "Arduino.h"
#include "utils.h"
#include "control.h"

namespace Event
{

    class Event
    {
    public:
        bool set = false;
        std::string since;
        std::string until;
        int durationSec;
        Event() {}
        Event(std::string s, std::string u, int dSec)
        {
            since = s;
            until = u;
            durationSec = dSec;
            set = true;
        }
        bool isActive(std::string now)
        {
            return set && Utils::checkScheduleTimeWindow(now, since, until);
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
        TemperatureEvent() {} // just for empty array of events
        TemperatureEvent(std::string s, std::string u, int dSec, float t) : Event(s, u, dSec)
        {
            temperature = t;
        }
        static int jsonSize()
        {
            return 80;
        }

        DynamicJsonDocument toJSON()
        {
            DynamicJsonDocument doc(jsonSize());
            doc["since"] = since;
            doc["until"] = until;
            doc["durationSec"] = durationSec;
            doc["temperature"] = temperature;
            return doc;
        }

        static TemperatureEvent fromJSON(std::string json)
        {
            TemperatureEvent temperatureEvent;

            DynamicJsonDocument doc(jsonSize());
            deserializeJson(doc, json);

            temperatureEvent.since = doc["since"].as<std::string>();
            temperatureEvent.until = doc["siuntilce"].as<std::string>();
            temperatureEvent.durationSec = doc["durationSec"];
            temperatureEvent.temperature = doc["temperature"];

            return temperatureEvent;
        }
    };

    class WateringEvent : public Event
    {
    public:
        WateringEvent(std::string s, std::string u, int dSec, float t) : Event(s, u, dSec) {}
    };

}

#endif