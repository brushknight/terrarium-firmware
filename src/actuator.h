#ifndef TERRARIUM_ACTUATOR
#define TERRARIUM_ACTUATOR

#include "Arduino.h"
#include <Adafruit_NeoPixel.h>
#include "config.h"
#include "data_structures.h"
#include <PCF8574.h>
#include "pitches.h"

namespace Actuator
{

    static const char *TAG = "actuator";

    void analogPinHigh(int pin);
    void analogPinLow(int pin);
    void analogPinSet(int pin, int value);
    void digitalPinSet(int pin, int value);

    struct HardwareLayer
    {

    public:
        PCF8574 pcf = PCF8574(0x20);
        bool isGPIOExpanderFound = false;

        HardwareLayer() {}
        void begin()
        {

            ESP_LOGD(TAG, "[..] Starting PCF8574 (IO expander)");
            if (pcf.begin())
            {
                ESP_LOGD(TAG, "[OK] Starting PCF8574 (IO expander)");
                isGPIOExpanderFound = true;

                ESP_LOGD(TAG, "[..] Resetting PCF8574 (IO expander)");
                for (int i = 0; i < 8; i++)
                {
                    pcf.pinMode(i, OUTPUT);
                    pcf.digitalWrite(i, LOW);
                }
                ESP_LOGD(TAG, "[OK] Resetting PCF8574 (IO expander)");
            }
            else
            {
                ESP_LOGD(TAG, "[FAIL] PCF8574 (IO expander) not found");
            }
        }
        void setExpanderRelayPort(int port, bool value)
        {
            int expanderPin = IO_EXPANDER_RELAY_PORTS[port];
            pcf.digitalWrite(expanderPin, value ? HIGH : LOW);
        }
        void setExpanderPin(int pin, bool value)
        {
            pcf.digitalWrite(pin, value ? HIGH : LOW);
        }

        void setRelayTo(int relayIndex, bool value)
        {
            if (isGPIOExpanderFound)
            {

                setExpanderRelayPort(relayIndex, value); // right now relays are 0,1,2 ports of expander
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
        void setFanTo(int port, int value)
        {
            int pin = FANS[port];
            analogPinSet(pin, value);
        }
        void setBuzzerTo(int port, int note)
        {
            int pin = BUZZERS[port];
            if (note == 0)
            {
                analogPinSet(pin, 0);
            }
            else
            {
                tone(pin, note, 50);
            }
        }
        void safetyRelayOn()
        {
            analogPinHigh(SAFETY_RELAY_PIN);
        }
        void safetyRelayOff()
        {
            analogPinLow(SAFETY_RELAY_PIN);
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
            // pinMode(RELAY_PINS[port], OUTPUT);
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
        bool test()
        {
            off();
            vTaskDelay(0.2 * 1000 / portTICK_PERIOD_MS);
            on();
            vTaskDelay(0.2 * 1000 / portTICK_PERIOD_MS);
            off();
            return true;
        }
    };
    class Dimmer
    {
    private:
        int port = -1;
        int brightness = 0;
        HardwareLayer *hardwareLayer;

        void applyHardware()
        {
            hardwareLayer->setRelayTo(port, brightness > 0);
        }
        // void applyHardware()
        // {
        // work around to control "dimmers" with relays
        // if (brightness > 0)
        // {
        //     analogPinHigh(RELAY_PINS[port]);
        // }
        // else
        // {
        //     analogPinLow(RELAY_PINS[port]);
        // }
        // }

    public:
        Dimmer(){};
        Dimmer(int p, HardwareLayer *hl)
        {
            port = p;
            hardwareLayer = hl;
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
        void off()
        {
            setBrigntness(0);
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
    class Fan
    {
    private:
        int port = -1;
        int state = 0;
        HardwareLayer *hardwareLayer;

        void applyHardware()
        {
            hardwareLayer->setFanTo(port, state);
        }

    public:
        Fan(){};
        Fan(int p, HardwareLayer *hl)
        {
            port = p;
            hardwareLayer = hl;
            // pinMode(RELAY_PINS[port], OUTPUT);
        };
        bool enabled()
        {
            return port > -1;
        }
        int status()
        {
            return state;
        }
        void on()
        {
            state = 255;
            applyHardware();
        }
        void off()
        {
            state = 0;
            applyHardware();
        }
        void adjust(int value)
        {
            state = value;
            applyHardware();
        }
        bool test()
        {
            for (int i = 1; i <= 10; i++)
            {
                adjust(10 * i);
                vTaskDelay(0.2 * 1000 / portTICK_PERIOD_MS);
                // TODO read feedback and check
            }

            off();

            return false;
        }
    };

    class Buzzer
    {
    private:
        int port = -1;
        int note = 0;
        HardwareLayer *hardwareLayer;

        void applyHardware()
        {
            hardwareLayer->setBuzzerTo(port, note);
        }

    public:
        Buzzer(){};
        Buzzer(int p, HardwareLayer *hl)
        {
            port = p;
            hardwareLayer = hl;
            // pinMode(RELAY_PINS[port], OUTPUT);
        };
        bool enabled()
        {
            return port > -1;
        }
        int status()
        {
            return note;
        }
        void on()
        {
            note = NOTE_G7;
            applyHardware();
        }
        void off()
        {
            note = 0;
            applyHardware();
        }
        void adjust(int n)
        {
            note = n;
            applyHardware();
        }
        bool test()
        {
            for (int i = 1; i <= 10; i++)
            {
                // TODO - make a melody
                // adjust(10 * i);
                // vTaskDelay(0.2 * 1000 / portTICK_PERIOD_MS);
                // TODO read feedback and check
            }

            off();

            return false;
        }
    };

    class Switches
    {
    public:
        Switch list[4];
    };

    class Dimmers
    {
    public:
        Dimmer list[4];
    };

    class ColorLights
    {
    public:
        ColorLight list[3];
    };

    class Fans
    {
    public:
        Fan list[2];
    };

    class Buzzers
    {
    public:
        Buzzer list[1];
    };

    class Controller
    {
    private:
        Switches switches = Switches();
        Dimmers dimmers = Dimmers();
        ColorLights colorLights = ColorLights();
        Fans fans = Fans();
        Buzzers buzzers = Buzzers();
        HardwareLayer *hardwareLayer;

    public:
        Controller(HardwareLayer *hl)
        {
            hardwareLayer = hl;
        };
        void begin()
        {
            hardwareLayer->begin();
            switches.list[0] = Switch(0, hardwareLayer);
            switches.list[1] = Switch(1, hardwareLayer);
            switches.list[2] = Switch(2, hardwareLayer);
            switches.list[3] = Switch(3, hardwareLayer);

            dimmers.list[0] = Dimmer(0, hardwareLayer);
            dimmers.list[1] = Dimmer(1, hardwareLayer);
            dimmers.list[2] = Dimmer(2, hardwareLayer);
            dimmers.list[3] = Dimmer(3, hardwareLayer);

            fans.list[0] = Fan(0, hardwareLayer);
            fans.list[1] = Fan(1, hardwareLayer);

            buzzers.list[0] = Buzzer(1, hardwareLayer);

            hardwareLayer->safetyRelayOn();

            // Only compatible with controller v0.11-rc1 and higher
            colorLights.list[0] = ColorLight(11, 30);
        }
        void test()
        {
            // run tests in parallel "threads"
            for (int i = 0; i < 8; i++)
            {
                hardwareLayer->setExpanderPin(i, false);
                vTaskDelay(0.2 * 1000 / portTICK_PERIOD_MS);
                hardwareLayer->setExpanderPin(i, true);
                vTaskDelay(0.2 * 1000 / portTICK_PERIOD_MS);
                hardwareLayer->setExpanderPin(i, false);
                vTaskDelay(0.2 * 1000 / portTICK_PERIOD_MS);
            }

            switches.list[0].test();
            switches.list[1].test();
            switches.list[2].test();
            switches.list[3].test();

            // test safety switch
            hardwareLayer->safetyRelayOff();
            vTaskDelay(0.2 * 1000 / portTICK_PERIOD_MS);
            hardwareLayer->safetyRelayOn();
            vTaskDelay(0.2 * 1000 / portTICK_PERIOD_MS);
            hardwareLayer->safetyRelayOff();

            fans.list[0].test();
            fans.list[1].test();
            buzzers.list[0].test();
        }
        void beginLightDome()
        {
            colorLights.list[0] = ColorLight(32, 20);
            colorLights.list[1] = ColorLight(25, 20);
            colorLights.list[2] = ColorLight(33, 20);
        }

        void beginNano()
        {
            hardwareLayer->begin();
            switches.list[0] = Switch(0, hardwareLayer);
        }

        void resetPorts()
        {
            switches.list[0].off();
            switches.list[1].off();
            switches.list[2].off();
            switches.list[3].off();

            dimmers.list[0].off();
            dimmers.list[1].off();
            dimmers.list[2].off();

            fans.list[0].off();
            fans.list[1].off();
        }
        bool turnSwitchOn(int port)
        {
            if (port < 0 || port > 3)
            {
                return false;
            }
            switches.list[port].on();
            return true;
        };
        bool turnSwitchOff(int port)
        {
            if (port < 0 || port > 3)
            {
                return false;
            }
            switches.list[port].off();
            return true;
        };
        bool setDimmer(int port, int percent)
        {
            if (port < 0 || port > 3)
            {
                return false;
            }
            dimmers.list[port].setBrigntness(percent);
            return true;
        };
        bool setFan(int port, int percent)
        {
            if (port < 0 || port > 1)
            {
                return false;
            }
            int power = 255.0 * (((float)percent) / 100.0);
            if (power > 255)
            {
                power = 255;
            }
            if (power < 0)
            {
                power = 0;
            }
            fans.list[port].adjust(power);
            return true;
        };
        bool setBuzzer(int port, int note)
        {
            if (port != 0)
            {
                return false;
            }

            buzzers.list[port].adjust(note);
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

            Color black = Color(0, 0, 0);
            colorLights.list[0].setColorAndBrightness(black, 20);
            colorLights.list[1].setColorAndBrightness(black, 20);
            colorLights.list[2].setColorAndBrightness(black, 20);

            Color color = Color(84, 22, 180); // make purple

            colorLights.list[0].setPixel(0, color);
            colorLights.list[1].setPixel(0, color);
            colorLights.list[2].setPixel(0, color);
            colorLights.list[0].show();
            colorLights.list[1].show();
            colorLights.list[2].show();
            delay(25);

            for (int i = 1; i < 20; i++)
            {
                colorLights.list[0].setPixel(i, color);
                colorLights.list[1].setPixel(i, color);
                colorLights.list[2].setPixel(i, color);
                colorLights.list[0].setPixel(i - 1, black);
                colorLights.list[1].setPixel(i - 1, black);
                colorLights.list[2].setPixel(i - 1, black);
                colorLights.list[0].show();
                colorLights.list[1].show();
                colorLights.list[2].show();
                delay(25);
            }
            for (int i = 18; i >= 0; i--)
            {
                colorLights.list[0].setPixel(i, color);
                colorLights.list[1].setPixel(i, color);
                colorLights.list[2].setPixel(i, color);
                colorLights.list[0].setPixel(i + 1, black);
                colorLights.list[1].setPixel(i + 1, black);
                colorLights.list[2].setPixel(i + 1, black);
                colorLights.list[0].show();
                colorLights.list[1].show();
                colorLights.list[2].show();
                delay(25);
            }
            colorLights.list[0].setPixel(0, black);
            colorLights.list[1].setPixel(0, black);
            colorLights.list[2].setPixel(0, black);
            colorLights.list[0].show();
            colorLights.list[1].show();
            colorLights.list[2].show();
            delay(25);
            colorLights.list[0].setColorAndBrightness(black, 20);
            colorLights.list[1].setColorAndBrightness(black, 20);
            colorLights.list[2].setColorAndBrightness(black, 20);
            delay(1000);

            for (int i = 0; i < kelvinValuesCount; i++)
            {
                colorLights.list[0].setColorAndBrightness(Color(kelvinValues[i]), 100);
                colorLights.list[1].setColorAndBrightness(Color(kelvinValues[i]), 100);
                colorLights.list[2].setColorAndBrightness(Color(kelvinValues[i]), 100);
                colorLights.list[0].show();
                colorLights.list[1].show();
                colorLights.list[2].show();
                delay(25);
            }

            // colorLights.list[0].setColorAndBrightness(Color(1000), 100);
            // colorLights.list[1].setColorAndBrightness(Color(1000), 100);
            // colorLights.list[2].setColorAndBrightness(Color(1000), 100);

            colorLights.list[0].setColorAndBrightness(black, 20);
            colorLights.list[1].setColorAndBrightness(black, 20);
            colorLights.list[2].setColorAndBrightness(black, 20);
            delay(5000);

            return true;
        };
    };
}

#endif