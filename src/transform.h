#ifndef TERRARIUM_TRANSFORM
#define TERRARIUM_TRANSFORM

#include <string>
#include "ArduinoJson.h"
#include "constants.h"
#include "utils_pure.h"

namespace Transform
{

    class Transform
    {
    public:
        int from = 0;
        int to = 0;
        Transform() {}
        Transform(int f, int t)
        {
            from = f;
            to = t;
        }
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

    // percent should be from 0.00 to 1.00
    int circadianKelvins(bool rising, float percent);
}

#endif