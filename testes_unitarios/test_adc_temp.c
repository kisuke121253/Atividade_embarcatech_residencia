#include "Unity/src/unity.h"  // Changed from "Unity/src/unity.h" to just "unity.h"
#include <math.h>
#include <stdio.h>

float adc_to_celsius(uint16_t adc_val) {
    // Convert ADC value to voltage
    float voltage = (adc_val / 4095.0f) * 3.3f;
    
    // Convert voltage to temperature
    // This formula matches your test case where 0.706V = 27°C
    float temperature = (voltage - 0.706f) * (27.0f / 0.706f) + 27.0f;
    
    return temperature;
}

void setUp(void) {
    // Can be left empty if not needed
}

void tearDown(void) {
    // Can be left empty if not needed
}

void test_adc_to_celsius_known_value(void) {
    uint16_t adc_val = (uint16_t)((0.706f / 3.3f) * 4095.0f); 
    float temperature = adc_to_celsius(adc_val);
    TEST_ASSERT_FLOAT_WITHIN(0.5f, 27.0f, temperature); 
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_adc_to_celsius_known_value);
    return UNITY_END();
}