#include "real_time.h"

namespace RealTime
{

    // todo - make it custom ntp - optional
    const char *ntpServer1 = "10.0.0.51";
    const char *ntpServer2 = "pool.ntp.org";
    const char *ntpServer3 = "1.europe.pool.ntp.org";
    const long gmtOffset_sec = 3600;                        // todo fix this to be +1
    const int daylightOffset_sec = 3600;                    // fix this to accept DST
    const char *timeZone = "CET-1CEST,M3.5.0/2,M10.5.0/ 3"; // https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html

    RTC_DS3231 rtc;

    bool rtcBeginFailed = false;

    void initRTC()
    {
        rtcBeginFailed = !rtc.begin();
    }

    bool isWiFiRequired()
    {
        return rtcBeginFailed || isRtcSyncRequired();
    }

    bool isRtcSyncRequired()
    {
        DateTime rtcDateTime = rtc.now();
        return rtcDateTime.unixtime() < 1644065211; // Sat, 05 Feb 2022 12:46:48 GMT
    }

    void setup()
    {
        Serial.println("RealTime: setup started");

        if (rtcBeginFailed)
        {
            // Status::setFetchingTimeStatus(Status::WARNING);
            Serial.println("Couldn't find RTC, check wiring!");
            // Serial.flush();
            // syncFromNTP();
            // // abort(); -> only if NTP time failed

            // printLocalTime();

            return;
        }

        Serial.println("RealTime: RTC enalbed");

        syncFromRTC();

        delay(1000);

        if (isRtcSyncRequired())
        {
            Serial.println("RealTime: RTC time expired");
            syncFromNTP();
            // validate time here
            saveTimeToRTC();
        }

        printLocalTime();

        Utils::log("RealTime: setup finished");
    }

    void syncFromRTC()
    {
        DateTime rtcDateTime = rtc.now();
        struct timeval tv;
        tv.tv_sec = rtcDateTime.unixtime();

        Serial.printf("TIMESTAMP from RTC %d\n", tv.tv_sec);

        settimeofday(&tv, NULL);
        setenv("TZ", timeZone, 1); // https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
        tzset();
    }

    void syncFromNTP()
    {
        Serial.println("RealTime: sync from NTP");
        Net::connect();
        struct tm timeinfo;
        int attempts = 0;
        while (!getLocalTime(&timeinfo))
        {
            Serial.println("Failed to obtain time, retry");
            configTzTime(timeZone, ntpServer1, ntpServer2, ntpServer3);
            // configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2, ntpServer3);
            attempts++;
        }
    }

    void syncFromNTPOnce()
    {
        Serial.println("RealTime: sync from NTP Once");
        Net::connect();
        struct tm timeinfo;
        int attempts = 0;
        if (!getLocalTime(&timeinfo))
        {
            Serial.println("Failed to obtain time, retry");
            configTzTime(timeZone, ntpServer1, ntpServer2, ntpServer3);
            // configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2, ntpServer3);
            attempts++;
        }
    }

    bool saveTimeToRTC()
    {
        Serial.println("RealTime: saving time into RTC");

        time_t now;
        struct tm timeDetails;

        time(&now);
        localtime_r(&now, &timeDetails);

        rtc.adjust(mktime(&timeDetails));

        return true;
    }

    std::string getTime()
    {
        int hour = 0;
        int minute = 0;
        int second = 0;

        struct tm timeinfo;
        if (!getLocalTime(&timeinfo))
        {
            Serial.println("getHour() Failed to obtain time");

            return 0;
        }

        hour = timeinfo.tm_hour;
        minute = timeinfo.tm_min;
        second = timeinfo.tm_sec;

        return Time::hourMinuteToString(hour, minute);
    }

    Time getTimeObj()
    {
        time_t now;
        struct tm timeDetails;

        time(&now);
        localtime_r(&now, &timeDetails);

        return Time(timeDetails.tm_hour, timeDetails.tm_min);
    }

    int getHour()
    {
        return getTimeObj().hours;
    }

    int getMinute()
    {
        return getTimeObj().minutes;
    }

    int getSecond()
    {
        int second = 0;

        struct tm timeinfo;
        if (!getLocalTime(&timeinfo))
        {
            Serial.println("getSecond() Failed to obtain time");
            // abort();
            return 0;
        }

        second = timeinfo.tm_sec;

        return second;
    }

    void printLocalTime()
    {

        time_t now;
        struct tm timeDetails;

        time(&now);
        localtime_r(&now, &timeDetails);

        Serial.println(&timeDetails, "TIME > %A, %B %d %Y %H:%M:%S");
    }

    int getBatteryPercent()
    {

        float oneMilliVolt = 3300.0 / 4096.0;

        float full = 3000.0 / oneMilliVolt;

        float empty = 2000.0 / oneMilliVolt;

        float current = float(analogRead(33));

        // TODO add check for zero
        if (full - empty == 0)
        {
            return 0;
        }

        float percent = (current - empty) / (full - empty);

        // if (percent > 1)
        // {
        //     percent = 1;
        // }

        return int(percent * 100.0);
    }

    int getBatteryVoltage()
    {

        float oneMilliVolt = 3300.0 / 4096.0;

        return int(float(analogRead(33)) * oneMilliVolt);
    }

    int getUptimeSec()
    {
        return esp_timer_get_time() / 1000000;
    }

}
