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
        int sensorIDs[3]; // to be clarified sensorsID / type + port
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

        void loopTick(std::string now, Measure::EnvironmentSensors sharedSensors, Control::Controller *controller)
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
            float averageTemperature = 0;

            // compare temperature to required
            // adjust corresponding relay via controller

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