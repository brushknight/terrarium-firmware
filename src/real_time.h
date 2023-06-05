#ifndef TERRARIUM_RTC
#define TERRARIUM_RTC

#define __BSD_VISIBLE 1

#include <sys/time.h>
#include "Arduino.h"
#include <RTClib.h>
#include "utils.h"
#include <string>
#include "data_structures.h"
// #include "event.h"

namespace RealTime
{

    static const char *TAG = "real-time";

    // "CET-1CEST,M3.5.0/2,M10.5.0/ 3"; // https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html

    class RealTime
    {
        std::string timeZone;
        bool ntpEnabled;
        RTC_DS3231 *rtc;
        const char *ntpServer1 = "0.europe.pool.ntp.org";
        const char *ntpServer2 = "1.europe.pool.ntp.org";
        const char *ntpServer3 = "2.europe.pool.ntp.org";

    public:
        RealTime(std::string tz, bool ntp, RTC_DS3231 *givenRtc)
        {
            timeZone = tz;
            ntpEnabled = ntp;
            rtc = givenRtc;
            rtc->begin();
        }
        bool isRtcSyncRequired()
        {
            return true;                       // debug
            return getUnixtime() < 1644065211; // Sat, 05 Feb 2022 12:46:48 GMT
        }
        bool isNTP()
        {
            return ntpEnabled;
        }
        void setNTPStatus(bool ntp)
        {
            ntpEnabled = ntp;
        }
        void updateTimeZOne(std::string tz)
        {
            timeZone = tz;
            setenv("TZ", timeZone.c_str(), 1);
            ESP_LOGD(TAG, "timezone: %s", timeZone.c_str());
        }
        time_t getUnixtime()
        {
            struct timeval tv;
            time_t t;
            struct tm *info;

            gettimeofday(&tv, NULL);
            return tv.tv_sec;
        }
        Time getTimeObj()
        {
            struct timeval tv;
            time_t t;
            struct tm *info;

            gettimeofday(&tv, NULL);
            t = tv.tv_sec;

            ESP_LOGD(TAG, "System timestamp %d", t);

            info = localtime(&t);
            // ESP_LOGD(TAG, "%d %d", info->tm_hour, info->tm_min);

            return Time(info->tm_hour, info->tm_min);
        }
        int getUptimeSec() {}
        bool syncFromRTC()
        {
            DateTime rtcDateTime = rtc->now();

            struct timeval tv;
            tv.tv_sec = rtcDateTime.unixtime(); // 1682452286; // 6:45:29 PM GMT+02:00 DST

            ESP_LOGD(TAG, "Timestamp from RTC: %d", tv.tv_sec);

            settimeofday(&tv, NULL);
            ESP_LOGD(TAG, "timezone: %s", timeZone.c_str());
            setenv("TZ", timeZone.c_str(), 1); // https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
            tzset();

            return true;
        }
        bool syncFromNTP(bool once)
        {
            if (!ntpEnabled)
            {
                ESP_LOGW(TAG, "NTP is disabled");
                return false;
            }

            ESP_LOGD(TAG, "[..] Sync from NTP");
            struct tm timeinfo;
            int attempts = 0;
            configTzTime(timeZone.c_str(), ntpServer1, ntpServer2, ntpServer3);
            while (!once && getUnixtime() < 946681200)
            {
                ESP_LOGE(TAG, "Failed to obtain time, retrying: %d", getUnixtime());
                configTzTime(timeZone.c_str(), ntpServer1, ntpServer2, ntpServer3);
                attempts++;
                vTaskDelay(0.1 * 1000 / portTICK_PERIOD_MS);
            }
            ESP_LOGD(TAG, "[OK] Sync from NTP, %d", getUnixtime());
            return true;
        }
        void setTimestamp(uint32_t timestamp, std::string tz)
        {
            struct timeval tv;
            tv.tv_sec = timestamp;
            timeZone = tz;
            setenv("TZ", timeZone.c_str(), 1); // https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
            // tzset();

            DateTime dt = DateTime(timestamp);
            ESP_LOGD(TAG, "timestamp before saving: %d", dt.unixtime());
            rtc->adjust(dt);
        }
        bool saveTimeToRTC()
        {
            ESP_LOGD(TAG, "[..] Saving time into RTC");

            struct timeval tv;
            gettimeofday(&tv, NULL);

            DateTime dt = DateTime(tv.tv_sec);
            ESP_LOGD(TAG, "timestamp before saving: %d", dt.unixtime());
            rtc->adjust(dt);

            ESP_LOGD(TAG, "[OK] Saving time into RTC");

            return true;
        }
        void printTime()
        {
            Time time = getTimeObj();
            ESP_LOGD(TAG, "Local time %d:%d %d", time.hours, time.minutes, getUnixtime());
        }
    };

    int getBatteryPercent();
    int getBatteryVoltage();
}

#endif