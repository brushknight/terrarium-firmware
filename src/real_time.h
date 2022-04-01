#ifndef TERRARIUM_RTC
#define TERRARIUM_RTC

#include "time.h"
#include "Arduino.h"
#include <RTClib.h>
#include "display.h"
#include "net.h"
#include "utils.h"
// #include "status.h"

namespace RealTime
{
    void setup(bool rtcEnabled);
    void syncFromRTC();
    void syncFromNTP();
    bool saveTimeToRTC();
    int getHour();
    int getMinute();
    int getSecond();
    void printLocalTime();
    int getUptimeSec();
    int getBatteryPercent();
    int getBatteryVoltage();
}

#endif