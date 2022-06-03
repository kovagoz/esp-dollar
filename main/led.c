#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include <string.h>
#include "esp_err.h"
#include "max7219.h"

#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK  18
#define PIN_NUM_CS    5

static max7219_t dev = {
   .cascade_size = 1,
   .digits = 8,
   .mirrored = true
};

static void led_init()
{
    static uint8_t initialized = 0;

    if (initialized == 0) {
        // Unused IOs must be -1
        spi_bus_config_t buscfg = {
            .mosi_io_num = PIN_NUM_MOSI, // ESP => LED
            .miso_io_num = -1, // LED => ESP (unused in this case)
            .sclk_io_num = PIN_NUM_CLK,
            .quadwp_io_num = -1,
            .quadhd_io_num = -1,
            .max_transfer_sz = 0,
            .flags = 0
        };

        ESP_ERROR_CHECK(spi_bus_initialize(VSPI_HOST, &buscfg, 1));
        ESP_ERROR_CHECK(max7219_init_desc(&dev, VSPI_HOST, MAX7219_MAX_CLOCK_SPEED_HZ, PIN_NUM_CS));
        ESP_ERROR_CHECK(max7219_init(&dev));

        max7219_clear(&dev);
        max7219_set_brightness(&dev, CONFIG_APP_LED_BRIGHTNESS);

        initialized = 1;
    }
}

void led_spinner_task()
{
    uint8_t i = 2;

    led_init();

    for (;;) {
        if (i == 64) {
            i = 2;
        } else {
            i = i << 1;
        }

        for (uint8_t j = 0; j < MAX7219_MAX_CASCADE_SIZE; j++) {
            max7219_set_digit(&dev, j, i);
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void led_clear()
{
    led_init();
    max7219_clear(&dev);
}

void led_show_number(double num)
{
    char buf[MAX7219_MAX_CASCADE_SIZE];
    led_init();
    sprintf(buf, "%.0f", num);
    max7219_draw_text_7seg(&dev, 8 - strlen(buf), buf);
}

void led_show_text(char *message)
{
    led_init();
    max7219_draw_text_7seg(&dev, 0, message);
}
