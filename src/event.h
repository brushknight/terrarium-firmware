#ifndef TERRARIUM_EVENT
#define TERRARIUM_EVENT

#include "Arduino.h"
#include "utils.h"
#include "control.h"

namespace Event
{

    class Transform
    {
    public:
        int from = 0;
        int to = 0;
        bool isSet()
        {
            return from > 0 || to > 0;
        }
        int direction()
        {
            if (from > to)
            {
                return -1;
            }
            else if (to > from)
            {
                return 1;
            }
            return 0;
        }
        static int jsonSize()
        {
            return 32; // to be adjusted
        }
        DynamicJsonDocument toJSON()
        {
            DynamicJsonDocument doc(jsonSize());
            doc["from"] = from;
            doc["to"] = to;
            return doc;
        }

        static Transform fromJSON(std::string json)
        {
            DynamicJsonDocument doc(jsonSize());
            deserializeJson(doc, json);

            return Transform::fromJSONObj(doc);
        }

        static Transform fromJSONObj(DynamicJsonDocument doc)
        {
            Transform transform;
            transform.from = doc["from"];
            transform.to = doc["to"];
            return transform;
        }
    };

    class Time
    {
    public:
        int hours = 0;
        int minutes = 0;
        std::string toString()
        {
            return Utils::hourMinuteToString(hours, minutes);
        }
        // returns diff in minutes
        int diff(Time target)
        {
            // todo: add test suites
            int targetMinutes = target.minutes;
            int targetHours = target.hours;

            int diffMinutes = target.minutes - minutes;

            if (minutes > targetMinutes)
            {
                diffMinutes = 60 - minutes + targetMinutes;
                targetHours--;
            }

            int diffHours = target.hours - hours;

            if (hours > targetHours)
            {
                diffHours = 24 - hours + targetHours;
            }

            return diffMinutes + diffHours * 60;
        }
        static Time fromString(std::string str)
        {
            Time object;

            char h[] = {str.c_str()[0], str.c_str()[1], 0};
            char m[] = {str.c_str()[3], str.c_str()[4], 0};

            object.hours = atoi(h);
            object.minutes = atoi(m);

            // Serial.printf("time - %d:%d", object.hours, object.minutes);

            return object;
        }
    };

    class Event
    {
    public:
        bool set = false;
        Time since;
        Time until;
        int durationSec = -1;
        int durationMin = -1;
        Transform transform;
        Event() {}
        Event(std::string s, std::string u, int dSec)
        {
            since = Time::fromString(s);
            until = Time::fromString(u);
            durationMin = since.diff(until);
            set = true;
        }
        // returns percent in float format
        float transformPercent(std::string now)
        {
            Time nowTime = Time::fromString(now);
            int passedMinutes = since.diff(nowTime);
            return ((float)passedMinutes) / ((float)durationMin);
        }
        float transformedValue(std::string now)
        {

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
        bool isActive(std::string now)
        {
            return set && Utils::checkScheduleTimeWindow(now, since.toString(), until.toString());
        }
    };

    class LightEvent : public Event
    {
    public:
        int brightness = 0;
        Control::Color color;
        LightEvent() {} // just for empty array of events
        LightEvent(std::string s, std::string u, int dSec, int b, Control::Color c) : Event(s, u, dSec)
        {
            brightness = b;
            color = c;
        }
        LightEvent(std::string s, std::string u, int dSec, Transform t) : Event(s, u, dSec)
        {
            transform = t;
        }
        static int jsonSize()
        {
            return 128 + Control::Color::jsonSize() + Transform::jsonSize(); // to be defined
        }
        DynamicJsonDocument toJSON()
        {
            DynamicJsonDocument doc(jsonSize());
            doc["set"] = set;
            doc["since"] = since.toString();
            doc["until"] = until.toString();
            doc["duration_sec"] = durationSec;
            doc["brightness"] = brightness;
            doc["color"] = color.toJSON();
            doc["transform"] = transform.toJSON();
            // Serial.printf("DEBUG - to JSON temp: %0.2f\n",doc["temperature"]);
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
            event.since = Time::fromString(doc["since"].as<std::string>());
            event.until = Time::fromString(doc["until"].as<std::string>());
            event.durationSec = doc["duration_sec"];
            // Serial.printf("DEBUG - from JSON temp: %0.2f\n",doc["temperature"]);
            event.brightness = doc["brightness"];
            event.color = Control::Color::fromJSONObj(doc["color"]);
            event.transform = Transform::fromJSONObj(doc["transform"]);
            event.durationMin = event.since.diff(event.until);
            return event;
        }
    };

    class TemperatureEvent : public Event
    {
    public:
        float temperature = -1;
        TemperatureEvent() {} // just for empty array of events
        TemperatureEvent(std::string s, std::string u, int dSec, float t) : Event(s, u, dSec)
        {
            temperature = t;
        }
        TemperatureEvent(std::string s, std::string u, int dSec, Transform t) : Event(s, u, dSec)
        {
            transform = t;
        }
        static int jsonSize()
        {
            return 128 + Transform::jsonSize();
        }

        DynamicJsonDocument toJSON()
        {
            DynamicJsonDocument doc(jsonSize());
            doc["set"] = set;
            doc["since"] = since.toString();
            doc["until"] = until.toString();
            doc["duration_sec"] = durationSec;
            doc["temperature"] = temperature;
            doc["transform"] = transform.toJSON();
            // Serial.printf("DEBUG - to JSON temp: %0.2f\n",doc["temperature"]);
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
            // Serial.printf("DEBUG - from JSON temp: %0.2f\n",doc["temperature"]);
            event.temperature = doc["temperature"];
            event.transform = Transform::fromJSONObj(doc["transform"]);
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