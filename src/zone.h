#ifndef TERRARIUM_ZONE
#define TERRARIUM_ZONE

#include "Arduino.h"
#include "utils.h"
#include "event.h"
#include "actuator.h"
#include "measure.h"
#include <string>
#include "ArduinoJson.h"
#include "data_structures.h"
#include "constants.h"
#include "i2c.h"
#include "QuickPID.h"

namespace Zone
{

    static const char *TAG = "zone";

    const int maxTemperatureZonesCount = 3;
    const int maxTemperatureZonesSensorsCount = 3;
    const int maxTemperatureZonesEventsCount = 5;
    const int maxDimmerZonesCount = 3;
    const int maxDimmerZonesEventsCount = 5;
    const int maxColorLightZonesCount = 1;
    const int maxColorLightZonesEventsCount = 5;

    const int maxFanZonesCount = 2;
    const int maxFanZonesEventsCount = 5;

    const int actuatorTypeOnBoard = 0;
    const int actuatorTypeLightDome = 1;
    const int actuatorTypeDimmer = 2;

    class ActuatorConfig
    {
    public:
        int type = -1;      // on board, light_dome, dimmer, rain...
        uint16_t i2cID = 0; // i2c ID
        static int jsonSize()
        {
            return 32; // to be defined
        }
        DynamicJsonDocument toJSON()
        {
            DynamicJsonDocument doc(jsonSize());
            doc["type"] = type;
            doc["i2c_id"] = i2cID;
            return doc;
        }

        static ActuatorConfig fromJSON(std::string json)
        {
            DynamicJsonDocument doc(jsonSize());
            deserializeJson(doc, json);

            return ActuatorConfig::fromJSONObj(doc);
        }

        static ActuatorConfig fromJSONObj(DynamicJsonDocument doc)
        {
            ActuatorConfig actuator;
            actuator.type = doc["type"];
            actuator.i2cID = doc["i2c_id"];
            return actuator;
        }

        void updateFromJSONObj(DynamicJsonDocument doc)
        {
            type = doc["type"];
            i2cID = doc["i2c_id"];
        }
    };

    class ZoneStatus
    {
    public:
        std::string slug = "";
        std::string error = "";
        int timestamp = 0;
        void addError(std::string e)
        {
            error = e;
        }
        boolean isSet()
        {
            return slug != "";
        }
    };

    class TemperatureZoneStatus : public ZoneStatus
    {
    public:
        bool isTargetSet = false;
        bool isActorSet = false;
        bool isCurrentTempSet = false;
        float averageTemperature = 0;
        float targetTemperature = 0;
        float temperatureError = 0;
        boolean heaterStatus = false;

        static int jsonSize()
        {
            return 196;
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

    class DimmerZoneStatus : public ZoneStatus
    {
    public:
        int brightness = 0;
        static int jsonSize()
        {
            return 96;
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

    class ColorLightZoneStatus : public ZoneStatus
    {
    public:
        Color color;
        int brightness = 0;
        static int jsonSize()
        {
            return 96 + Color::jsonSize();
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

    class FanZoneStatus : public ZoneStatus
    {
    public:
        int power = 0;
        static int jsonSize()
        {
            return 96;
        }
        DynamicJsonDocument toJSON()
        {
            DynamicJsonDocument doc(jsonSize());
            doc["power"] = power;
            return doc;
        }
        static FanZoneStatus fromJSON(std::string json)
        {
            DynamicJsonDocument doc(jsonSize());
            deserializeJson(doc, json);

            return FanZoneStatus::fromJSONObj(doc);
        }

        static FanZoneStatus fromJSONObj(DynamicJsonDocument doc)
        {
            FanZoneStatus zoneStatus;

            zoneStatus.power = doc["power"];
            return zoneStatus;
        }
    };

    class ZonesStatuses
    {
    public:
        TemperatureZoneStatus temperatureZones[maxTemperatureZonesCount];
        DimmerZoneStatus dimmerZones[maxDimmerZonesCount];
        ColorLightZoneStatus colorLightZones[maxColorLightZonesCount];
        FanZoneStatus fanZones[maxFanZonesCount];
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
            for (int i = 0; i < maxFanZonesCount; i++)
            {
                if (fanZones[i].isSet())
                {
                    doc["fan_zones"][fanZones[i].slug] = fanZones[i].toJSON();
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

        // PID stuff
        // bool isPID = false;
        float PID_Target;
        float PID_Current;
        float PID_Decision;
        float PID_Kp = 2;
        float PID_Ki = 2;
        float PID_Kd = 2;
        QuickPID PID;

        TemperatureZone()
        {
            // stub for empty array creation
        }

        TemperatureZone(std::string s)
        {
            enabled = true;
            slug = s;
        }

        void begin()
        {
            // pid
            // isPID = true;
            PID = QuickPID(&PID_Current, &PID_Decision, &PID_Target);
            PID.SetTunings(PID_Kp, PID_Ki, PID_Kd);
            PID.SetOutputLimits(0, 1);
            PID.SetMode(PID.Control::automatic);
            ESP_LOGD(TAG, "PID settings Kd %.2f Kp %.2f Kd %.2f", PID.GetKp(), PID.GetKi(), PID.GetKd());
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
            // doc["is_pid"] = isPID;
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
            TemperatureZone temperatureZone = TemperatureZone();
            temperatureZone.slug = doc["slug"].as<std::string>();
            // temperatureZone.status = TemperatureZoneStatus::fromJSONObj(doc["status"]); // should not be saved
            //  temperatureZone.status = TemperatureZoneStatus();
            temperatureZone.heaterPort = doc["heater_port"];
            temperatureZone.enabled = doc["enabled"];
            // temperatureZone.isPID = doc["is_pid"];
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

        void updateFromJSONObj(DynamicJsonDocument doc)
        {
            slug = doc["slug"].as<std::string>();

            // temperatureZone.status = TemperatureZoneStatus::fromJSONObj(doc["status"]); // should not be saved
            //  temperatureZone.status = TemperatureZoneStatus();
            heaterPort = doc["heater_port"];
            enabled = doc["enabled"];
            // isPID = doc["is_pid"];

            for (int i = 0; i < maxTemperatureZonesSensorsCount; i++)
            {
                sensorIDs[i] = Measure::SensorID::fromJSONObj(doc["sensor_ids"][i]);
            }
            for (int i = 0; i < maxTemperatureZonesEventsCount; i++)
            {
                events[i] = Event::TemperatureEvent::fromJSONObj(doc["events"][i]);
            }

            ESP_LOGD(TAG, "slug: %s", slug.c_str());
            ESP_LOGD(TAG, "heater_port: %d", heaterPort);
            ESP_LOGD(TAG, "enabled: %d", enabled);
        }

        void reset()
        {
            enabled = false;
        }

        bool isEnabled()
        {
            return enabled;
        }

        TemperatureZoneStatus loopTick(Time now, Measure::EnvironmentSensors *sharedSensors, Actuator::Controller *controller)
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
                    ESP_LOGD(TAG, "active event id: %d", i);
                }
            }

            TemperatureMeasurments tm;

            for (int i = 0; i < maxTemperatureZonesSensorsCount; i++)
            {
                if (sensorIDs[i].isSet())
                {
                    ESP_LOGD(TAG, "sensor %d status %d, temp %.2f", i, (*sharedSensors).get(sensorIDs[i]).enabled(), (*sharedSensors).get(sensorIDs[i]).temperature());

                    float t = (*sharedSensors).get(sensorIDs[i]).temperature();

                    if (t < 0)
                    {
                        ESP_LOGE(TAG, "temperature is subzero, %.2f", t);
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
                sprintf(errTempDiff, "ERROR: temperature difference across sensors (%0.2f) is higher than it should be (%0.2f)", status.temperatureError, maxTempError);

                status.addError(std::string(errTempDiff));
                ESP_LOGW(TAG, "%s", errTempDiff);
            }

            status.slug = slug;

            if (tm.count() == 0)
            {
                status.addError("ERROR: no temperature sensros found");
                ESP_LOGE(TAG, "No temperature sensros found");
                (*controller).turnSwitchOff(heaterPort);
                status.setHeater(false);
                return status;
            }

            status.setCurrentTemp(tm.calcAverage());
            status.timestamp = Utils::getTimestamp();

            if (!activeEvent.isSet())
            {
                status.addError("ERROR: no active event found");
                ESP_LOGE(TAG, "No active event found");
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

            ESP_LOGD(TAG, "Average temperature %.2f, target  %.2f", status.averageTemperature, status.targetTemperature);

            if (heaterPort > -1)
            {

                if (activeEvent.isPID())
                {
                    PID_Current = status.averageTemperature;
                    PID_Target = status.targetTemperature;
                    PID.Compute();
                    ESP_LOGD(TAG, "PID settings Kd %.2f Kp %.2f Kd %.2f ", PID.GetKp(), PID.GetKi(), PID.GetKd());
                    ESP_LOGD(TAG, "PID output: %.2f", PID_Decision);
                    if (PID_Decision > 0)
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
                else
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

        void updateFromJSONObj(DynamicJsonDocument doc)
        {
            slug = doc["slug"].as<std::string>();
            dimmerPort = doc["dimmer_port"];
            enabled = doc["enabled"];
            for (int i = 0; i < maxDimmerZonesEventsCount; i++)
            {
                events[i] = Event::LightEvent::fromJSONObj(doc["events"][i]);
            }
        }

        void reset()
        {
            enabled = false;
        }

        bool isEnabled()
        {
            return enabled;
        }

        DimmerZoneStatus loopTick(Time now, Measure::EnvironmentSensors *sharedSensors, Actuator::Controller *controller)
        {
            status = DimmerZoneStatus();

            Event::LightEvent activeEvent;

            for (int i = 0; i < maxDimmerZonesEventsCount; i++)
            {
                if (events[i].isActive(now))
                {
                    activeEvent = events[i];
                    ESP_LOGD(TAG, "active event id: %d", i);
                }
            }

            if (!activeEvent.isSet())
            {
                status.addError("ERROR: no active event found");
                ESP_LOGE(TAG, "No active event found");
                return status;
            }

            (*controller).setDimmer(dimmerPort, activeEvent.brightness);
            status.brightness = activeEvent.brightness;

            status.slug = slug;

            return status;
        }
    };

    class FanZone
    {
    private:
        bool enabled = false;

    public:
        std::string slug = "";
        Event::FanEvent events[maxDimmerZonesEventsCount];
        int port = -1;
        FanZoneStatus status;

        FanZone()
        {
            // stub for empty array creation
        }

        FanZone(std::string s)
        {
            enabled = true;
            slug = s;
        }

        static int jsonSize()
        {
            return 128 + FanZoneStatus::jsonSize() + Event::FanEvent::jsonSize() * maxFanZonesEventsCount;
        }

        DynamicJsonDocument toJSON()
        {
            DynamicJsonDocument doc(jsonSize());
            doc["slug"] = slug;
            doc["enabled"] = enabled;
            for (int i = 0; i < maxFanZonesEventsCount; i++)
            {
                doc["events"][i] = events[i].toJSON();
            }
            doc["port"] = port;
            return doc;
        }

        static FanZone fromJSON(std::string json)
        {
            DynamicJsonDocument doc(jsonSize());
            deserializeJson(doc, json);

            return FanZone::fromJSONObj(doc);
        }

        static FanZone fromJSONObj(DynamicJsonDocument doc)
        {
            FanZone zone;
            zone.slug = doc["slug"].as<std::string>();
            zone.port = doc["port"];
            zone.enabled = doc["enabled"];
            for (int i = 0; i < maxFanZonesEventsCount; i++)
            {
                zone.events[i] = Event::FanEvent::fromJSONObj(doc["events"][i]);
            }

            return zone;
        }

        void updateFromJSONObj(DynamicJsonDocument doc)
        {
            slug = doc["slug"].as<std::string>();
            port = doc["port"];
            enabled = doc["enabled"];
            for (int i = 0; i < maxFanZonesEventsCount; i++)
            {
                events[i] = Event::FanEvent::fromJSONObj(doc["events"][i]);
            }
        }

        void reset()
        {
            enabled = false;
        }

        bool isEnabled()
        {
            return enabled;
        }

        FanZoneStatus loopTick(Time now, Measure::EnvironmentSensors *sharedSensors, Actuator::Controller *controller)
        {
            status = FanZoneStatus();

            Event::FanEvent activeEvent;

            for (int i = 0; i < maxFanZonesEventsCount; i++)
            {
                if (events[i].isActive(now))
                {
                    activeEvent = events[i];
                    ESP_LOGD(TAG, "active event id: %d", i);
                }
            }

            if (!activeEvent.isSet())
            {
                status.addError("ERROR: no active event found");
                ESP_LOGE(TAG, "No active event found");
                return status;
            }

            if (activeEvent.power > -1)
            {
                status.power = activeEvent.power;
            }

            if (activeEvent.transform.isSet())
            {
                status.power = activeEvent.transformedValue(now);
            }

            (*controller).setFan(port, status.power);
            status.power = activeEvent.power;

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
        // int ledPort = -1;
        ActuatorConfig actuator;
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
            return 128 + ColorLightZoneStatus::jsonSize() + Event::LightEvent::jsonSize() * maxDimmerZonesEventsCount + ActuatorConfig::jsonSize();
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
            doc["actuator"] = actuator.toJSON();
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
            zone.enabled = doc["enabled"];
            for (int i = 0; i < maxColorLightZonesEventsCount; i++)
            {
                zone.events[i] = Event::LightEvent::fromJSONObj(doc["events"][i]);
            }
            zone.actuator = ActuatorConfig::fromJSONObj(doc["actuator"]);

            return zone;
        }

        void updateFromJSONObj(DynamicJsonDocument doc)
        {
            slug = doc["slug"].as<std::string>();
            enabled = doc["enabled"];
            for (int i = 0; i < maxColorLightZonesEventsCount; i++)
            {
                events[i] = Event::LightEvent::fromJSONObj(doc["events"][i]);
            }
            actuator = ActuatorConfig::fromJSONObj(doc["actuator"]);
        }

        void reset()
        {
            enabled = false;
        }

        bool isEnabled()
        {
            return enabled;
        }

        ColorLightZoneStatus loopTick(Time now, Measure::EnvironmentSensors *sharedSensors, Actuator::Controller *controller)
        {

            status = ColorLightZoneStatus();

            Event::LightEvent activeEvent;

            for (int i = 0; i < maxColorLightZonesEventsCount; i++)
            {
                if (events[i].isActive(now))
                {
                    activeEvent = events[i];
                    ESP_LOGD(TAG, "Color light zone active event id: %d\n", i);
                }
            }

            if (!activeEvent.isSet())
            {
                status.addError("ERROR: no active event found");
                ESP_LOGE(TAG, "No active event found");
                return status;
            }

            if (activeEvent.isCircadian())
            {
                int brightness = 0;
                int kelvins = 0;

                int isRising = activeEvent.isRising(now);
                if (isRising == 1)
                {
                    kelvins = Transform::circadianKelvins(true, activeEvent.risingPercent(now));
                    brightness = int(activeEvent.risingPercent(now) * 100);
                }
                else if (isRising == -1)
                {
                    kelvins = Transform::circadianKelvins(false, activeEvent.fadingPercent(now));
                    brightness = 100 - int(activeEvent.fadingPercent(now) * 100);
                }
                else
                {
                    kelvins = kelvinNoon;
                    brightness = 100;
                }

                status.brightness = brightness;
                status.color = Color(kelvins);
            }
            else
            {
                // simple
                int brightness = activeEvent.brightness;
                if (activeEvent.transform.isSet())
                {
                    brightness = activeEvent.transformedValue(now);
                }

                status.color = activeEvent.color;
                status.brightness = brightness;
            }

            switch (actuator.type)
            {
            case actuatorTypeOnBoard:
                (*controller).setColorAndBrightness(0, status.color, status.brightness);
                break;
            case actuatorTypeLightDome:
                // todo add address
                // uint16_t var2 = (uint16_t) ~((unsigned int) var1);
                I2C::write(actuator.i2cID, I2C::Message(status.brightness, status.color));
                // (*controller).setColorAndBrightness(0, status.color, status.brightness);
                break;
            }

            status.slug = slug;

            return status;
        }
    };

    class ClimateService
    {
    private:
        TemperatureZone temperatureZones[maxTemperatureZonesCount];
        DimmerZone dimmerZones[maxDimmerZonesCount];
        ColorLightZone colorLightZones[maxColorLightZonesCount];
        FanZone fanZones[maxFanZonesCount];
        bool paused = false;
        bool busy = false;
        bool changed = false;

    public:
        ClimateService() {}
        void begin()
        {
            // ESP_ERROR_CHECK(I2C::i2c_master_init());

            for (int i = 0; i < maxTemperatureZonesCount; i++)
            {
                if (temperatureZones[i].isEnabled())
                {
                    temperatureZones[i].begin();
                }
            }
        }
        bool toBePersisted()
        {
            return changed;
        }
        void persisted()
        {
            changed = false;
        }
        void pause()
        {
            if (paused)
            {
                return;
            }
            // make it wait for iteratium to finish
            ESP_LOGD(TAG, "[..] pause");
            while (busy)
            {
                vTaskDelay(100 / portTICK_PERIOD_MS);
            }
            paused = true;
            ESP_LOGD(TAG, "[OK] pause");
        }
        void resume()
        {
            ESP_LOGD(TAG, "resume");
            paused = false;
        }
        ZonesStatuses loopTick(Time now, Measure::EnvironmentSensors *sharedSensors, Actuator::Controller *controller)
        {

            ZonesStatuses statuses;
            // TODO return list of statuses
            ESP_LOGD(TAG, "paused: %d", paused);
            if (!paused)
            {
                busy = true;
                sharedSensors->readSensors();

                // loop over all zones
                for (int i = 0; i < maxTemperatureZonesCount; i++)
                {

                    ESP_LOGD(TAG, "temperature zone: %d is %d", i, temperatureZones[i].isEnabled());
                    if (temperatureZones[i].isEnabled())
                    {
                        statuses.temperatureZones[i] = temperatureZones[i].loopTick(now, sharedSensors, controller);
                    }
                }
                for (int i = 0; i < maxDimmerZonesCount; i++)
                {
                    ESP_LOGD(TAG, "dimmer zone: %d is %d", i, dimmerZones[i].isEnabled());
                    if (dimmerZones[i].isEnabled())
                    {
                        statuses.dimmerZones[i] = dimmerZones[i].loopTick(now, sharedSensors, controller);
                    }
                }
                for (int i = 0; i < maxColorLightZonesCount; i++)
                {
                    ESP_LOGD(TAG, "color light zone: %d is %d", i, colorLightZones[i].isEnabled());
                    if (colorLightZones[i].isEnabled())
                    {
                        statuses.colorLightZones[i] = colorLightZones[i].loopTick(now, sharedSensors, controller);
                    }
                }
                for (int i = 0; i < maxFanZonesCount; i++)
                {
                    ESP_LOGD(TAG, "fan zone: %d is %d", i, fanZones[i].isEnabled());
                    if (fanZones[i].isEnabled())
                    {
                        statuses.fanZones[i] = fanZones[i].loopTick(now, sharedSensors, controller);
                    }
                }
                busy = false;
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
            for (int i = 0; i < maxFanZonesCount; i++)
            {
                doc["fan_zones"][i] = fanZones[i].toJSON();
            }

            ESP_LOGD(TAG, "Climate config as JSON: %s", doc);

            return doc;
        }

        static ClimateService fromJSON(std::string json)
        {
            DynamicJsonDocument doc = DynamicJsonDocument(jsonSize());
            deserializeJson(doc, json);
            return ClimateService::fromJSONObj(doc);
        }

        static ClimateService fromJSONObj(DynamicJsonDocument doc)
        {
            ClimateService climateService;

            for (int i = 0; i < maxTemperatureZonesCount; i++)
            {
                climateService.temperatureZones[i] = TemperatureZone::fromJSONObj(doc["temperature_zones"][i]);
            }

            for (int i = 0; i < maxDimmerZonesCount; i++)
            {
                climateService.dimmerZones[i] = DimmerZone::fromJSONObj(doc["dimmer_zones"][i]);
            }

            for (int i = 0; i < maxColorLightZonesCount; i++)
            {
                climateService.colorLightZones[i] = ColorLightZone::fromJSONObj(doc["color_light_zones"][i]);
            }
            for (int i = 0; i < maxFanZonesCount; i++)
            {
                climateService.fanZones[i] = FanZone::fromJSONObj(doc["fan_zones"][i]);
            }
            return climateService;
        }

        void updateFromJSON(std::string *json)
        {
            pause();
            // ESP_LOGD(TAG, "updating from JSON: %s", json->c_str());
            DynamicJsonDocument doc = DynamicJsonDocument(jsonSize());
            deserializeJson(doc, *json);
            updateFromJSONObj(&doc);
            resume();
            // unset doc
            doc.clear();
        }

        void updateFromJSONObj(DynamicJsonDocument *doc)
        {
            ESP_LOGD(TAG, "[..] Updating from json");
            // ESP_LOGD(TAG, "%s", (*doc)["temperature_zones"][0]["slug"]);
            for (int i = 0; i < maxTemperatureZonesCount; i++)
            {
                temperatureZones[i].updateFromJSONObj((*doc)["temperature_zones"][i]); // = TemperatureZone::fromJSONObj(doc["temperature_zones"][i]);
            }

            for (int i = 0; i < maxDimmerZonesCount; i++)
            {
                dimmerZones[i].updateFromJSONObj((*doc)["dimmer_zones"][i]); // = DimmerZone::fromJSONObj((*doc)["dimmer_zones"][i]);
            }

            for (int i = 0; i < maxColorLightZonesCount; i++)
            {
                colorLightZones[i].updateFromJSONObj((*doc)["color_light_zones"][i]); // = ColorLightZone::fromJSONObj((*doc)["color_light_zones"][i]);
            }

            for (int i = 0; i < maxFanZonesCount; i++)
            {
                fanZones[i].updateFromJSONObj((*doc)["fan_zones"][i]); // = ColorLightZone::fromJSONObj((*doc)["color_light_zones"][i]);
            }
            ESP_LOGD(TAG, "[OK] Updating from json");
            changed = true;
        }

        void initFromJSON(std::string *json)
        {
            pause();
            // ESP_LOGD(TAG, "updating from JSON: %s", json->c_str());
            DynamicJsonDocument doc = DynamicJsonDocument(jsonSize());
            deserializeJson(doc, *json);
            initFromJSONObj(&doc);
            resume();
            // unset doc
            doc.clear();
        }
        void initFromJSONObj(DynamicJsonDocument *doc)
        {
            ESP_LOGD(TAG, "[..] Updating from json");
            // ESP_LOGD(TAG, "%s", (*doc)["temperature_zones"][0]["slug"]);
            for (int i = 0; i < maxTemperatureZonesCount; i++)
            {
                temperatureZones[i].updateFromJSONObj((*doc)["temperature_zones"][i]); // = TemperatureZone::fromJSONObj(doc["temperature_zones"][i]);
            }

            for (int i = 0; i < maxDimmerZonesCount; i++)
            {
                dimmerZones[i].updateFromJSONObj((*doc)["dimmer_zones"][i]); // = DimmerZone::fromJSONObj((*doc)["dimmer_zones"][i]);
            }

            for (int i = 0; i < maxColorLightZonesCount; i++)
            {
                colorLightZones[i].updateFromJSONObj((*doc)["color_light_zones"][i]); // = ColorLightZone::fromJSONObj((*doc)["color_light_zones"][i]);
            }

            for (int i = 0; i < maxFanZonesCount; i++)
            {
                fanZones[i].updateFromJSONObj((*doc)["fan_zones"][i]); // = ColorLightZone::fromJSONObj((*doc)["color_light_zones"][i]);
            }
            ESP_LOGD(TAG, "[OK] Updating from json");
        }
    };
}

#endif