#include <unity.h>
#include <Arduino.h>
#include "../src/data_structures.h"

void test_TemperatureMeasurments_calcError2_unit(float first, float second, float expected)
{

    TemperatureMeasurments tm = TemperatureMeasurments();

    tm.add(first);
    tm.add(second);

    float actual = tm.calcError();

    static char message[100];
    sprintf(message, " > %0.2f %0.2f | expected error %0.2f, got %0.2f\n", first, second, expected, actual);

    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(expected, actual, message);
}

void test_TemperatureMeasurments_calcError3_unit(float first, float second, float third, float expected)
{

    TemperatureMeasurments tm = TemperatureMeasurments();

    tm.add(first);
    tm.add(second);
    tm.add(third);

    float actual = tm.calcError();

    static char message[100];
    sprintf(message, " > %0.2f, %0.2f, %0.2f | expected error %0.2f, got %0.2f\n", first, second, third, expected, actual);

    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(expected, actual, message);
}

void test_Color_fromKelvin_unit(int k, Color expected)
{

    Color actual = Color(k);

    TEST_ASSERT_EQUAL_INT(expected.red, actual.red);
    TEST_ASSERT_EQUAL_INT(expected.green, actual.green);
    TEST_ASSERT_EQUAL_INT(expected.blue, actual.blue);
    TEST_ASSERT_EQUAL_INT(expected.kelvin, actual.kelvin);
}

void test_TemperatureMeasurments_calcError(void)
{
    test_TemperatureMeasurments_calcError2_unit(1.0, 2.0, 1.0);
    test_TemperatureMeasurments_calcError2_unit(1.5, 2.0, 0.5);
    test_TemperatureMeasurments_calcError2_unit(1.7, 2.0, 0.3);
    test_TemperatureMeasurments_calcError2_unit(2.0, 2.0, 0.0);
    test_TemperatureMeasurments_calcError2_unit(2.0, 1.7, 0.3);
    test_TemperatureMeasurments_calcError2_unit(2.0, 1.5, 0.5);
    test_TemperatureMeasurments_calcError2_unit(2.0, 1.0, 1.0);

    test_TemperatureMeasurments_calcError3_unit(2.0, 1.5, 1.0, 1.0);
    test_TemperatureMeasurments_calcError3_unit(2.0, 2.1, 1.5, 0.6);
}

void test_Color_fromKelvin(void)
{
    test_Color_fromKelvin_unit(1000, Color(255, 56, 0, 1000));
    test_Color_fromKelvin_unit(900, Color(255, 56, 0, 1000));
    test_Color_fromKelvin_unit(1050, Color(255, 71, 0, 1100));
}