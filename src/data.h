#ifndef TERRARIUM_DATA
#define TERRARIUM_DATA

#include "config.h"
#include "zone.h"

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
    // change to granular statuses
    // connection status wifi/bt
    bool WiFiStatus;
    bool BluetoothStatus;
    int RtcBatteryPercent;
    Metadata metadata;
    InitialSetup initialSetup;
    // terrarium metadata
};

#endif