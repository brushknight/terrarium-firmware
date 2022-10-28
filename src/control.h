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
        int r = 0;
        int g = 0;
        int b = 0;
        int brightness = 0;
        static int jsonSize()
        {
            return 64; // to be defined
        }
        DynamicJsonDocument toJSON()
        {
            DynamicJsonDocument doc(jsonSize());
            doc["r"] = r;
            doc["g"] = g;
            doc["b"] = b;
            doc["brightness"] = brightness;
            return doc;
        }

        static Color fromJSON(std::string json)
        {
            DynamicJsonDocument doc(jsonSize());
            deserializeJson(doc, json);

            return Color::fromJSONObj(doc);
        }

        static Color fromJSONObj(DynamicJsonDocument doc)
        {
            Color color;
            color.r = doc["r"];
            color.g = doc["g"];
            color.b = doc["b"];
            color.brightness = doc["brightness"];
            return color;
        }
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
        Adafruit_NeoPixel *pixels;
        int pixelsCount = 1;

    public:
        // ColorLight(){};
        ColorLight()
        {

            int pixelFormat = NEO_GRB + NEO_KHZ800;
            pixels = new Adafruit_NeoPixel(pixelsCount, LEDPIN, pixelFormat);
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
        bool setColor(int port, Color color)
        {
            colorLights.list[port].setColor(color);
            return true;
        };
    };
}

#endif