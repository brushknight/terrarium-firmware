#include "climate.h"

namespace Climate
{

#define DHTTYPE DHT22

    DHTStable DHT;

    class DHT22CustomSensor
    {
    public:
        int pin;
        ClimateData current;
        ClimateData previous;
        // just for statistics
        int errorsInARow = 0;
        int succesfulReadingsCountInARow = 0;

        DHT22CustomSensor()
        {
        }

        DHT22CustomSensor(int p)
        {
            pin = p;
        }

        bool success()
        {
            return current.status == DHTLIB_OK;
        }

        bool isSet()
        {
            return pin > 0;
        }

        ClimateData getCurrent()
        {

            if (!isSet())
            {
                return ClimateData(0, 0, DHTLIB_ERROR_TIMEOUT);
            }

            return current;
        }

        bool read()
        {
            Serial.printf("[pin:%d] reading sensor\n", pin);

            //Status::setClimateStatus(Status::WORKING);

            if (pin == 0)
            {
                Serial.printf("[pin:%d] sensor disabled\n", pin);
                return false;
            }

            int status = DHT.read22(pin);

            switch (status)
            {
            case DHTLIB_OK:
                previous = current;

                current.h = DHT.getHumidity();
                current.t = DHT.getTemperature();

                // TODO check for data error (more than X% in x Seconds)

                Serial.printf("[pin:%d] Temperature: %.2f°C\n", pin, current.t);
                Serial.printf("[pin:%d] Humidity: %.2f%%\n", pin, current.h);
                current.status = status;
                //Status::setClimateStatus(Status::SUCCESS);
                return true;
            case DHTLIB_ERROR_CHECKSUM:
                //counter.crc_error++;
                Serial.printf("[pin:%d] Checksum error,\n", pin);
                current.h = 0;
                current.t = 0;
                current.status = status;
                //Status::setClimateStatus(Status::WARNING);
                return false;
            case DHTLIB_ERROR_TIMEOUT:
                //counter.time_out++;
                Serial.printf("[pin:%d] Time out error,\n", pin);
                current.h = 0;
                current.t = 0;
                current.status = status;
                //Status::setClimateStatus(Status::WARNING);
                return false;
            default:
                //counter.unknown++;
                Serial.printf("[pin:%d] Unknown error,\n", pin);
                current.h = 0;
                current.t = 0;
                current.status = status;
                //Status::setClimateStatus(Status::WARNING);
                return false;
            }

            return false;
        }
    };

    class ClimateZone
    {
    public:
        std::string name;
        std::string slug;
        int relayPin;
        DHT22CustomSensor sensorsDHT22[MAX_DHT22_SENSORS_IN_CLIMATE_ZONE];
        Schedule schedule[MAX_SCHEDULE_COUNT];
        bool isSet = false;

        float averageTemp;
        float averageHumid;

        float targetTemperature;

        HeaterPhase heaterPhase;

        ClimateZone() {}
        ClimateZone(std::string givenName, std::string givenSlug, int DHT22SensorPins[MAX_DHT22_SENSORS_IN_CLIMATE_ZONE], int givenRelayPin, Schedule givenSchedule[MAX_SCHEDULE_COUNT])
        {
            name = givenName;
            slug = givenSlug;
            for (int i = 0; i < MAX_DHT22_SENSORS_IN_CLIMATE_ZONE; i++)
            {
                sensorsDHT22[i] = DHT22CustomSensor(DHT22SensorPins[i]);
            }
            relayPin = givenRelayPin;

            for (int i = 0; i < MAX_SCHEDULE_COUNT; i++)
            {
                schedule[i] = givenSchedule[i];
            }

            isSet = true;
        }
        void turnRelayOn()
        {
            heaterPhase = heating;
            digitalWrite(relayPin, HIGH);
        }

        void turnRelayOff()
        {
            heaterPhase = cooling;
            digitalWrite(relayPin, LOW);
        }

        void adjust(int hour, int minute)
        {

            // read sesors and check for errors
            for (int i = 0; i < MAX_DHT22_SENSORS_IN_CLIMATE_ZONE; i++)
            {
                if (sensorsDHT22[i].isSet())
                {
                    int retryAttempt = 0;
                    while (!sensorsDHT22[i].read() && retryAttempt < MAX_SENSOR_RETRY)
                    {
                        rebootSensorsWithWait();
                        retryAttempt++;
                    }
                }
            }

            // todo: add watchdog for wrong data

            // find average metrics for climate zone
            int sensorsEnabledCount = 0;
            float sumTemp = 0;
            float sumHumid = 0;
            for (int i = 0; i < MAX_DHT22_SENSORS_IN_CLIMATE_ZONE; i++)
            {
                if (sensorsDHT22[i].isSet() && sensorsDHT22[i].success())
                {
                    sensorsEnabledCount++;
                    sumTemp += sensorsDHT22[i].current.t;
                    sumHumid += sensorsDHT22[i].current.h;
                }
            }
            averageTemp = sumTemp / float(sensorsEnabledCount);
            averageHumid = sumHumid / float(sensorsEnabledCount);

            // find current schedule
            // wrap into a func and write tests
            Schedule currentSchedule;
            std::string now = Utils::hourMinuteToString(hour, minute);
            for (int i = 0; i < MAX_SCHEDULE_COUNT; i++)
            {

                if (schedule[i].isSet)
                {
                    std::string since = Utils::hourMinuteToString(schedule[i].sinceHour, schedule[i].sinceMinute);
                    std::string until = Utils::hourMinuteToString(schedule[i].untilHour, schedule[i].untilMinute);

                    if (RealTime::checkScheduleTimeWindow(now, since, until))
                    {
                        currentSchedule = schedule[i];
                    }
                }
            }

            targetTemperature = currentSchedule.temperature;

            // check with schedule for adjustments
            bool heaterOn = false;
            if (averageTemp > targetTemperature)
            {
                heaterOn = false;
            }
            else
            {
                heaterOn = true;
            }

            if (heaterOn)
            {
                turnRelayOn();
            }
            else
            {
                turnRelayOff();
            }

            // return current cliamte zones statuses
        }

        DataClimateZone toDataClimateZone()
        {
            DataClimateZone dataClimateZone = DataClimateZone();

            // todo: cleanup
            dataClimateZone.heaterStatus = heaterPhase == heating;
            dataClimateZone.heatingPhase = heaterPhase == heating;

            dataClimateZone.humidity = averageHumid;
            dataClimateZone.temperature = averageTemp;
            dataClimateZone.targetTemperature = targetTemperature;

            dataClimateZone.name = name;
            dataClimateZone.slug = slug;

            dataClimateZone.isSet = true;

            return dataClimateZone;
        }
    };

    ClimateZone climateZones[MAX_CLIMATE_ZONES];

    void setup(Config config)
    {
        pinMode(SENSORS_ENABLE_PIN, OUTPUT);
        pinMode(RELAY_0_PIN, OUTPUT);
        pinMode(RELAY_1_PIN, OUTPUT);
        pinMode(RELAY_2_PIN, OUTPUT);
        digitalWrite(RELAY_0_PIN, LOW);
        digitalWrite(RELAY_1_PIN, LOW);
        digitalWrite(RELAY_2_PIN, LOW);

        for (int i = 0; i < MAX_CLIMATE_ZONES; i++)
        {
            if (config.climateZoneConfigs[i].isSet)
            {
                climateZones[i] = ClimateZone(
                    config.climateZoneConfigs[i].name,
                    config.climateZoneConfigs[i].slug,
                    config.climateZoneConfigs[i].dht22SensorPins,
                    config.climateZoneConfigs[i].relayPin,
                    config.climateZoneConfigs[i].schedule);
            }
        }
    }

    DataClimateZone *control(int hour, int minute)
    {

        static DataClimateZone result[MAX_CLIMATE_ZONES];

        for (int i = 0; i < MAX_CLIMATE_ZONES; i++)
        {
            if (climateZones[i].isSet)
            {
                climateZones[i].adjust(hour, minute);
                result[i] = climateZones[i].toDataClimateZone();
            }
        }

        return result;
    }

    void enableSensors()
    {
        digitalWrite(SENSORS_ENABLE_PIN, HIGH);
    }

    void disableSensors()
    {
        digitalWrite(SENSORS_ENABLE_PIN, LOW);
    }

    void rebootSensorsWithWait()
    {
        //Status::setClimateStatus(Status::WARNING);
        disableSensors();
        delay(1000);
        enableSensors();
        delay(3000); // wait for sensros to boot
        //Status::setClimateStatus(Status::IDLE);
    }

}
