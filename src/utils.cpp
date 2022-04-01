#include "utils.h"

namespace Utils
{
    void log(const char str[])
    {
        Serial.print(str);
    }

    void TCA9548A(uint8_t bus)
    {
        Wire.beginTransmission(0x70); // TCA9548A address is 0x70
        Wire.write(1 << bus);         // send byte to select bus
        Wire.endTransmission();
        Serial.print(bus);
    }

    void scanForI2C()
    {
        Serial.println();
        Serial.println("I2C scanner. Scanning ...");
        byte count = 0;

        Wire.begin();
        for (byte i = 1; i < 120; i++)
        {
            Wire.beginTransmission(i);
            if (Wire.endTransmission() == 0)
            {
                Serial.print("Found address: ");
                Serial.print(i, DEC);
                Serial.print(" (0x");
                Serial.print(i, HEX);
                Serial.println(")");
                count++;
                delay(1); // maybe unneeded?
            }             // end of good response
        }                 // end of for loop
        Serial.println("Done.");
        Serial.print("Found ");
        Serial.print(count, DEC);
        Serial.println(" device(s).");
    }

}