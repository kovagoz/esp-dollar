#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "clock.h"
#include "currency.h"
#include "wifi.h"

static const char *TAG = "app";

static void http_test_task(void *pvParameters)
{
    double exchange_rate = get_exchange_rate("USD");

    int result = (int) round(exchange_rate * 50);

    if (result == 0) {
        ESP_LOGE(TAG, "Failed to fetch the exchange rate");
    } else {
        ESP_LOGI(TAG, "50 USD = %i HUF", result);
    }

    vTaskDelete(NULL);
}

static void nvs_init()
{
    esp_err_t err = nvs_flash_init();

    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    ESP_ERROR_CHECK(err);
}

void app_main(void)
{
    ESP_LOGI(TAG, "Starting main task");

    timezone_set(CLOCK_TZ_EUROPE_BUDAPEST);

    nvs_init();

    if (wifi_connect() == ESP_OK) {
        ntp_sync();
        xTaskCreate(&http_test_task, "http_test_task", 8192, NULL, 5, NULL);
    }
}
