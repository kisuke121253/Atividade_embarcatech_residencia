#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/time.h"
#include "inc/ssd1306.h"

#define I2C_PORT i2c1
#define SDA_PIN 14
#define SCL_PIN 15
#define BTN_A 5
#define BTN_B 6

uint8_t ssd[ssd1306_buffer_length];

volatile int contador = 0;
volatile int clicks_b = 0;
volatile bool contando = false;
volatile bool start_novo_ciclo = false;

uint64_t ultimo_clique_b = 0;
uint64_t ultimo_decremento = 0;

struct render_area frame_area = {
    start_column : 0,
    end_column : ssd1306_width - 1,
    start_page : 0,
    end_page : ssd1306_n_pages - 1
};

void display_message(const char *line1, const char *line2) {
    memset(ssd, 0, ssd1306_buffer_length);

    int x1 = (ssd1306_width - strlen(line1) * 6) / 2;
    int x2 = (ssd1306_width - strlen(line2) * 6) / 2;

    int font_height = 8;
    int line_spacing = 12;
    int total_height = 2 * font_height + line_spacing;
    int y1 = (ssd1306_height - total_height) / 2;
    int y2 = y1 + font_height + line_spacing;

    ssd1306_draw_string(ssd, x1, y1, line1);
    ssd1306_draw_string(ssd, x2, y2, line2);

    render_on_display(ssd, &frame_area);
}

void update_display() {
    char line1[20];
    char line2[20];

    snprintf(line1, sizeof(line1), "Tempo: %d", contador);
    snprintf(line2, sizeof(line2), "Cliques: %d", clicks_b);

    display_message(line1, line2);
}

void gpio_callback(uint gpio, uint32_t events) {
    if (gpio == BTN_A) {
        start_novo_ciclo = true;
    } else if (gpio == BTN_B && contando && contador > 0) {
        uint64_t agora = time_us_64();
        if (agora - ultimo_clique_b > 200000) { 
            ultimo_clique_b = agora;
            clicks_b++;
            update_display();
        }
    }
}

int main() {
    stdio_init_all();

    i2c_init(I2C_PORT, ssd1306_i2c_clock * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    gpio_init(BTN_A);
    gpio_set_dir(BTN_A, GPIO_IN);
    gpio_pull_up(BTN_A);

    gpio_init(BTN_B);
    gpio_set_dir(BTN_B, GPIO_IN);
    gpio_pull_up(BTN_B);

    gpio_set_irq_enabled_with_callback(BTN_A, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(BTN_B, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    ssd1306_init();
    calculate_render_area_buffer_length(&frame_area);
    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);

    while (1) {
        if (start_novo_ciclo) {
            contador = 9;
            clicks_b = 0;
            contando = true;
            start_novo_ciclo = false;
            ultimo_decremento = time_us_64();
            update_display();
        }

        if (contando && contador > 0) {
            uint64_t agora = time_us_64();
            if (agora - ultimo_decremento >= 1000000) { // 1 segundo
                contador--;
                ultimo_decremento = agora;
                update_display();

                if (contador == 0) {
                    contando = false;
                    update_display();
                }
            }
        }

        tight_loop_contents();
    }

    return 0;
}
