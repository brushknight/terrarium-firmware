#include "status.h"

namespace Status
{

#define LEDPIN 23
#define LED_COUNT 1

const int statusLedBrightness = 5;


    int pixelFormat = NEO_GRB + NEO_KHZ800;

    Adafruit_NeoPixel *pixels;

    int climateStatus = IDLE;
    int fetchingTimeStatus = IDLE;
    int connectingToWiFiStatus = IDLE;

    void setup()
    {
        pixels = new Adafruit_NeoPixel(LED_COUNT, LEDPIN, pixelFormat);
        pixels->begin();
        pixels->setBrightness(statusLedBrightness);
        turnLedOff();
    }

    void setClimateStatus(int status)
    {
        climateStatus = status;
    }

    void setFetchingTimeStatus(int status)
    {
        fetchingTimeStatus = status;
    }

    void setConnectingToWiFiStatus(int status)
    {
        connectingToWiFiStatus = status;
    }

    void handleStatusLed()
    {

        if (climateStatus != IDLE)
        {
            switch (climateStatus)
            {
            case WORKING:
                setPink();
                return;
            case SUCCESS:
                setPink();
                return;
            case WARNING:
                setWarning();
                return;
            case ERROR:
                setError();
                return;
            default:
                return;
            }
        }

        if (connectingToWiFiStatus != IDLE)
        {
            switch (connectingToWiFiStatus)
            {
            case WORKING:
                setBlue();
                return;
            case SUCCESS:
                setBlue();
                return;
            case WARNING:
                setWarning();
                return;
            case ERROR:
                setError();
                return;
            default:
                return;
            }
        }

        if (fetchingTimeStatus != IDLE)
        {
            switch (fetchingTimeStatus)
            {
            case WORKING:
                setMint();
                return;
            case SUCCESS:
                setMint();
                return;
            case WARNING:
                setWarning();
                return;
            case ERROR:
                setError();
                return;
            default:
                return;
            }
        }

        turnLedOff();
    }

    void setWarning()
    {
        turnLedOn(255, 106, 0);
    }
    void setError()
    {
        turnLedOn(255, 0, 0);
    }
    // void ok()
    // {
    //     turnLedOn(84, 22, 180);
    // }
    // void connectingToWifi()
    // {
    //     turnLedOn(0, 181, 236);
    // }
    // void startHttpServer()
    // {
    //     turnLedOn(0, 255, 0);
    // }
    // void fetchTime()
    // {
    //     turnLedOn(254, 203, 0);
    // }

    void setOrange()
    {
        turnLedOn(255, 106, 0);
    }

    void setGreen(){
        turnLedOn(0, 255, 0);
    }

    void setPurple()
    {
        turnLedOn(84, 22, 180);
    }

    void setPink()
    {
        turnLedOn(237, 94, 191);
    }

    void setMint()
    {
        turnLedOn(94, 237, 211);
    }
    void setBlue()
    {
        turnLedOn(0, 181, 236);
    }

    void turnLedOff()
    {

        pixels->setPixelColor(0, 0, 0, 0);
        pixels->show();
    }

    void turnLedOn(int r, int g, int b)
    {

        pixels->setPixelColor(0, r, g, b);
        pixels->show();
    }
}