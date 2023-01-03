#ifndef TERRARIUM_CONTROL
#define TERRARIUM_CONTROL

#include "Arduino.h"
#include <Adafruit_NeoPixel.h>
#include "config.h"
#include "data_structures.h"

namespace Control
{

    void analogPinHigh(int pin);
    void analogPinLow(int pin);

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
            pinMode(RELAY_PINS[port], OUTPUT);
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
        void applyHardware()
        {
            // work around to control "dimmers" with relays
            if (brightness > 0)
            {
                analogPinHigh(RELAY_PINS[port]);
            }
            else
            {
                analogPinLow(RELAY_PINS[port]);
            }
        }

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
            applyHardware();
        }
    };
    class ColorLight
    {
    private:
        Color state;
        int brightness = 0; // max 255
        Adafruit_NeoPixel *pixels;
        int pixelsCount = 30;
        int pixelsOffset = 1;
        int ledPin = LEDPIN;
        void applyHardware()
        {
            for (int i = pixelsOffset; i < pixelsCount + pixelsOffset; i++)
            {
                pixels->setPixelColor(i, (state.red << 16) + (state.green << 8) + state.blue);
            }
            pixels->setBrightness(brightness);
            pixels->show();
        }

    public:
        // ColorLight(){};
        ColorLight()
        {
            int pixelFormat = NEO_GRB + NEO_KHZ800;
            pixels = new Adafruit_NeoPixel(pixelsCount + pixelsOffset, ledPin, pixelFormat);
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
            brightness = 0;
            applyHardware();
        }
        void off()
        {
            brightness = 255;
            applyHardware();
        }
        void setColorAndBrightness(Color c, int percent)
        {
            state = c;
            brightness = 255.0 * (((float)percent)/100.0);
            applyHardware();
        }
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

    class Controller
    {
    private:
        Switches switches = Switches();
        Dimmers dimmers = Dimmers();
        ColorLights colorLights = ColorLights();

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
        void resetPorts()
        {
            switches.list[0].off();
            switches.list[1].off();
            switches.list[2].off();
        }
        bool turnSwitchOn(int port)
        {
            if (port < 0 || port > 2)
            {
                return false;
            }
            switches.list[port].on();
            return true;
        };
        bool turnSwitchOff(int port)
        {
            if (port < 0 || port > 2)
            {
                return false;
            }
            switches.list[port].off();
            return true;
        };
        bool setDimmer(int port, int percent)
        {
            if (port < 0 || port > 2)
            {
                return false;
            }
            dimmers.list[port].setBrigntness(percent);
            return true;
        };
        bool setColorAndBrightness(int port, Color color, int percent)
        {
            // Serial.printf("LED %d %d%% %d %d %d\n", port, percent, color.red, color.green, color.blue);
            colorLights.list[port].setColorAndBrightness(color, percent);
            return true;
        };
    };
}

#endif