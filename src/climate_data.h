#ifndef TERRARIUM_CLIMATE_DATA
#define TERRARIUM_CLIMATE_DATA

namespace Climate
{

    class ClimateData
    {
    public:
        float t;
        float h;
        int status;
        ClimateData(){};
        ClimateData(float temp, float humid, int st)
        {
            t = temp;
            h = humid;
            status = st;
        };
    };

    enum HeaterPhase
    {
        heating,
        cooling
    };
}

#endif