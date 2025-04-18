#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"

#define I2C_PORT i2c1
#define SDA_PIN 14
#define SCL_PIN 15

#define JOYSTICK_X_PIN 26  // GP26 (ADC0)
#define JOYSTICK_Y_PIN 27  // GP27 (ADC1)
#define JOYSTICK_SW_PIN 22 // GP22 (botão)

#define SAMPLE_SIZE 5      
#define DEADZONE 50        

uint8_t ssd[ssd1306_buffer_length];
int center_x, center_y;   

struct render_area frame_area = {
    start_column : 0,
    end_column : ssd1306_width - 1,
    start_page : 0,
    end_page : ssd1306_n_pages - 1
};

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

int read_filtered_adc(int channel) {
    int sum = 0;
    adc_select_input(channel);
    
    for(int i = 0; i < SAMPLE_SIZE; i++) {
        sum += adc_read();
        sleep_ms(1);
    }
    return sum / SAMPLE_SIZE;
}

void calibrate_joystick() {
    center_x = read_filtered_adc(0);
    center_y = read_filtered_adc(1);
    
    char cal_msg[30];
    snprintf(cal_msg, sizeof(cal_msg), "Calibrado: X=%d Y=%d", center_x, center_y);
    display_message("Calibrando...", cal_msg, "Solte o joystick");
    sleep_ms(1000);
}

int apply_deadzone(int value, int center) {
    if(abs(value - center) < DEADZONE) {
        return center;
    }
    return value;
}

void update_display(int x_value, int y_value, bool button_pressed) {
    char line1[20], line2[20], line3[20];
    
    snprintf(line1, sizeof(line1), "X: %4d (%+4d)", x_value, x_value - center_x);
    snprintf(line2, sizeof(line2), "Y: %4d (%+4d)", y_value, y_value - center_y);
    snprintf(line3, sizeof(line3), "BTN: %s", button_pressed ? "ON " : "OFF");
    
    display_message(line1, line2, line3);
}

void init_joystick() {
    adc_init();
    adc_gpio_init(JOYSTICK_X_PIN);
    adc_gpio_init(JOYSTICK_Y_PIN);
    
    gpio_init(JOYSTICK_SW_PIN);
    gpio_set_dir(JOYSTICK_SW_PIN, GPIO_IN);
    gpio_pull_up(JOYSTICK_SW_PIN);
}

int main() {
    stdio_init_all();
    init_joystick();
    
    i2c_init(I2C_PORT, ssd1306_i2c_clock * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    
    ssd1306_init();
    calculate_render_area_buffer_length(&frame_area);
    
    calibrate_joystick();
    
    while(1) {
        int raw_x = read_filtered_adc(0);
        int raw_y = read_filtered_adc(1);
        
        int filtered_x = apply_deadzone(raw_x, center_x);
        int filtered_y = apply_deadzone(raw_y, center_y);
        
        bool button_pressed = !gpio_get(JOYSTICK_SW_PIN);
        
        update_display(filtered_x, filtered_y, button_pressed);
        sleep_ms(100);
    }
    
    return 0;
}