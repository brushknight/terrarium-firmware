#include "utils_pure.h"

namespace UtilsPure
{
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