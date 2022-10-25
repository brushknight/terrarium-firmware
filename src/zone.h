#ifndef TERRARIUM_ZONE
#define TERRARIUM_ZONE

#include "Arduino.h"
#include "utils.h"
#include "event.h"
#include "control.h"
#include "measure.h"
#include <string>
#include "ArduinoJson.h"

namespace Zone
{
    const int maxTemperatureZonesCount = 3;
    const int maxTemperatureZonesSensorsCount = 3;
    const int maxTemperatureZonesEventsCount = 10;

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
        static TemperatureZoneStatus fromJSON(std::string json)
        {
            DynamicJsonDocument doc(jsonSize());
            deserializeJson(doc, json);

            return TemperatureZoneStatus::fromJSONObj(doc);
        }

        static TemperatureZoneStatus fromJSONObj(DynamicJsonDocument doc)
        {
            TemperatureZoneStatus temperatureZoneStatus;

            temperatureZoneStatus.averageTemperature = doc["averageTemperature"];
            temperatureZoneStatus.targetTemperature = doc["targetTemperature"];
            temperatureZoneStatus.heaterStatus = doc["heaterStatus"];

            return temperatureZoneStatus;
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
            doc["enabled"] = enabled;
            for (int i = 0; i < maxTemperatureZonesSensorsCount; i++)
            {
                doc["sensorIDs"][i] = sensorIDs[i].toJSON();
            }
            for (int i = 0; i < maxTemperatureZonesEventsCount; i++)
            {
                doc["events"][i] = events[i].toJSON();
            }
            doc["status"] = status.toJSON();
            doc["heaterPort"] = heaterPort;
            return doc;
        }

        static TemperatureZone fromJSON(std::string json)
        {
            DynamicJsonDocument doc(jsonSize());
            deserializeJson(doc, json);

            return TemperatureZone::fromJSONObj(doc);
        }

        static TemperatureZone fromJSONObj(DynamicJsonDocument doc)
        {
            TemperatureZone temperatureZone;
            temperatureZone.slug = doc["slug"].as<std::string>();
            temperatureZone.status = TemperatureZoneStatus::fromJSONObj(doc["status"]);
            temperatureZone.heaterPort = doc["heaterPort"];
            temperatureZone.enabled = doc["enabled"];
            for (int i = 0; i < maxTemperatureZonesSensorsCount; i++)
            {
                temperatureZone.sensorIDs[i] = Measure::SensorID::fromJSONObj(doc["sensorIDs"][i]);
            }
            for (int i = 0; i < maxTemperatureZonesEventsCount; i++)
            {
                temperatureZone.events[i] = Event::TemperatureEvent::fromJSONObj(doc["events"][i]);
            }

            return temperatureZone;
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

            Serial.printf("Average temperature %.2f, target  %.2f\n", status.averageTemperature, status.targetTemperature);

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
        Controller() {}
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

        static Controller fromJSON(std::string json)
        {
            DynamicJsonDocument doc(jsonSize());
            deserializeJson(doc, json);

            return Controller::fromJSONObj(doc);
        }

        static Controller fromJSONObj(DynamicJsonDocument doc)
        {
            Controller controller;

            for (int i = 0; i < maxTemperatureZonesCount; i++)
            {
                controller.temperatureZones[i] = TemperatureZone::fromJSONObj(doc["temperatureZones"][i]);
            }

            return controller;
        }

        // save to eeprom
        // load from eeprom

        // report of all zones, how to do? (to display and for api)
    };
}

#endif