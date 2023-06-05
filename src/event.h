#ifndef TERRARIUM_EVENT
#define TERRARIUM_EVENT

#include "Arduino.h"
#include "actuator.h"
#include "utils.h"
#include "transform.h"
#include "data_structures.h"

namespace Event
{

    class Event
    {
    public:
        bool set = false;
        Time since;
        Time until;
        int durationSec = -1;
        int durationMin = -1;
        Transform::Transform transform;
        Event() {}
        Event(std::string s, std::string u, int dSec)
        {
            since = Time::fromString(s);
            until = Time::fromString(u);
            durationMin = since.diff(until);
            durationSec = dSec;
            set = true;
        }
        // returns percent in float format
        float transformPercent(Time now)
        {
            int passedMinutes = since.diff(now);
            return ((float)passedMinutes) / ((float)durationMin);
        }
        // return 1 if rising, 0 if noon, -1 if faiding
        int isRising(Time now)
        {
            int durationMinHalf = durationMin / 2;
            int passedMinutes = since.diff(now);
            if (passedMinutes < durationMinHalf)
            {
                return 1;
            }
            else if (passedMinutes == durationMinHalf)
            {
                return 0;
            }
            else
            {
                return -1;
            }
        }
        // split time windows in half and find how much time passed in it
        // returns percent as float 0.00 -> 1.00
        float risingPercent(Time now)
        {
            int durationMinHalf = durationMin / 2;
            int passedMinutes = since.diff(now);
            return ((float)passedMinutes) / ((float)durationMinHalf);
        }

        
        float fadingPercent(Time now)
        {
            int durationMinHalf = durationMin / 2;
            int passedMinutes = since.diff(now);
            return ((float)passedMinutes - (float)durationMinHalf) / ((float)durationMinHalf);
        }
        float transformedValue(Time now)
        {

            if (transform.direction() == 0)
            {
                return transform.from;
            }
            float tPercent = transformPercent(now);
            if (transform.direction() == -1)
            {
                return transform.from - tPercent * (transform.from - transform.to);
            }
            if (transform.direction() == 1)
            {
                return transform.from + tPercent * (transform.to - transform.from);
            }
            return 0;
        }
        bool isSet()
        {
            return set;
        }
        bool isActive(Time now)
        {
            if (!set)
            {
                return false;
            }

            return now.inRange(since, until);
        }
    };

    class LightEvent : public Event
    {
    public:
        int brightness = 0;
        std::string type = "simple";
        Color color;
        LightEvent() {} // just for empty array of events
        LightEvent(std::string s, std::string u, int dSec, int b, Color c) : Event(s, u, dSec)
        {
            brightness = b;
            color = c;
        }
        LightEvent(std::string s, std::string u, int dSec, Transform::Transform t) : Event(s, u, dSec)
        {
            transform = t;
        }
        bool isCircadian()
        {
            return type == "circadian";
        }
        static int jsonSize()
        {
            return 156 + Color::jsonSize() + Transform::Transform::jsonSize(); // to be defined
        }
        DynamicJsonDocument toJSON()
        {
            DynamicJsonDocument doc(jsonSize());
            doc["set"] = set;
            doc["type"] = type;
            doc["since"] = since.toString();
            doc["until"] = until.toString();
            doc["duration_sec"] = durationSec;
            doc["brightness"] = brightness;
            doc["color"] = color.toJSON();
            doc["transform"] = transform.toJSON();
            return doc;
        }

        static LightEvent fromJSON(std::string json)
        {
            DynamicJsonDocument doc(jsonSize());
            deserializeJson(doc, json);

            return LightEvent::fromJSONObj(doc);
        }

        static LightEvent fromJSONObj(DynamicJsonDocument doc)
        {
            LightEvent event;
            event.set = doc["set"];
            if (doc.containsKey("type"))
            {
                event.type = doc["type"].as<std::string>();
            }
            event.since = Time::fromString(doc["since"].as<std::string>());
            event.until = Time::fromString(doc["until"].as<std::string>());
            event.durationSec = doc["duration_sec"];
            event.brightness = doc["brightness"];
            event.color = Color::fromJSONObj(doc["color"]);
            event.transform = Transform::Transform::fromJSONObj(doc["transform"]);
            event.durationMin = event.since.diff(event.until);
            return event;
        }
    };

    class TemperatureEvent : public Event
    {
    public:
        float temperature = -1;
        int mode; // 0 - PD, 1 - PID
        TemperatureEvent() {} // just for empty array of events
        TemperatureEvent(std::string s, std::string u, int dSec, float t) : Event(s, u, dSec)
        {
            temperature = t;
        }
        TemperatureEvent(std::string s, std::string u, int dSec, Transform::Transform t) : Event(s, u, dSec)
        {
            transform = t;
        }
        static int jsonSize()
        {
            return 128 + 16 + Transform::Transform::jsonSize(); // re-calculate
        }
        bool isPID(){
            return mode == 1;
        }
        DynamicJsonDocument toJSON()
        {
            DynamicJsonDocument doc(jsonSize());
            doc["set"] = set;
            doc["since"] = since.toString();
            doc["until"] = until.toString();
            doc["duration_sec"] = durationSec;
            doc["temperature"] = temperature;
            doc["mode"] = mode;
            doc["transform"] = transform.toJSON();
            return doc;
        }

        static TemperatureEvent fromJSON(std::string json)
        {
            DynamicJsonDocument doc(jsonSize());
            deserializeJson(doc, json);

            return TemperatureEvent::fromJSONObj(doc);
        }

        static TemperatureEvent fromJSONObj(DynamicJsonDocument doc)
        {
            TemperatureEvent event;
            event.set = doc["set"];
            event.since = Time::fromString(doc["since"].as<std::string>());
            event.until = Time::fromString(doc["until"].as<std::string>());
            event.durationSec = doc["duration_sec"];
            event.temperature = doc["temperature"];
            event.mode = doc["mode"];
            event.transform = Transform::Transform::fromJSONObj(doc["transform"]);
            event.durationMin = event.since.diff(event.until);
            return event;
        }
    };

    class WateringEvent : public Event
    {
    public:
        WateringEvent(std::string s, std::string u, int dSec, float t) : Event(s, u, dSec) {}
    };

}

#endif