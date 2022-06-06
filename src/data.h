#ifndef TERRARIUM_DATA
#define TERRARIUM_DATA

#include "config.h"

// todo: add different type of events

class InitialSetup
{
public:
    bool isInSetupMode = false;
    std::string apName;
    std::string ipAddr;
};

class DataClimateZone
{
public:
    std::string name;
    std::string slug;
    float temperature;
    float targetTemperature;
    float humidity;
    bool heatingPhase;
    bool heaterStatus;
    bool isSet = false;
};

class Metadata
{
public:
    std::string id;
    std::string wifiName;
};

class Data
{
public:
    // [] climate zone - temp, humid, heating phase, heater status
    DataClimateZone climateZones[MAX_CLIMATE_ZONES];
    LightEvent lightEvents[MAX_LIGHT_EVENTS];
    // change to granular statuses
    // connection status wifi/bt
    bool WiFiStatus;
    bool BluetoothStatus;
    Metadata metadata;
    InitialSetup initialSetup;
    // terrarium metadata
};

#endif