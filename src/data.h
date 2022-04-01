#ifndef TERRARIUM_DATA
#define TERRARIUM_DATA

#include "config.h"

// todo: add different type of events
class Schedule
{
public:
    int sinceHour;
    int sinceMinute;
    int untilHour;
    int untilMinute;
    float temperature;
    bool isSet = false;
};

class DataClimateZone
{
public:
    std::string name;
    float temperature;
    float humidity;
    bool heatingPhase;
    bool heaterStatus;
    bool isSet = false;
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