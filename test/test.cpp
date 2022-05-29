#include <Arduino.h>
#include <unity.h>
#include "../src/real_time.cpp"
#include "../src/utils.cpp"
#include "../src/config.h"
#include "../src/config.cpp"

void test_config_serialization(void)
{

    Config config = loadConfig();
    config.climateZoneConfigs[0].relayPin = 10;
    DynamicJsonDocument doc = config.toJSON();
    // Lastly, you can print the resulting JSON to a String
    std::string output;
    serializeJson(doc, output);
    Config actualConfig = Config::fromJSON(output);
    

    TEST_ASSERT_EQUAL(
        config.climateZoneConfigs[0].relayPin, 
        actualConfig.climateZoneConfigs[0].relayPin);
}

void test_schedule_table(std::string now, std::string since, std::string until, bool expected)
{

    int actual = RealTime::checkScheduleTimeWindow(now, since, until);
    Serial.printf("%s | %s -> %s\n", now.c_str(), since.c_str(), until.c_str());
    TEST_ASSERT_EQUAL(expected, actual);
}

void test_schedule(void)
{

    test_schedule_table("09:15", "09:00", "10:00", true);  // in range
    test_schedule_table("09:15", "09:16", "10:00", false); // out of range
    test_schedule_table("09:15", "10:00", "09:00", false); // over night - out
    test_schedule_table("09:15", "20:00", "10:00", true);  // over night
    test_schedule_table("09:15", "09:15", "10:00", true);  // bottom edge
    test_schedule_table("09:59", "09:15", "10:00", true);  // top edge
    test_schedule_table("10:00", "09:15", "10:00", false); // top edge - out
    test_schedule_table("10:00", "09:00", "09:00", true);  // all day
}

void setup()
{

    delay(2000);
    Serial.begin(115200);
    UNITY_BEGIN();
    RUN_TEST(test_config_serialization);
    RUN_TEST(test_schedule);
}

void loop() {}