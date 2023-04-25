#ifndef TERRARIUM_RTC
#define TERRARIUM_RTC

#define __BSD_VISIBLE 1

#include <sys/time.h>
#include "Arduino.h"
// #include <RTClib.h>
// #include "utils.h"
#include <string>
// #include "data_structures.h"
// #include "event.h"

namespace RealTime
{
    static const char *TAG = "real-time";

    void setup();
    bool isWiFiRequired();
    void syncFromRTC();
    void syncFromNTP();
    void syncFromNTPOnce();
    void setTimestamp(uint32_t timestamp, std::string tz);
    bool saveTimeToRTC();
    bool isRtcSyncRequired();
    void initRTC(std::string tz, bool ntp);
    void updateTimeZone(std::string tz);
    // Time getTimeObj();
    int getHour();
    int getMinute();
    void printLocalTime();
    int getUptimeSec();
    int getBatteryPercent();
    int getBatteryVoltage();
    bool checkScheduleTimeWindow(std::string now, std::string since, std::string until);
}

#endif