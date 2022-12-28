#include <unity.h>
#include <Arduino.h>
#include "../src/transform.h"

void test_Transform_circadianKelvins_unit(bool isRising, float percent, int expected)
{

    int actual = Transform::circadianKelvins(isRising, percent);

    TEST_ASSERT_EQUAL_INT(expected, actual);
}

void test_Transform_circadianKelvins(void)
{
    test_Transform_circadianKelvins_unit(true, 0.01, 1100);
    test_Transform_circadianKelvins_unit(true, 1.00, 6500);

    test_Transform_circadianKelvins_unit(false, 1.00, 1000);
    test_Transform_circadianKelvins_unit(false, 0.01, 6500);
    test_Transform_circadianKelvins_unit(false, 0.10, 6000);
}