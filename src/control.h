#ifndef TERRARIUM_CONTROL
#define TERRARIUM_CONTROL

#include "Arduino.h"
#include <Adafruit_NeoPixel.h>
#include "config.h"

namespace Control
{

    void analogPinHigh(int pin);
    void analogPinLow(int pin);

    struct Color
    {
        int r;
        int g;
        int b;
        int brightness;
    };

    class Controller
    {
    private:
        Switches switches;
        Dimmers dimmers;
        ColorLights colorLights;

    public:
        Controller()
        {
            switches.list[0] = Switch(0);
            switches.list[1] = Switch(1);
            switches.list[2] = Switch(2);

            dimmers.list[0] = Dimmer(0);
            dimmers.list[1] = Dimmer(1);
            dimmers.list[2] = Dimmer(2);

            colorLights.list[0] = ColorLight();
        };
        bool turnSwitchOn(int port)
        {
            switches.list[port].on();
        };
        bool turnSwitchOff(int port)
        {
            switches.list[port].off();
        };
        bool setDimmer(int port, int percent)
        {
            dimmers.list[port].setBrigntness(percent);
        };
        bool setColor(int port, Color color)
        {
            colorLights.list[port].setColor(color);
        };
    };

    class Switches
    {
    public:
        Switch list[3];
    };

    class Dimmers
    {
    public:
        Dimmer list[3];
    };

    class ColorLights
    {
    public:
        ColorLight list[3];
    };

    class Switch
    {
    private:
        int port = -1;
        bool state = false;

        void applyHardware()
        {
            if (state)
            {
                analogPinHigh(RELAY_PINS[port]);
            }
            else
            {
                analogPinLow(RELAY_PINS[port]);
            }
        }

    public:
        Switch(){};
        Switch(int p)
        {
            port = p;
        };
        bool enabled()
        {
            return port > -1;
        }
        bool status()
        {
            return state;
        }
        void on()
        {
            state = true;
            applyHardware();
        }
        void off()
        {
            state = false;
            applyHardware();
        }
    };
    class Dimmer
    {
    private:
        int port = -1;
        int brightness = 0;

    public:
        Dimmer(){};
        Dimmer(int p)
        {
            port = p;
        };
        bool enabled()
        {
            return port > -1;
        }
        int status()
        {
            return brightness;
        }
        void setBrigntness(int b)
        {
            brightness = b;
            // apply hardware changes
        }
    };
    class ColorLight
    {
    private:
        Color state;
        Adafruit_NeoPixel *pixels;
        int pixelsCount = 1;

    public:
        // ColorLight(){};
        ColorLight()
        {

            int pixelFormat = NEO_GRB + NEO_KHZ800;
            pixels = new Adafruit_NeoPixel(pixelsCount, WS2812_PIN, pixelFormat);
            pixels->begin();
        };
        // bool enabled()
        // {
        //     return port > -1;
        // }
        Color status()
        {
            return state;
        }
        void on()
        {
            state.brightness = 0;
        }
        void off()
        {
            state.brightness = 100;
        }
        void setColor(Color c)
        {
            state = c;
        }
    };
}

#endif