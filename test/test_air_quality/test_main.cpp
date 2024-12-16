#include <unity.h>
#include <string>
#include "../src/AirQualityModels.h"


void test_clean_function() {
    debuguear::AirQualityModel model;

    model.pm10_standard = 50;
    model.pm25_standard = 25;
    model.clean();

    TEST_ASSERT_EQUAL(0, model.pm10_standard);
    TEST_ASSERT_EQUAL(0, model.pm25_standard);
}

void test_toString_function() {
    debuguear::AirQualityModel model;

    model.pm10_standard = 50;
    model.pm25_standard = 25;
    String result = model.toString();

    TEST_ASSERT_TRUE(result.indexOf("PM10 (env):") > 0);
}

int main() {

    UNITY_BEGIN();

    RUN_TEST(test_clean_function);
    RUN_TEST(test_toString_function);

    UNITY_END();
    return 0;
}
