#ifndef TERRARIUM_CONTROL
#define TERRARIUM_CONTROL

#include "Arduino.h"
#include <Adafruit_NeoPixel.h>
#include "config.h"
#include <PCF8574.h>

namespace Control
{

    void analogPinHigh(int pin);
    void analogPinLow(int pin);

    struct HardwareLayer
    {

    public:
        PCF8574 pcf = PCF8574(0x20);
        bool isGPIOExpanderFound = false;

        HardwareLayer(){}
        void begin(){
            // Set pinMode to OUTPUT
            pcf.pinMode(0, OUTPUT);
            pcf.pinMode(1, OUTPUT);
            pcf.pinMode(2, OUTPUT);
            pcf.pinMode(3, OUTPUT);

            Serial.println("Init pcf8574");
            if (pcf.begin())
            {
                Serial.println("pcf8574 connected");
                isGPIOExpanderFound = true;
            }
            else
            {
                Serial.println("no pcf8574 found"); // means this is old pcb
            }
        }
        void setExpanderPort(int port, bool value)
        {
            pcf.digitalWrite(port, value ? HIGH : LOW);
        }

        void setRelayTo(int relayIndex, bool value)
        {
            if (isGPIOExpanderFound)
            {
                setExpanderPort(relayIndex, value); // right now relays are 0,1,2 ports of expander
            }
            else
            {
                int pinNumber = RELAY_PINS[relayIndex];
                if (value)
                {
                    analogPinHigh(pinNumber);
                }
                else
                {
                    analogPinLow(pinNumber);
                }
            }
        }
    };

    struct Color
    {

    public:
        int red = 0;
        int green = 0;
        int blue = 0;
        static int jsonSize()
        {
            return 64; // to be defined
        }
        DynamicJsonDocument toJSON()
        {
            DynamicJsonDocument doc(jsonSize());
            doc["r"] = red;
            doc["g"] = green;
            doc["b"] = blue;
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
            color.red = doc["r"];
            color.green = doc["g"];
            color.blue = doc["b"];
            return color;
        }
    };
    class Switch
    {
    private:
        int port = -1;
        bool state = false;
        HardwareLayer *hardwareLayer;

        void applyHardware()
        {
            hardwareLayer->setRelayTo(port, state);
        }

    public:
        Switch(){};
        Switch(int p, HardwareLayer *hl)
        {
            port = p;
            hardwareLayer = hl;
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
            brightness = 255.0 * (((float)percent) / 100.0);
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
        HardwareLayer hardwareLayer = HardwareLayer();

    public:
        Controller(){};
        void begin(){
            hardwareLayer.begin();
            switches.list[0] = Switch(0, &hardwareLayer);
            switches.list[1] = Switch(1, &hardwareLayer);
            switches.list[2] = Switch(2, &hardwareLayer);

            dimmers.list[0] = Dimmer(0);
            dimmers.list[1] = Dimmer(1);
            dimmers.list[2] = Dimmer(2);

            colorLights.list[0] = ColorLight();
        }
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