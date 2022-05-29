#ifndef TERRARIUM_STATUS
#define TERRARIUM_STATUS

#include <Adafruit_NeoPixel.h>

namespace Status
{

    const int IDLE = 0;
    const int WORKING = 1;
    const int SUCCESS = 2;
    const int WARNING = 100;
    const int ERROR = 101;

    void setup();
    void setWarning();
    void setError();
    void setPink();
    void setPurple();
    void setMint();
    void setBlue();
    void handleStatusLed();
    // void ok();
    // void connectingToWifi();
    // void startHttpServer();
    // void fetchTime();
    void turnLedOn(int r, int g, int b);
    void turnLedOff();
    void setClimateStatus(int status);
    void setFetchingTimeStatus(int status);
    void setConnectingToWiFiStatus(int status);
}

#endif