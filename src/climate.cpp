#include "climate.h"

namespace Climate
{

#define DHTTYPE DHT22
#define SENSORS_ENABLE_PIN 32

// DHT22 sensors pins
#define DHT_0 int(16) // #0
#define DHT_1 int(17) // #1
#define DHT_2 int(18) // #2
#define DHT_3 int(19) // #3
#define DHT_4 int(27) // #4
#define DHT_5 int(26) // #5
// relay pins
#define RELAY_0_PIN 4
#define RELAY_1_PIN 25
#define RELAY_2_PIN 5

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
        int relayPin;
        DHT22CustomSensor sensorsDHT22[MAX_DHT22_SENSORS_IN_CLIMATE_ZONE];
        Schedule schedule[MAX_SCHEDULE_COUNT];
        bool isSet = false;

        float averageTemp;
        float averageHumid;

        HeaterPhase heaterPhase;

        ClimateZone() {}
        ClimateZone(std::string givenName, int DHT22SensorPins[MAX_DHT22_SENSORS_IN_CLIMATE_ZONE], int givenRelayPin, Schedule givenSchedule[MAX_SCHEDULE_COUNT])
        {
            name = givenName;
            for (int i = 0; i < MAX_DHT22_SENSORS_IN_CLIMATE_ZONE; i++)
            {
                if (DHT22SensorPins[i] != 0)
                {
                    sensorsDHT22[i] = DHT22CustomSensor(DHT22SensorPins[i]);
                }
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
            std::string now = hourMinuteToString(hour, minute);
            for (int i = 0; i < MAX_SCHEDULE_COUNT; i++)
            {

                if (schedule[i].isSet)
                {
                    std::string since = hourMinuteToString(schedule[i].sinceHour, schedule[i].sinceMinute);
                    std::string until = hourMinuteToString(schedule[i].untilHour, schedule[i].untilMinute);
                    if (since.compare(now) <= 0 && until.compare(now) > 0)
                    {
                        currentSchedule = schedule[i];
                    }
                }

                // // event in the same hour
                // if (schedule[i].sinceHour == hour && schedule[i].untilHour == hour)
                // {
                //     if (schedule[i].sinceMinute <= minute && schedule[i].untilMinute > minute)
                //     {
                //         currentSchedule = schedule[i];
                //     }
                // }
                // else if (schedule[i].sinceHour == hour)
                // {
                //     if (schedule[i].sinceMinute <= minute && schedule[i].untilHour > hour)
                // }

                // if (schedule[i].sinceHour <= hour && schedule[i].sinceMinute <= minute && schedule[i].untilHour >= hour && schedule[i].untilMinute > minute)
                // {
                //     currentSchedule = schedule[i];
                // }
            }
            // check with schedule for adjustments
            bool heaterOn = false;
            if (averageTemp > currentSchedule.temperature)
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

            dataClimateZone.name = name;

            dataClimateZone.isSet = true;

            return dataClimateZone;
        }
    };

    ClimateZone climateZones[MAX_CLIMATE_ZONES];

    void setup()
    {
        pinMode(SENSORS_ENABLE_PIN, OUTPUT);
        pinMode(RELAY_0_PIN, OUTPUT);
        pinMode(RELAY_1_PIN, OUTPUT);
        pinMode(RELAY_2_PIN, OUTPUT);
        digitalWrite(RELAY_0_PIN, LOW);
        digitalWrite(RELAY_1_PIN, LOW);
        digitalWrite(RELAY_2_PIN, LOW);
        int sensors[MAX_DHT22_SENSORS_IN_CLIMATE_ZONE] = {DHT_0, DHT_1, DHT_2};
        Schedule schedule[MAX_SCHEDULE_COUNT];
        schedule[0].sinceHour = 9;
        schedule[0].sinceMinute = 0;
        schedule[0].untilHour = 18;
        schedule[0].untilMinute = 0;
        schedule[0].temperature = 29;
        schedule[0].isSet = true;
        climateZones[0] = ClimateZone(std::string("hot corner"), sensors, RELAY_0_PIN, schedule);
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

    std::string hourMinuteToString(int hour, int minute)
    {
        static char buffer[10];
        sprintf(buffer, "%02d:%02d", hour, minute);
        //Serial.println(buffer);
        return std::string(buffer);
    }
}
