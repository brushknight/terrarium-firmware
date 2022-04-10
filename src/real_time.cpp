#include "real_time.h"

namespace RealTime
{

    const char *ntpServer1 = "10.0.0.51";
    const char *ntpServer2 = "pool.ntp.org";
    const char *ntpServer3 = "1.europe.pool.ntp.org";
    const long gmtOffset_sec = 3600;  // todo fix this to be +1
    const int daylightOffset_sec = 0; // fix this to accept DST
    //const char* timezoneNTP = "CET";

    // setup(rtc_enabled)
    // syncFromNTP()
    // flashRTC(time)
    // syncFromRTC()
    // getTime()
    // getHour()
    // getMinute()
    // getSecond()
    // getDate()

    RTC_DS3231 rtc;

    bool isWiFiRequired()
    {
        if (!rtc.begin())
        {
            Serial.println("Couldn't find RTC, check wiring!");
            return true;
        }
        return rtc.lostPower();
    }

    void setup(bool rtcEnabled)
    {
        // Status::setFetchingTimeStatus(Status::WORKING);
        Serial.println("RealTime: setup started");

        if (rtcEnabled)
        {
            Serial.println("RealTime: RTC enalbed");
            if (!rtc.begin())
            {
                // Status::setFetchingTimeStatus(Status::WARNING);
                Serial.println("Couldn't find RTC, check wiring!");
                Serial.flush();
                syncFromNTP();
                //abort(); -> only if NTP time failed

                printLocalTime();

                return;
            }

            // or rtc time is a shit
            if (!rtc.lostPower())
            {
                syncFromRTC();
            }
            else
            {
                // Status::setFetchingTimeStatus(Status::WARNING);
                Serial.println("RealTime: RTC lost power");
                syncFromNTP();
                // validate time here
                saveTimeToRTC();
            }
        }

        else
        {
            syncFromNTP();
        }
        printLocalTime();
        Serial.println("RealTime: setup finished");
        Utils::log("RealTime: setup finished");
        // Status::setFetchingTimeStatus(Status::IDLE);
    }

    void syncFromRTC()
    {
        Serial.println("RealTime: sync from RTC");
        DateTime rtcDateTime = rtc.now();

        Serial.print("minutes from RTC: ");
        Serial.println(rtcDateTime.minute());

        struct timeval tv;
        tv.tv_sec = rtcDateTime.unixtime();

        if (tv.tv_sec < 1644065211)
        { // Sat, 05 Feb 2022 12:46:48 GMT
            syncFromNTP();
        }
        else
        {
            Serial.print("UNIX timestamp from RTC: ");
            Serial.println(tv.tv_sec);

            timezone tz_utcPlus1 = {gmtOffset_sec, daylightOffset_sec};

            //setenv("TZ",timezone.c_str(),1);

            settimeofday(&tv, &tz_utcPlus1);

            struct timeval tv_set;
            gettimeofday(&tv_set, &tz_utcPlus1);

            Serial.print("UNIX timestamp set from RTC: ");
            Serial.println(tv_set.tv_sec);
        }
    }

    void syncFromNTP()
    {
        Serial.println("RealTime: sync from NTP");
        struct tm timeinfo;
        int attempts = 0;
        while (!getLocalTime(&timeinfo))
        {
            Serial.println("Failed to obtain time, retry");
            //configTzTime(timezoneNTP, ntpServer1, ntpServer2, ntpServer3);
            configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2, ntpServer3);
            attempts++;
        }
    }

    bool saveTimeToRTC()
    {
        Serial.println("RealTime: saving time into RTC");
        struct tm timeinfo;
        if (getLocalTime(&timeinfo))
        {

            Serial.print("minutes saving into RTC: ");
            Serial.println(timeinfo.tm_min);

            rtc.adjust(mktime(&timeinfo));
            return true;
        }
        else
        {
            Serial.print("Failed to flash RTC");
            return false;
        }
    }

    int getHour()
    {
        int hour = 0;

        struct tm timeinfo;
        if (!getLocalTime(&timeinfo))
        {
            Serial.println("getHour() Failed to obtain time");

            return 0;
        }

        hour = timeinfo.tm_hour;
        return hour;
    }

    int getMinute()
    {

        // time_t now;
        // struct tm timeinfo1;
        // time(&now);
        // localtime_r(&now, &timeinfo1);

        // Serial.print(">>>>> minutes RTC: ");
        // Serial.println(timeinfo1.tm_min);

        int minute = 0;

        struct tm timeinfo;
        if (!getLocalTime(&timeinfo))
        {
            Serial.println("getMinute() Failed to obtain time");
            //abort();
            return 0;
        }

        minute = timeinfo.tm_min;

        return minute;
    }

    int getSecond()
    {
        int second = 0;

        struct tm timeinfo;
        if (!getLocalTime(&timeinfo))
        {
            Serial.println("getSecond() Failed to obtain time");
            //abort();
            return 0;
        }

        second = timeinfo.tm_sec;

        return second;
    }

    void printLocalTime()
    {
        struct tm timeinfo;
        if (!getLocalTime(&timeinfo))
        {
            Serial.println("printLocalTime() Failed to obtain time");
            return;
        }
        Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
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

    bool checkScheduleTimeWindow(std::string now, std::string since, std::string until)
    {

        if (since.compare(until) < 0)
        {
            return since.compare(now) <= 0 && until.compare(now) > 0;
        }
        else
        {
            return since.compare(now) <= 0 || until.compare(now) > 0;
        }
    }
}
