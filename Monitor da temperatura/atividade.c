#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"

#define I2C_PORT i2c1
#define SDA_PIN 14
#define SCL_PIN 15

uint8_t ssd[ssd1306_buffer_length];
struct render_area frame_area = {
    start_column : 0,
    end_column : ssd1306_width - 1,
    start_page : 0,
    end_page : ssd1306_n_pages - 1
};

#define ADC_REF_VOLTAGE 3.3f
#define ADC_RESOLUTION (1 << 12) 
#define TEMP_SENSOR_ADC_CHANNEL 4 
#define SAMPLE_SIZE 10 
#define TEMP_SENSOR_25C 0.706f 
#define TEMP_SENSOR_MV_PER_C 1.721f 

void display_message(const char *line1, const char *line2, const char *line3) {
    memset(ssd, 0, ssd1306_buffer_length);
    
    int x1 = 10, y1 = 10;
    int x2 = 10, y2 = 30;
    int x3 = 10, y3 = 50;
    
    ssd1306_draw_string(ssd, x1, y1, line1);
    ssd1306_draw_string(ssd, x2, y2, line2);
    ssd1306_draw_string(ssd, x3, y3, line3);
    
    render_on_display(ssd, &frame_area);
}

float read_temperature() {
    // Configura o ADC para o sensor de temperatura
    adc_select_input(TEMP_SENSOR_ADC_CHANNEL);
    
    uint32_t sum = 0;
    for (int i = 0; i < SAMPLE_SIZE; i++) {
        sum += adc_read();
        sleep_ms(1);
    }
    float adc_value = (float)sum / SAMPLE_SIZE;
    
    float voltage = adc_value * (ADC_REF_VOLTAGE / ADC_RESOLUTION);
    
    float temperature = 25.0f - ((voltage - TEMP_SENSOR_25C) * 1000.0f / TEMP_SENSOR_MV_PER_C);
    
    return temperature;
}

int main() {
    stdio_init_all();
    
    adc_init();
    adc_set_temp_sensor_enabled(true);
    
    i2c_init(I2C_PORT, ssd1306_i2c_clock * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);
    
    ssd1306_init();
    calculate_render_area_buffer_length(&frame_area);
    
    while(1) {
        float temperature = read_temperature();
        
        char line1[20], line2[20], line3[20];
        snprintf(line1, sizeof(line1), "RP2040 Temp:");
        snprintf(line2, sizeof(line2), "%.2f C", temperature);
        snprintf(line3, sizeof(line3), "%.2f F", temperature * 9/5 + 32);
        
        display_message(line1, line2, line3);
        sleep_ms(1000);
    }
    
    return 0;
}