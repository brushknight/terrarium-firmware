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
    const int maxTemperatureZonesCount = 3;
    const int maxTemperatureZonesSensorsCount = 3;
    const int maxTemperatureZonesEventsCount = 10;

    class Controller
    {
    private:
        TemperatureZone temperatureZones[maxTemperatureZonesCount];
        bool paused = false;

        void pause()
        {
            paused = true;
        }
        void resume()
        {
            paused = false;
        }

    public:
        void loopTick(std::string now, Measure::EnvironmentSensors *sharedSensors, Control::Controller *controller)
        {
            if (!paused)
            {
                // loop over all zones
                for (int i = 0; i < maxTemperatureZonesCount; i++)
                {
                    if (temperatureZones[i].isEnabled())
                    {
                        temperatureZones[i].loopTick(now, sharedSensors, controller);
                    }
                }
            }
        }
        void addTemperatureZone(TemperatureZone tempZone)
        {
            pause();
            for (int i = 0; i < maxTemperatureZonesCount; i++)
            {
                if (!temperatureZones[i].isEnabled())
                {
                    temperatureZones[i] = tempZone;
                }
                // TODO return error if have already 3 zones
            }
            // changes
            resume();
        }
        void removeTemperatureZone(int id)
        {
            if (id >= 0 && id < maxTemperatureZonesCount)
            {
                pause();
                temperatureZones[id].reset();
                // changes
                resume();
            }
        }
        void updateTemperatureZone(int id, TemperatureZone tempZone)
        {
            if (id >= 0 && id < maxTemperatureZonesCount)
            {
                pause();
                temperatureZones[id] = tempZone;
                // changes
                resume();
            }
        }

        static int jsonSize()
        {
            return TemperatureZone::jsonSize() * maxTemperatureZonesCount;
        }

        // return as json
        DynamicJsonDocument toJSON()
        {
            DynamicJsonDocument doc(jsonSize());
            for (int i = 0; i < maxTemperatureZonesCount; i++)
            {
                doc["temperatureZones"][i] = temperatureZones[i].toJSON();
            }
            return doc;
        }

        // save to eeprom
        // load from eeprom

        // parse from json
        // report of all zones, how to do? (to display and for api)
    };

    class TemperatureZoneStatus
    {
    public:
        float averageTemperature;
        float targetTemperature;
        boolean heaterStatus;
        static int jsonSize()
        {
            return 64;
        }

        DynamicJsonDocument toJSON()
        {
            DynamicJsonDocument doc(jsonSize());
            doc["averageTemperature"] = averageTemperature;
            doc["targetTemperature"] = targetTemperature;
            doc["heaterStatus"] = heaterStatus;
            return doc;
        }
    };

    class TemperatureZone
    {
    private:
        bool enabled = false;

    public:
        std::string slug;
        Measure::SensorID sensorIDs[maxTemperatureZonesSensorsCount];
        Event::TemperatureEvent events[maxTemperatureZonesEventsCount];
        int heaterPort;
        TemperatureZoneStatus status;

        TemperatureZone()
        {
            // stub for empty array creation
        }

        TemperatureZone(std::string s)
        {
            enabled = true;
            slug = s;
        }

        static int jsonSize()
        {
            return 96 + TemperatureZoneStatus::jsonSize() + Event::TemperatureEvent::jsonSize() * maxTemperatureZonesEventsCount + Measure::SensorID::jsonSize() * maxTemperatureZonesSensorsCount;
        }

        DynamicJsonDocument toJSON()
        {
            DynamicJsonDocument doc(jsonSize());
            doc["slug"] = slug;
            for (int i = 0; i < maxTemperatureZonesSensorsCount; i++)
            {
                doc["sensorIDs"][i] = sensorIDs[i];
            }
            for (int i = 0; i < maxTemperatureZonesEventsCount; i++)
            {
                doc["events"][i] = events[i].toJSON();
            }
            doc["status"] = status.toJSON();
            doc["heaterPort"] = heaterPort;
            return doc;
        }

        void reset()
        {
            enabled = false;
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

            status.averageTemperature = sensorsCount > 0 ? tempSum / sensorsCount : -100;
            status.targetTemperature = activeEvent.temperature;
            // throw error

            // compare temperature to required
            // adjust corresponding relay via controller

            Serial.printf("Average temperature %.2f\n", status.averageTemperature);

            if (status.averageTemperature < 0)
            {
                Serial.println("ERROR: temperature cant be subzero");
                return;
            }

            if (status.averageTemperature < status.targetTemperature)
            {
                (*controller).turnSwitchOn(heaterPort);
                status.heaterStatus = true;
            }
            else
            {
                (*controller).turnSwitchOff(heaterPort);
                status.heaterStatus = false;
            }
        }
    };

}

#endif