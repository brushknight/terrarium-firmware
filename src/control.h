#ifndef TERRARIUM_CONTROL
#define TERRARIUM_CONTROL

#include "Arduino.h"
#include <Adafruit_NeoPixel.h>
#include "config.h"
#include "data_structures.h"
#include <PCF8574.h>

namespace Control
{

    static const char *TAG = "control";

    void analogPinHigh(int pin);
    void analogPinLow(int pin);

    struct HardwareLayer
    {

    public:
        PCF8574 pcf = PCF8574(0x20);
        bool isGPIOExpanderFound = false;

        HardwareLayer() {}
        void begin()
        {
            // Set pinMode to OUTPUT
            pcf.pinMode(0, OUTPUT);
            pcf.pinMode(1, OUTPUT);
            pcf.pinMode(2, OUTPUT);
            pcf.pinMode(3, OUTPUT);

            ESP_LOGD(TAG, "Init pcf8574");
            if (pcf.begin())
            {
                ESP_LOGD(TAG, "pcf8574 connected");
                isGPIOExpanderFound = true;
            }
            else
            {
                ESP_LOGE(TAG, "no pcf8574 found"); // means this is old pcb
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
        int length;
        int ledPin;
        void applyHardware()
        {
            for (int i = 0; i < length; i++)
            {
                pixels->setPixelColor(i, state.red, state.green, state.blue);
            }
            pixels->setBrightness(brightness);
            pixels->show();
        }

    public:
        ColorLight(){};
        ColorLight(int pin, int l)
        {
            ledPin = pin;
            length = l;
            int pixelFormat = NEO_GRB + NEO_KHZ800;
            pixels = new Adafruit_NeoPixel(length, ledPin, pixelFormat);
            pixels->begin();
        };
        bool enabled()
        {
            return ledPin > -1;
        }
        Color status()
        {
            return state;
        }
        void on()
        {
            brightness = 255;
            applyHardware();
        }
        void off()
        {
            brightness = 0;
            applyHardware();
        }
        void setColorAndBrightness(Color c, int percent)
        {
            state = c;
            brightness = 255.0 * (((float)percent) / 100.0);
            if (brightness > 255)
            {
                brightness = 255;
            }
            if (brightness < 0)
            {
                brightness = 0;
            }
            applyHardware();
        }
        void setPixel(int index, Color color)
        {
            pixels->setPixelColor(index, color.red, color.green, color.blue);
        }
        void setBrightness(int brightness)
        {
            pixels->setBrightness(brightness);
        }
        void show()
        {
            pixels->show();
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
        void begin()
        {
            hardwareLayer.begin();
            switches.list[0] = Switch(0, &hardwareLayer);
            switches.list[1] = Switch(1, &hardwareLayer);
            switches.list[2] = Switch(2, &hardwareLayer);

            dimmers.list[0] = Dimmer(0);
            dimmers.list[1] = Dimmer(1);
            dimmers.list[2] = Dimmer(2);

            // Only compatible with controller v1.9 and higher
            // colorLights.list[0] = ColorLight(32, 30);
        }

        void beginLightDome()
        {
            colorLights.list[0] = ColorLight(32, 20);
            colorLights.list[1] = ColorLight(25, 20);
            colorLights.list[2] = ColorLight(33, 20);
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
            if (percent > 100)
            {
                percent = 100;
            }

            if (port < 0 || port > 2)
            {
                return false;
            }

            colorLights.list[port].setColorAndBrightness(color, percent);
            return true;
        };
        bool lightDomeStartupAnimation()
        {

            colorLights.list[0].setColorAndBrightness(Color(0, 0, 0), 100);
            colorLights.list[1].setColorAndBrightness(Color(0, 0, 0), 100);
            colorLights.list[2].setColorAndBrightness(Color(0, 0, 0), 100);

            Color color = Color(0, 0, 255);

            colorLights.list[1].setPixel(0, color);

            for (int i = 0; i < 19; i++)
            {
                colorLights.list[0].setPixel(i, color);
                colorLights.list[1].setPixel(i + 1, color);
                colorLights.list[2].setPixel(i, color);
                colorLights.list[0].show();
                colorLights.list[1].show();
                colorLights.list[2].show();
                delay(100);
            }
            colorLights.list[0].setPixel(19, color);
            colorLights.list[2].setPixel(19, color);
            colorLights.list[0].show();
            colorLights.list[2].show();
            delay(100);

            colorLights.list[0].setColorAndBrightness(Color(0, 0, 0), 100);
            colorLights.list[1].setColorAndBrightness(Color(0, 0, 0), 100);
            colorLights.list[2].setColorAndBrightness(Color(0, 0, 0), 100);

            return true;
        };
    };
}

#endif