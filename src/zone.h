#ifndef TERRARIUM_ZONE
#define TERRARIUM_ZONE

#include "Arduino.h"
#include "utils.h"
#include "event.h"
#include "control.h"
#include "measure.h"
#include <string>
#include "ArduinoJson.h"
#include "data_structures.h"

namespace Zone
{
    const int maxTemperatureZonesCount = 3;
    const int maxTemperatureZonesSensorsCount = 3;
    const int maxTemperatureZonesEventsCount = 5;
    const int maxDimmerZonesCount = 3;
    const int maxDimmerZonesEventsCount = 5;
    const int maxColorLightZonesCount = 1;
    const int maxColorLightZonesEventsCount = 5;

    class TemperatureZoneStatus
    {
    public:
        bool isTargetSet = false;
        bool isActorSet = false;
        bool isCurrentTempSet = false;
        float averageTemperature = 0;
        float targetTemperature = 0;
        float temperatureError = 0;
        std::string slug = "";
        boolean heaterStatus = false;
        int timestamp = 0;
        std::string error = "";
        static int jsonSize()
        {
            return 196;
        }
        void addError(std::string e)
        {
            error = e;
        }
        void setCurrentTemp(float temp)
        {
            averageTemperature = temp;
            isCurrentTempSet = true;
        }
        void setTarget(float target)
        {
            targetTemperature = target;
            isTargetSet = true;
        }
        void setHeater(bool heater)
        {
            heaterStatus = heater;
            isActorSet = true;
        }
        boolean isSet()
        {
            return slug != "";
        }
        DynamicJsonDocument toJSON()
        {
            DynamicJsonDocument doc(jsonSize());
            if (isCurrentTempSet)
            {
                doc["average_temperature"] = averageTemperature;
            }
            if (isTargetSet)
            {
                doc["target_temperature"] = targetTemperature;
            }
            if (isActorSet)
            {
                doc["heater_status"] = heaterStatus;
            }
            doc["temperature_error"] = temperatureError;
            if (error != "")
            {
                doc["error"] = error;
            }
            doc["timestamp"] = timestamp;
            // doc["slug"] = slug;
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

            temperatureZoneStatus.averageTemperature = doc["average_temperature"];
            temperatureZoneStatus.targetTemperature = doc["target_temperature"];
            temperatureZoneStatus.heaterStatus = doc["heater_status"];
            // temperatureZoneStatus.slug = doc["slug"].as<std::string>();

            return temperatureZoneStatus;
        }
    };

    class DimmerZoneStatus
    {
    public:
        std::string slug = "";
        int brightness = 0;
        static int jsonSize()
        {
            return 96;
        }
        boolean isSet()
        {
            return slug != "";
        }
        DynamicJsonDocument toJSON()
        {
            DynamicJsonDocument doc(jsonSize());
            doc["brightness"] = brightness;
            // doc["slug"] = slug;
            return doc;
        }
        static DimmerZoneStatus fromJSON(std::string json)
        {
            DynamicJsonDocument doc(jsonSize());
            deserializeJson(doc, json);

            return DimmerZoneStatus::fromJSONObj(doc);
        }

        static DimmerZoneStatus fromJSONObj(DynamicJsonDocument doc)
        {
            DimmerZoneStatus zoneStatus;

            zoneStatus.brightness = doc["brightness"];
            // temperatureZoneStatus.slug = doc["slug"].as<std::string>();

            return zoneStatus;
        }
    };

    class ColorLightZoneStatus
    {
    public:
        std::string slug = "";
        Color color;
        int brightness = 0;
        static int jsonSize()
        {
            return 96 + Color::jsonSize();
        }
        boolean isSet()
        {
            return slug != "";
        }
        DynamicJsonDocument toJSON()
        {
            DynamicJsonDocument doc(jsonSize());
            doc["color"] = color.toJSON();
            doc["brightness"] = brightness;
            // doc["slug"] = slug;
            return doc;
        }
        static ColorLightZoneStatus fromJSON(std::string json)
        {
            DynamicJsonDocument doc(jsonSize());
            deserializeJson(doc, json);

            return ColorLightZoneStatus::fromJSONObj(doc);
        }

        static ColorLightZoneStatus fromJSONObj(DynamicJsonDocument doc)
        {
            ColorLightZoneStatus zoneStatus;

            zoneStatus.color = Color::fromJSONObj(doc["color"]);
            zoneStatus.brightness = doc["brightness"];
            return zoneStatus;
        }
    };

    class ZonesStatuses
    {
    public:
        TemperatureZoneStatus temperatureZones[maxTemperatureZonesCount];
        DimmerZoneStatus dimmerZones[maxDimmerZonesCount];
        ColorLightZoneStatus colorLightZones[maxColorLightZonesCount];
        static int jsonSize()
        {
            return 64 + TemperatureZoneStatus::jsonSize() * maxTemperatureZonesCount + DimmerZoneStatus::jsonSize() * maxDimmerZonesCount;
        }
        DynamicJsonDocument toJSON()
        {
            DynamicJsonDocument doc(jsonSize());
            for (int i = 0; i < maxTemperatureZonesCount; i++)
            {
                if (temperatureZones[i].isSet())
                {
                    doc["temperature_zones"][temperatureZones[i].slug] = temperatureZones[i].toJSON();
                }
            }
            for (int i = 0; i < maxDimmerZonesCount; i++)
            {
                if (dimmerZones[i].isSet())
                {
                    doc["dimmer_zones"][dimmerZones[i].slug] = dimmerZones[i].toJSON();
                }
            }
            for (int i = 0; i < maxColorLightZonesCount; i++)
            {
                if (colorLightZones[i].isSet())
                {
                    doc["color_light_zones"][colorLightZones[i].slug] = colorLightZones[i].toJSON();
                }
            }
            return doc;
        }
    };

    class TemperatureZone
    {
    private:
        bool enabled = false;

    public:
        std::string slug = "";
        Measure::SensorID sensorIDs[maxTemperatureZonesSensorsCount];
        Event::TemperatureEvent events[maxTemperatureZonesEventsCount];
        int heaterPort = -1;
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
                doc["sensor_ids"][i] = sensorIDs[i].toJSON();
            }
            for (int i = 0; i < maxTemperatureZonesEventsCount; i++)
            {
                doc["events"][i] = events[i].toJSON();
            }
            // doc["status"] = status.toJSON();
            doc["heater_port"] = heaterPort;
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
            // temperatureZone.status = TemperatureZoneStatus::fromJSONObj(doc["status"]); // should not be saved
            //  temperatureZone.status = TemperatureZoneStatus();
            temperatureZone.heaterPort = doc["heater_port"];
            temperatureZone.enabled = doc["enabled"];
            for (int i = 0; i < maxTemperatureZonesSensorsCount; i++)
            {
                temperatureZone.sensorIDs[i] = Measure::SensorID::fromJSONObj(doc["sensor_ids"][i]);
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

        TemperatureZoneStatus loopTick(Time now, Measure::EnvironmentSensors *sharedSensors, Control::Controller *controller)
        {
            // TODO return status
            //  find active event (latest in the window)

            status = TemperatureZoneStatus();

            Event::TemperatureEvent activeEvent;

            for (int i = 0; i < maxTemperatureZonesEventsCount; i++)
            {
                if (events[i].isActive(now))
                {
                    activeEvent = events[i];
                    Serial.printf("active event id: %d\n", i);
                }
            }

            TemperatureMeasurments tm;

            for (int i = 0; i < maxTemperatureZonesSensorsCount; i++)
            {
                if (sensorIDs[i].isSet())
                {
                    Serial.printf("sensor %d status %d, temp %.2f\n", i, (*sharedSensors).get(sensorIDs[i]).enabled(), (*sharedSensors).get(sensorIDs[i]).temperature());

                    float t = (*sharedSensors).get(sensorIDs[i]).temperature();

                    if (t < 0)
                    {
                        status.addError("ERROR: temperature is subzero");
                    }

                    if (t > 0)
                    {
                        tm.add(t);
                    }
                }
            }

            status.temperatureError = tm.calcError();
            float maxTempError = 1.0;
            if (status.temperatureError > maxTempError)
            {

                static char errTempDiff[100];
                sprintf(errTempDiff, "ERROR: temperature difference across sensors (%0.2f) is higher than it should be (0.2%f)", status.temperatureError, maxTempError);

                status.addError(std::string(errTempDiff));
                Serial.println(errTempDiff);
            }

            status.slug = slug;

            if (tm.count() == 0)
            {
                status.addError("ERROR: no temperature sensros found");
                Serial.println("ERROR: no temperature sensros found");
                (*controller).turnSwitchOff(heaterPort);
                status.setHeater(false);
                return status;
            }

            status.setCurrentTemp(tm.calcAverage());
            status.timestamp = Utils::getTimestamp();

            if (!activeEvent.isSet())
            {
                status.addError("ERROR: no active event found");
                Serial.println("ERROR: no active event found");
                return status;
            }

            if (activeEvent.temperature > -1)
            {
                status.setTarget(activeEvent.temperature);
            }

            if (activeEvent.transform.isSet())
            {
                status.setTarget(activeEvent.transformedValue(now));
            }

            Serial.printf("Average temperature %.2f, target  %.2f\n", status.averageTemperature, status.targetTemperature);

            if (heaterPort > -1)
            {
                if (status.averageTemperature < status.targetTemperature)
                {
                    (*controller).turnSwitchOn(heaterPort);
                    status.setHeater(true);
                }
                else
                {
                    (*controller).turnSwitchOff(heaterPort);
                    status.setHeater(false);
                }
            }

            return status;
        }
    };

    class DimmerZone
    {
    private:
        bool enabled = false;

    public:
        std::string slug = "";
        Event::LightEvent events[maxDimmerZonesEventsCount];
        int dimmerPort = -1;
        DimmerZoneStatus status;

        DimmerZone()
        {
            // stub for empty array creation
        }

        DimmerZone(std::string s)
        {
            enabled = true;
            slug = s;
        }

        static int jsonSize()
        {
            return 128 + DimmerZoneStatus::jsonSize() + Event::LightEvent::jsonSize() * maxDimmerZonesEventsCount;
        }

        DynamicJsonDocument toJSON()
        {
            DynamicJsonDocument doc(jsonSize());
            doc["slug"] = slug;
            doc["enabled"] = enabled;
            for (int i = 0; i < maxDimmerZonesEventsCount; i++)
            {
                doc["events"][i] = events[i].toJSON();
            }
            doc["dimmer_port"] = dimmerPort;
            return doc;
        }

        static DimmerZone fromJSON(std::string json)
        {
            DynamicJsonDocument doc(jsonSize());
            deserializeJson(doc, json);

            return DimmerZone::fromJSONObj(doc);
        }

        static DimmerZone fromJSONObj(DynamicJsonDocument doc)
        {
            DimmerZone zone;
            zone.slug = doc["slug"].as<std::string>();
            zone.dimmerPort = doc["dimmer_port"];
            zone.enabled = doc["enabled"];
            for (int i = 0; i < maxDimmerZonesEventsCount; i++)
            {
                zone.events[i] = Event::LightEvent::fromJSONObj(doc["events"][i]);
            }

            return zone;
        }

        void reset()
        {
            enabled = false;
        }

        bool isEnabled()
        {
            return enabled;
        }

        DimmerZoneStatus loopTick(Time now, Measure::EnvironmentSensors *sharedSensors, Control::Controller *controller)
        {
            status = DimmerZoneStatus();

            Event::LightEvent activeEvent;

            for (int i = 0; i < maxDimmerZonesEventsCount; i++)
            {
                if (events[i].isActive(now))
                {
                    activeEvent = events[i];
                    Serial.printf("active event id: %d\n", i);
                }
            }

            (*controller).setDimmer(dimmerPort, activeEvent.brightness);
            status.brightness = activeEvent.brightness;

            status.slug = slug;

            return status;
        }
    };

    class ColorLightZone
    {
    private:
        bool enabled = false;

    public:
        std::string slug = "";
        Event::LightEvent events[maxDimmerZonesEventsCount];
        int ledPort = -1;
        ColorLightZoneStatus status;

        ColorLightZone()
        {
            // stub for empty array creation
        }

        ColorLightZone(std::string s)
        {
            enabled = true;
            slug = s;
        }

        static int jsonSize()
        {
            return 128 + ColorLightZoneStatus::jsonSize() + Event::LightEvent::jsonSize() * maxDimmerZonesEventsCount;
        }

        DynamicJsonDocument toJSON()
        {
            DynamicJsonDocument doc(jsonSize());
            doc["slug"] = slug;
            doc["enabled"] = enabled;
            for (int i = 0; i < maxDimmerZonesEventsCount; i++)
            {
                doc["events"][i] = events[i].toJSON();
            }
            doc["led_port"] = ledPort;
            return doc;
        }

        static ColorLightZone fromJSON(std::string json)
        {
            DynamicJsonDocument doc(jsonSize());
            deserializeJson(doc, json);

            return ColorLightZone::fromJSONObj(doc);
        }

        static ColorLightZone fromJSONObj(DynamicJsonDocument doc)
        {
            ColorLightZone zone;
            zone.slug = doc["slug"].as<std::string>();
            zone.ledPort = doc["led_port"];
            zone.enabled = doc["enabled"];
            for (int i = 0; i < maxColorLightZonesEventsCount; i++)
            {
                zone.events[i] = Event::LightEvent::fromJSONObj(doc["events"][i]);
            }

            return zone;
        }

        void reset()
        {
            enabled = false;
        }

        bool isEnabled()
        {
            return enabled;
        }

        ColorLightZoneStatus loopTick(Time now, Measure::EnvironmentSensors *sharedSensors, Control::Controller *controller)
        {
            
            status = ColorLightZoneStatus();

            Event::LightEvent activeEvent;

            for (int i = 0; i < maxColorLightZonesEventsCount; i++)
            {
                if (events[i].isActive(now))
                {
                    activeEvent = events[i];
                    Serial.printf("color light zone active event id: %d\n", i);
                }
            }

            int brightness = activeEvent.brightness;
            if (activeEvent.transform.isSet())
            {
                brightness = activeEvent.transformedValue(now);
            }

            (*controller).setColorAndBrightness(ledPort, activeEvent.color, brightness);
            status.color = activeEvent.color;
            status.brightness = brightness;

            status.slug = slug;

            return status;
        }
    };

    class Controller
    {
    private:
        TemperatureZone temperatureZones[maxTemperatureZonesCount];
        DimmerZone dimmerZones[maxDimmerZonesCount];
        ColorLightZone colorLightZones[maxColorLightZonesCount];
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
        ZonesStatuses loopTick(Time now, Measure::EnvironmentSensors *sharedSensors, Control::Controller *controller)
        {
            ZonesStatuses statuses;
            // TODO return list of statuses
            if (!paused)
            {
                // loop over all zones
                for (int i = 0; i < maxTemperatureZonesCount; i++)
                {
                    if (temperatureZones[i].isEnabled())
                    {
                        statuses.temperatureZones[i] = temperatureZones[i].loopTick(now, sharedSensors, controller);
                    }
                }
                for (int i = 0; i < maxDimmerZonesCount; i++)
                {
                    if (dimmerZones[i].isEnabled())
                    {
                        statuses.dimmerZones[i] = dimmerZones[i].loopTick(now, sharedSensors, controller);
                    }
                }
                for (int i = 0; i < maxColorLightZonesCount; i++)
                {
                    if (colorLightZones[i].isEnabled())
                    {
                        statuses.colorLightZones[i] = colorLightZones[i].loopTick(now, sharedSensors, controller);
                    }
                }
            }
            return statuses;
        }
        TemperatureZone getTemperatureZone(int index)
        {
            return temperatureZones[index];
        }
        void addTemperatureZone(TemperatureZone tempZone)
        {
            pause();
            for (int i = 0; i < maxTemperatureZonesCount; i++)
            {
                if (!temperatureZones[i].isEnabled())
                {
                    temperatureZones[i] = tempZone;
                    break;
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
            // TODO test without this extra 4k
            return 1024 * 2 + 128 + TemperatureZone::jsonSize() * maxTemperatureZonesCount + DimmerZone::jsonSize() * maxDimmerZonesCount + ColorLightZone::jsonSize() * maxColorLightZonesCount;
        }

        // return as json
        DynamicJsonDocument toJSON()
        {
            DynamicJsonDocument doc(jsonSize());
            for (int i = 0; i < maxTemperatureZonesCount; i++)
            {
                doc["temperature_zones"][i] = temperatureZones[i].toJSON();
            }
            for (int i = 0; i < maxDimmerZonesCount; i++)
            {
                doc["dimmer_zones"][i] = dimmerZones[i].toJSON();
            }
            for (int i = 0; i < maxColorLightZonesCount; i++)
            {
                doc["color_light_zones"][i] = colorLightZones[i].toJSON();
            }
            return doc;
        }

        static Controller fromJSON(std::string json)
        {
            DynamicJsonDocument doc = DynamicJsonDocument(jsonSize());
            Serial.println(jsonSize());
            Serial.println(json.length());
            Serial.println(doc.capacity());
            deserializeJson(doc, json);

            Serial.println("slug");
            Serial.println(doc["temperature_zones"][0]["slug"].as<std::string>().c_str());

            return Controller::fromJSONObj(doc);
        }

        static Controller fromJSONObj(DynamicJsonDocument doc)
        {
            Controller controller;

            Serial.println(doc.capacity());
            Serial.println("slug");
            Serial.println(doc["temperature_zones"][0]["slug"].as<std::string>().c_str());

            for (int i = 0; i < maxTemperatureZonesCount; i++)
            {
                controller.temperatureZones[i] = TemperatureZone::fromJSONObj(doc["temperature_zones"][i]);
            }

            for (int i = 0; i < maxDimmerZonesCount; i++)
            {
                controller.dimmerZones[i] = DimmerZone::fromJSONObj(doc["dimmer_zones"][i]);
            }

            for (int i = 0; i < maxColorLightZonesCount; i++)
            {
                controller.colorLightZones[i] = ColorLightZone::fromJSONObj(doc["color_light_zones"][i]);
            }

            Serial.println("Controller fromJSONObj()");
            Serial.println(controller.temperatureZones[0].slug.c_str());

            return controller;
        }

        // save to eeprom
        // load from eeprom

        // report of all zones, how to do? (to display and for api)
    };
}

#endif