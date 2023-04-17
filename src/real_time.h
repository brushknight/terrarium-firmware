#ifndef TERRARIUM_RTC
#define TERRARIUM_RTC

#include <sys/time.h>
#include "Arduino.h"
#include <RTClib.h>
#include "net.h"
#include "utils.h"
#include <string>
#include "event.h"
#include "data_structures.h"

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
    std::string getTime();
    Time getTimeObj();
    int getHour();
    int getMinute();
    int getSecond();
    void printLocalTime();
    int getUptimeSec();
    int getBatteryPercent();
    int getBatteryVoltage();
    bool checkScheduleTimeWindow(std::string now, std::string since, std::string until);
}

#endif