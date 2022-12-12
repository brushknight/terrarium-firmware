// #include <Arduino.h>
// #include "Wire.h"
// #include "Adafruit_I2CDevice.h"
// #include "SPI.h"
#include <unity.h>
#include "../src/utils.cpp"
#include "../src/config.h"
#include "../src/event.h"

void test_time_inRange_unit(std::string now, std::string since, std::string until, bool expected)
{

    Event::Time nowTime = Event::Time::fromString(now);
    Event::Time sinceTime = Event::Time::fromString(since);
    Event::Time untilTime = Event::Time::fromString(until);

    bool actual = nowTime.inRange(sinceTime, untilTime);

    static char message[100];
    sprintf(message, "%s | %s -> %s\n", now.c_str(), since.c_str(), until.c_str());

    // debug
    // int overnight = sinceTime.compare(untilTime) > 0;
    // bool nowVsSince = nowTime.compare(sinceTime);
    // bool nowVsUntil = nowTime.compare(untilTime);
    // bool inRange = nowTime.compare(sinceTime) >= 0 && nowTime.compare(untilTime) <= 0;
    // sprintf(message, "%s | %s -> %s | overnight: %d, inRange: %d, now vs sunce: %d, now vs until: %d\n",now.c_str(), since.c_str(), until.c_str(), overnight, inRange, nowVsSince, nowVsUntil);

    TEST_ASSERT_EQUAL_INT_MESSAGE(expected, actual, message);
}

void test_time_compare_unit(std::string now, std::string to, int expected)
{

    Event::Time nowTime = Event::Time::fromString(now);
    Event::Time toTime = Event::Time::fromString(to);

    int actual = nowTime.compare(toTime);

    static char message[100];

    sprintf(message, "%s %s \n", now.c_str(), to.c_str());

    TEST_ASSERT_EQUAL_INT_MESSAGE(expected, actual, message);
}

void test_time_diff_unit(std::string now, std::string to, int expected)
{
    Event::Time nowTime = Event::Time::fromString(now);
    Event::Time toTime = Event::Time::fromString(to);

    int actual = nowTime.diff(toTime);

    static char message[100];

    sprintf(message, "%s -> %s diff: %d \n", now.c_str(), to.c_str(), actual);

    TEST_ASSERT_EQUAL_INT_MESSAGE(expected, actual, message);
}

void test_transform_direction_unit(int from, int to, int expected)
{

    Event::Transform tr = Event::Transform(from, to);

    int actual = tr.direction();

    static char message[100];

    sprintf(message, "%d -> %d direction: %d \n", from, to, actual);

    TEST_ASSERT_EQUAL_INT_MESSAGE(expected, actual, message);
}

void test_event_transformPercent_unit(std::string now, std::string since, std::string until, float expected)
{
    Event::Event e = Event::Event(since, until, 0);
    Event::Time nowTime = Event::Time::fromString(now);

    float actual = e.transformPercent(nowTime);

    static char message[100];

    sprintf(message, "now: %s | %s -> %s | actual: %.5f expected: %.5f \n", now.c_str(), since.c_str(), until.c_str(), actual, expected);

    TEST_ASSERT_TRUE_MESSAGE(std::abs(expected - actual) < 0.001, message);
}

void test_event_transformedValue_unit(std::string now, std::string since, std::string until, int from, int to, float expected)
{
    Event::Event e = Event::Event(since, until, 0);
    e.transform.from = from;
    e.transform.to = to;
    Event::Time nowTime = Event::Time::fromString(now);

    float actual = e.transformedValue(nowTime);

    static char message[100];

    sprintf(message, "now: %s | %s -> %s | %d -> %d | actual: %.5f expected: %.5f \n", now, since, until, from, to, actual, expected);

    TEST_ASSERT_TRUE_MESSAGE(std::abs(expected - actual) < 0.1, message);
}

void test_time_inRange(void)
{

    test_time_inRange_unit("09:15", "09:00", "10:00", true);  // in range
    test_time_inRange_unit("09:15", "09:16", "10:00", false); // out of range
    test_time_inRange_unit("09:15", "10:00", "09:00", false); // over night - out
    test_time_inRange_unit("09:15", "20:00", "10:00", true);  // over night - in range
    test_time_inRange_unit("09:15", "09:15", "10:00", true);  // bottom edge
    test_time_inRange_unit("09:59", "09:15", "10:00", true);  // top edge
    test_time_inRange_unit("10:00", "09:15", "10:00", true);  // top edge include
    test_time_inRange_unit("10:00", "09:00", "09:00", true);  // all day
}

void test_time_compare(void)
{
    test_time_compare_unit("09:15", "09:00", 1);  // 1 > 2
    test_time_compare_unit("09:00", "10:00", -1); // 1 < 2
    test_time_compare_unit("09:15", "09:16", -1); // 1 < 2
    test_time_compare_unit("09:15", "09:15", 0);  // 1 == 2
}

void test_time_diff(void)
{
    test_time_diff_unit("10:00", "11:00", 60);
    test_time_diff_unit("10:00", "11:30", 90);
    test_time_diff_unit("10:00", "09:00", 60 * 23);
    test_time_diff_unit("09:00", "08:59", 60 * 23 + 59);
}

void test_transform_direction(void)
{
    test_transform_direction_unit(0, 10, 1);
    test_transform_direction_unit(10, 10, 0);
    test_transform_direction_unit(10, 0, -1);
}

void test_event_transformPercent(void)
{
    test_event_transformPercent_unit("10:00", "09:00", "11:00", 0.5);
    // todo add more tests
}

void test_event_transformedValue(void){
    test_event_transformedValue_unit("10:00", "09:00", "11:00", 20, 30, 25);
    // todo add more tests
}

void setup()
{
    delay(2000);
    Serial.begin(115200);
    UNITY_BEGIN();
    RUN_TEST(test_time_compare);
    RUN_TEST(test_time_inRange);
    RUN_TEST(test_time_diff);
    RUN_TEST(test_transform_direction);
    RUN_TEST(test_event_transformPercent);
    RUN_TEST(test_event_transformedValue);
}

void loop()
{
    return;
}