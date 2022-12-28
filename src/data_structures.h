#ifndef TERRARIUM_DATA_STRUCTURES
#define TERRARIUM_DATA_STRUCTURES

#include <math.h>
#include <string>
#include "ArduinoJson.h"
#include "constants.h"
#include "utils_pure.h"

class Time
{
public:
    int hours = 0;
    int minutes = 0;
    int seconds = 0; // not fully implemented
    Time() {}
    Time(int h, int m)
    {
        hours = h;
        minutes = m;
    }
    static std::string hourMinuteToString(int hour, int minute)
    {
        static char buffer[10];
        sprintf(buffer, "%02d:%02d", hour, minute);
        // Serial.println(buffer);
        return std::string(buffer);
    }
    std::string toString()
    {
        return hourMinuteToString(hours, minutes);
    }
    int toMin()
    {
        return hours * 60 + minutes;
    }
    int toSec()
    {
        return toMin() * 60 + seconds;
    }
    // returns (update logic to the best practices)
    // 1 - when this > that
    // 0 - when this = that
    // -1 - when this < that
    int compare(Time to)
    {
        if (toMin() > to.toMin())
        {
            return 1;
        }
        else if (toMin() < to.toMin())
        {
            return -1;
        }
        return 0;
    }

    bool inRange(Time from, Time to)
    {
        if (from.compare(to) == 0)
        {
            // this means that this is all day event, any time will be
            return true;
        }
        // overnight transition
        if (from.compare(to) > 0)
        {
            return compare(from) >= 0 || compare(to) <= 0;
        }
        else
        {
            return compare(from) >= 0 && compare(to) <= 0;
        }
    }

    // diff in minutes
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

        // todo add check for corrupted strings
        // todo add check for h:m vs h:m:s
        // str.length();

        char h[] = {str.c_str()[0], str.c_str()[1], 0};
        char m[] = {str.c_str()[3], str.c_str()[4], 0};

        object.hours = atoi(h);
        object.minutes = atoi(m);

        // Serial.printf("time - %d:%d", object.hours, object.minutes);

        return object;
    }
};

struct Color
{

public:
    int red = 0;
    int green = 0;
    int blue = 0;
    int kelvin = 0;
    Color() {}
    Color(int r, int g, int b)
    {
        red = r;
        green = g;
        blue = b;
    }
    Color(int r, int g, int b, int k)
    {
        red = r;
        green = g;
        blue = b;
        kelvin = k;
    }
    Color(int k)
    {
        kelvin = UtilsPure::roundKelvins(k);

        // kelvin to rgb (approx)
        // TODO - refactor this
        for (int i = 0; i < kelvinValuesCount; i++)
        {
            if (k > kelvinValues[i])
            {
                continue;
            }
            // calculate what is closer
            red = kelvinToRBG[i][0];
            green = kelvinToRBG[i][1];
            blue = kelvinToRBG[i][2];
            break;
        }
    }
    static int jsonSize()
    {
        return 64; // to be defined
    }
    DynamicJsonDocument toJSON()
    {
        DynamicJsonDocument doc(jsonSize());
        doc["r"] = red;
        doc["g"] = green;
        doc["b"] = blue;
        doc["k"] = kelvin;
        return doc;
    }

    static Color fromJSON(std::string json)
    {
        DynamicJsonDocument doc(jsonSize());
        deserializeJson(doc, json);

        return Color::fromJSONObj(doc);
    }

    static Color fromJSONObj(DynamicJsonDocument doc)
    {
        Color color;
        color.red = doc["r"];
        color.green = doc["g"];
        color.blue = doc["b"];
        color.kelvin = doc["k"];
        return color;
    }
};

class TemperatureMeasurments
{
protected:
    int c = 0;
    float measurments[10];

public:
    void add(float t)
    {
        measurments[c] = t;
        c++;
    }
    float calcError()
    {
        float min = measurments[0], max = measurments[0];
        for (int i = 0; i < c; i++)
        {
            if (measurments[i] >= max)
            {
                max = measurments[i];
            }
            if (measurments[i] <= min)
            {
                min = measurments[i];
            }
        }
        return abs(max - min);
    }
    int count()
    {
        return c;
    }
    float calcAverage()
    {
        if (count() == 0)
        {
            return 0;
        }
        float sum = 0;
        for (int i = 0; i < c; i++)
        {
            sum += measurments[i];
        }
        return sum / c;
    }
};

#endif