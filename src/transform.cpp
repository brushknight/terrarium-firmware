#include "transform.h"

namespace Transform
{
    // percent should be from 0.00 to 1.00
    int circadianKelvins(bool rising, float percent)
    {
        float max = kelvinNoon;
        float min = kelvinSunset;

        if (rising)
        {
            // linear chart formla
            return UtilsPure::roundKelvins(min + int((max - min) * percent));
        }

        // linear chart formla
        return UtilsPure::roundKelvins(max - int((max - min) * percent));
    }
}
