#ifndef TERRARIUM_DATA
#define TERRARIUM_DATA

#include "config.h"
#include "zone.h"
#include "measure.h"

// todo: add different type of events

class InitialSetup
{
public:
    bool isInSetupMode = false;
    std::string apName;
    std::string ipAddr;
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
    Zone::ZonesStatuses zones;
    Measure::EnvironmentSensors sharedSensors;
    // change to granular statuses
    // connection status wifi/bt
    bool WiFiStatus;
    bool BluetoothStatus;
    int RtcBatteryPercent;
    int RtcBatteryMilliVolt;
    std::string mac;
    Metadata metadata;
    InitialSetup initialSetup;
};

#endif