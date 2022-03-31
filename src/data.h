#ifndef TERRARIUM_DATA
#define TERRARIUM_DATA

const int MAX_CLIMATE_ZONES = 3;

class DataClimateZone
{
public:
    std::string name;
    float temperature;
    float humidity;
    bool heatingPhase;
    bool heaterStatus;
};

class Metadata
{
public:
    int id;
};

class Data
{
public:
    // [] climate zone - temp, humid, heating phase, heater status
    DataClimateZone climateZones[MAX_CLIMATE_ZONES];
    // change to granular statuses
    // connection status wifi/bt
    bool WiFiStatus;
    bool BluetoothStatus;
    Metadata metadata;
    // terrarium metadata
};

#endif