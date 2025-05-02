#include "hal_led.h"
#include "led_embutido.h"
#include "pico/time.h"

void hal_led_init() {
    led_embutido_init();
}

void hal_led_toggle() {
    static bool estado = false;
    led_embutido_set(estado);
    estado = !estado;
}
