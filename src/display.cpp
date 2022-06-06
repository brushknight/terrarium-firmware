#include "display.h"

namespace Display
{

    byte Cold[] = {
        B00000,
        B00100,
        B10101,
        B01110,
        B01110,
        B10101,
        B00100,
        B00000};

    byte Hot[] = {
        B00000,
        B10010,
        B01001,
        B01001,
        B10010,
        B11011,
        B11111,
        B00000};

    byte ArrowUp[] = {
        B00000,
        B00100,
        B01110,
        B11111,
        B00100,
        B00100,
        B00100,
        B00000};

    byte ArrowDown[] = {
        B00000,
        B00100,
        B00100,
        B00100,
        B11111,
        B01110,
        B00100,
        B00000};

    byte Temperature[] = {
        B10011,
        B10011,
        B11000,
        B10000,
        B10000,
        B10000,
        B10001,
        B01110};

    byte Humidity[] = {
        B11001,
        B11010,
        B00010,
        B00100,
        B00100,
        B01000,
        B01011,
        B10011};

    byte WiFi[] = {
        B00000,
        B00000,
        B11100,
        B00010,
        B11001,
        B00101,
        B10101,
        B00000};

    byte Cross[] = {
        B00000,
        B10001,
        B01010,
        B00100,
        B00100,
        B01010,
        B10001,
        B00000};

    byte Bluetooth[] = {
        B01100,
        B01010,
        B11001,
        B01110,
        B01110,
        B11001,
        B01010,
        B01100};

    int lcdColumns = 20;
    int lcdRows = 4;

    LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

    void setup()
    {
        // initialize LCD
        lcd.init();
        // turn on LCD backlight
        lcd.backlight();
        lcd.createChar(0, Cold);
        lcd.createChar(1, Hot);
        lcd.createChar(2, ArrowUp);
        lcd.createChar(3, ArrowDown);
        lcd.createChar(4, Temperature);
        lcd.createChar(5, Humidity);
        lcd.createChar(6, WiFi);
        lcd.createChar(7, Cross);
        // out of range
        //lcd.createChar(7, Bluetooth);
        Serial.println("display setup: ok");
    }

    void render(Data data)
    {
        lcd.clear();

        if (data.initialSetup.isInSetupMode)
        {
            renderInitialSetup(data.initialSetup);
        }
        else
        {
            lcd.setCursor(0, 1);
            lcd.write(4);

            lcd.setCursor(0, 2);
            lcd.write(5);

            int enabledClimateZones = 0;

            for (int i = 0; i < MAX_CLIMATE_ZONES; i++)
            {
                if (data.climateZones[i].isSet)
                {
                    enabledClimateZones++;
                }
            }

            for (int i = 0; i < MAX_CLIMATE_ZONES; i++)
            {
                if (data.climateZones[i].isSet)
                {
                    if (enabledClimateZones == 2)
                    {
                        renderClimateZone(data.climateZones[i], 12 * i + 2);
                    }
                    else
                    {
                        renderClimateZone(data.climateZones[i], 6 * i + 2);
                    }
                }
            }

            lcd.setCursor(19, 0);
            if (data.WiFiStatus)
            {
                lcd.write(6);
            }
            else
            {
                lcd.write(7);
            }

            // lcd.setCursor(19, 0);
            // lcd.write(6);

            // lcd.setCursor(19, 2);
            // lcd.write(8);

            lcd.setCursor(0, 3);
            lcd.printf("ID:%s", data.metadata.id.c_str());

            lcd.setCursor(15, 3);
            lcd.print(Utils::hourMinuteToString(RealTime::getHour(), RealTime::getMinute()).c_str());
        }

        //Serial.println("display render");
    }

    void renderInitialSetup(InitialSetup data)
    {
        lcd.setCursor(0, 0);
        lcd.print("Hi - connect to wifi");
        //lcd.write(6);
        lcd.setCursor(0, 1);
        lcd.print(data.apName.c_str());
        lcd.setCursor(0, 2);
        lcd.print("And visit");
        lcd.setCursor(0, 3);
        lcd.print(data.ipAddr.c_str());
    }

    void renderClimateZone(DataClimateZone data, int offset)
    {
        lcd.setCursor(offset, 0);
        if (data.heaterStatus)
        {
            lcd.write(1);
        }
        else
        {
            lcd.write(0);
        }
        lcd.setCursor(offset + 3, 0);
        if (data.heatingPhase)
        {
            lcd.write(2);
        }
        else
        {
            lcd.write(3);
        }
        lcd.setCursor(offset, 1);
        lcd.print(floatToString(data.temperature));

        lcd.setCursor(offset, 2);
        lcd.print(floatToString(data.humidity));
    }

    char *floatToString(double value)
    {
        static char buffer[5];
        sprintf(buffer, "%.1f", value);
        //Serial.println(buffer);
        return buffer;
    }

}