#ifndef CONFIG
#define CONFIG

#include <string>

//#define SENSORS_COUNT 2
#define TERRARIUM_ID 100

//#define VERSION "v0.0.0-dev"

//#define DEBUG false

//#define DISPLAY_ENABLED true
//#define RTC_ENABLED true

//#define SERVER_ENABLED true

//#define STATUS_LED_ENABLED true

//#define SENSORS_ENABLED true

//#define TEST_BOARD false
//#define DEMO_BOARD true

#define MAX_DHT22_SENSORS_IN_CLIMATE_ZONE int(3)
#define MAX_CLIMATE_ZONES int(3)
#define MAX_SENSOR_RETRY int(3)
#define MAX_SCHEDULE_COUNT int(10)

#define CLIMATE_LOOP_INTERVAL_SEC 5
#define DISPLAY_RENDER_INTERVAL_SEC 1

// Hardware
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

class Schedule
{
public:
    int sinceHour;
    int sinceMinute;
    int untilHour;
    int untilMinute;
    float temperature;
    bool isSet = false;
};

class ClimateZoneConfig
{
public:
    std::string name;
    std::string slug;
    int relayPin = 0;
    int dht22SensorPins[MAX_DHT22_SENSORS_IN_CLIMATE_ZONE] = {0,0,0};
    bool isSet = false;
    Schedule schedule[MAX_SCHEDULE_COUNT];
};

class Config
{
public:
    ClimateZoneConfig climateZoneConfigs[MAX_CLIMATE_ZONES];
};

Config loadConfig();

#endif