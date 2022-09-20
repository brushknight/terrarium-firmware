#ifndef TERRARIUM_ZONE
#define TERRARIUM_ZONE

#include "Arduino.h"
#include "utils.h"
#include "event.h"
#include "control.h"
#include "measure.h"
#include "real_time.h"

namespace Zone
{

    class TemperatureZone
    {
    private:
        bool enabled = false;

    public:
        std::string slug;
        Measure::SensorID sensorIDs[3];
        Event::TemperatureEvent events[10];
        int heaterPort;

        TemperatureZone(std::string s)
        {
            enabled = true;
            slug = s;
        }

        bool isEnabled()
        {
            return enabled;
        }

        void loopTick(std::string now, Measure::EnvironmentSensors *sharedSensors, Control::Controller *controller)
        {
            // find active event (latest in the window)

            Event::TemperatureEvent activeEvent;

            for (int i = 0; i < 10; i++)
            {
                if (events[i].isActive(now))
                {
                    activeEvent = events[i];
                    Serial.printf("active event id: %d\n", i);
                }
            }

            // check sensors, get average

            float tempSum = 0;
            float sensorsCount = 0;

            for (int i = 0; i < 3; i++)
            {
                if (sensorIDs[i].isSet())
                {
                    Serial.printf("sensor status %d, temp %.2f\n", (*sharedSensors).get(sensorIDs[i]).enabled(), (*sharedSensors).get(sensorIDs[i]).temperature());

                    tempSum += (*sharedSensors).get(sensorIDs[i]).temperature();
                    sensorsCount++;
                }
            }

            float averageTemperature = sensorsCount > 0 ? tempSum / sensorsCount : -100;
            // throw error

            // compare temperature to required
            // adjust corresponding relay via controller

            Serial.printf("Average temperature %.2f\n", averageTemperature);

            if (averageTemperature < 0)
            {
                Serial.println("ERROR: temperature cant be subzero");
                return;
            }

            if (averageTemperature < activeEvent.temperature)
            {
                (*controller).turnSwitchOn(heaterPort);
            }
            else
            {
                (*controller).turnSwitchOff(heaterPort);
            }
        }
    };

}

#endif