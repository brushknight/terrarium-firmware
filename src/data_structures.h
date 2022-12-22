#ifndef TERRARIUM_DATA_STRUCTURES
#define TERRARIUM_DATA_STRUCTURES

#include <math.h>

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