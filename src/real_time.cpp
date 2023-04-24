#include "real_time.h"

namespace RealTime
{
    // todo - make it custom ntp - optional
    const char *ntpServer1 = "0.europe.pool.ntp.org"; 
    const char *ntpServer2 = "1.europe.pool.ntp.org";
    const char *ntpServer3 = "2.europe.pool.ntp.org";
    // const char *timeZone = "CET-1CEST"; // https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html

    std::string timeZone = "CET-1CEST,M3.5.0/2,M10.5.0/ 3"; // https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
    bool ntpEnabled = false;

    RTC_DS3231 rtc;

    bool rtcBeginFailed = false;

    void initRTC(std::string tz, bool ntp)
    {
        timeZone = tz;
        ntpEnabled = ntp;
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

        ESP_LOGI(TAG, "[..] Enabling RTC chip");

        if (rtcBeginFailed)
        {
            ESP_LOGE(TAG, "[FAIL] Couldn't find RTC chip");
            // Status::setFetchingTimeStatus(Status::WARNING);
            // Serial.flush();
            // syncFromNTP();
            // // abort(); -> only if NTP time failed

            // printLocalTime();

            return;
        }

        ESP_LOGI(TAG, "[OK] Enabling RTC chip");
        ESP_LOGI(TAG, "[..] Sync time from RTC");

        syncFromRTC();

        delay(1000);

        if (isRtcSyncRequired())
        {
            ESP_LOGD(TAG, "RTC time expired");
            syncFromNTP();
            saveTimeToRTC();
        }

        printLocalTime();

        ESP_LOGI(TAG, "[OK] Sync time from RTC");
    }

    void syncFromRTC()
    {
        DateTime rtcDateTime = rtc.now();
        struct timeval tv;
        tv.tv_sec = rtcDateTime.unixtime();

        ESP_LOGD(TAG, "Timestamp from RTC: %d", tv.tv_sec);

        settimeofday(&tv, NULL);
        setenv("TZ", timeZone.c_str(), 1); // https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
        tzset();

        Serial.println(getTime().c_str());
    }

    void setTimestamp(uint32_t timestamp, std::string tz)
    {
        struct timeval tv;
        tv.tv_sec = timestamp;
        timeZone = tz;
        setenv("TZ", timeZone.c_str(), 1); // https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
        tzset();

        DateTime dt = DateTime(timestamp);
        rtc.adjust(dt);
    }

    void syncFromNTP()
    {
        if (!ntpEnabled)
        {
            ESP_LOGW(TAG, "NTP is disabled");
            return;
        }

        ESP_LOGD(TAG, "[..] Sync from NTP");
        struct tm timeinfo;
        int attempts = 0;
        while (!getLocalTime(&timeinfo))
        {
            ESP_LOGE(TAG, "Failed to obtain time, retrying");
            configTzTime(timeZone.c_str(), ntpServer1, ntpServer2, ntpServer3);
            attempts++;
        }
        ESP_LOGD(TAG, "[OK] Sync from NTP");
    }

    void syncFromNTPOnce()
    {

        if (!ntpEnabled)
        {
            ESP_LOGW(TAG, "NTP is disabled");
            return;
        }

        ESP_LOGD(TAG, "[..] Sync from NTP Once");

        struct tm timeinfo;
        int attempts = 0;
        if (!getLocalTime(&timeinfo))
        {
            ESP_LOGE(TAG, "Failed to obtain time, retrying");
            configTzTime(timeZone.c_str(), ntpServer1, ntpServer2, ntpServer3);
            attempts++;
        }
        ESP_LOGD(TAG, "[OK] Sync from NTP Once");
    }

    bool saveTimeToRTC()
    {
        ESP_LOGD(TAG, "[..] Saving time into RTC");

        time_t now;
        struct tm timeDetails;

        time(&now);
        localtime_r(&now, &timeDetails);

        rtc.adjust(mktime(&timeDetails));

        ESP_LOGD(TAG, "[OK] Saving time into RTC");

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
            ESP_LOGE(TAG, "getHour() Failed to obtain time");
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
            ESP_LOGE(TAG, "getSecond() Failed to obtain time");
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

        // check it this format works

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
