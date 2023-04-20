#include "utils_pure.h"
#include <string>

namespace UtilsPure
{
    // rounds kelvins to known kelvin values from a known values
    int roundKelvins(int k)
    {
        for (int i = 0; i < kelvinValuesCount; i++)
        {
            if (k > kelvinValues[i])
            {
                continue;
            }
            // calculate what is closer
            return kelvinValues[i];
        }
        return 0;
    }
}