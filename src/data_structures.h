#ifndef TERRARIUM_DATA_STRUCTURES
#define TERRARIUM_DATA_STRUCTURES

#include <math.h>
#include <string>
#include "ArduinoJson.h"

const int kelvinValuesCount = 111;

const int kelvinValues[kelvinValuesCount] = {
    1000, 1100, 1200, 1300, 1400,
    1500, 1600, 1700, 1800, 1900,
    2000, 2100, 2200, 2300, 2400,
    2500, 2600, 2700, 2800, 2900,
    3000, 3100, 3200, 3300, 3400,
    3500, 3600, 3700, 3800, 3900,
    4000, 4100, 4200, 4300, 4400,
    4500, 4600, 4700, 4800, 4900,
    5000, 5100, 5200, 5300, 5400,
    5500, 5600, 5700, 5800, 5900,
    6000, 6100, 6200, 6300, 6400,
    6500, 6600, 6700, 6800, 6900,
    7000, 7100, 7200, 7300, 7400,
    7500, 7600, 7700, 7800, 7900,
    8000, 8100, 8200, 8300, 8400,
    8500, 8600, 8700, 8800, 8900,
    9000, 9100, 9200, 9300, 9400,
    9500, 9600, 9700, 9800, 9900,
    10000, 10100, 10200, 10300, 10400,
    10500, 10600, 10700, 10800, 10900,
    11000, 11100, 11200, 11300, 11400,
    11500, 11600, 11700, 11800, 11900,
    12000};

const int kelvinToRBG[kelvinValuesCount][3] = {
    {255, 56, 0},
    {255, 71, 0},
    {255, 83, 0},
    {255, 93, 0},
    {255, 101, 0},
    {255, 109, 0},
    {255, 115, 0},
    {255, 121, 0},
    {255, 126, 0},
    {255, 131, 0},
    {255, 138, 18},
    {255, 142, 33},
    {255, 147, 44},
    {255, 152, 54},
    {255, 157, 63},
    {255, 161, 72},
    {255, 165, 79},
    {255, 169, 87},
    {255, 173, 94},
    {255, 177, 101},
    {255, 180, 107},
    {255, 184, 114},
    {255, 187, 120},
    {255, 190, 126},
    {255, 193, 132},
    {255, 196, 137},
    {255, 199, 143},
    {255, 201, 148},
    {255, 204, 153},
    {255, 206, 159},
    {255, 209, 163},
    {255, 211, 168},
    {255, 213, 173},
    {255, 215, 177},
    {255, 217, 182},
    {255, 219, 186},
    {255, 221, 190},
    {255, 223, 194},
    {255, 225, 198},
    {255, 227, 202},
    {255, 228, 206},
    {255, 230, 210},
    {255, 232, 213},
    {255, 233, 217},
    {255, 235, 220},
    {255, 236, 224},
    {255, 238, 227},
    {255, 239, 230},
    {255, 240, 233},
    {255, 242, 236},
    {255, 243, 239},
    {255, 244, 242},
    {255, 245, 245},
    {255, 246, 247},
    {255, 248, 251},
    {255, 249, 253},
    {254, 249, 255},
    {252, 247, 255},
    {249, 246, 255},
    {247, 245, 255},
    {245, 243, 255},
    {243, 242, 255},
    {240, 241, 255},
    {239, 240, 255},
    {237, 239, 255},
    {235, 238, 255},
    {233, 237, 255},
    {231, 236, 255},
    {230, 235, 255},
    {228, 234, 255},
    {227, 233, 255},
    {225, 232, 255},
    {224, 231, 255},
    {222, 230, 255},
    {221, 230, 255},
    {220, 229, 255},
    {218, 229, 255},
    {217, 227, 255},
    {216, 227, 255},
    {215, 226, 255},
    {214, 225, 255},
    {212, 225, 255},
    {211, 224, 255},
    {210, 223, 255},
    {209, 223, 255},
    {208, 222, 255},
    {207, 221, 255},
    {207, 221, 255},
    {206, 220, 255},
    {205, 220, 255},
    {207, 218, 255},
    {207, 218, 255},
    {206, 217, 255},
    {205, 217, 255},
    {204, 216, 255},
    {204, 216, 255},
    {203, 215, 255},
    {202, 215, 255},
    {202, 214, 255},
    {201, 214, 255},
    {200, 213, 255},
    {200, 213, 255},
    {199, 212, 255},
    {198, 212, 255},
    {198, 212, 255},
    {197, 211, 255},
    {197, 211, 255},
    {197, 210, 255},
    {196, 210, 255},
    {195, 210, 255},
    {195, 209, 255}};

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
        // kelvin to rgb (approx)
        for (int i = 0; i < kelvinValuesCount; i++)
        {
            if (k > kelvinValues[i])
            {
                continue;
            }
            // calculate what is closer
            kelvin = kelvinValues[i];
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