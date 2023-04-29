#include "real_time.h"

namespace RealTime
{

    void printLocalTime()
    {
        struct timeval tv;
        time_t t;
        struct tm *info;

        gettimeofday(&tv, NULL);
        t = tv.tv_sec;

        info = localtime(&t);

        ESP_LOGI(TAG, "%d:%d:%d", info->tm_hour, info->tm_min, info->tm_sec);
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
