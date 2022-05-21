#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "wifi.h"
#include "currency.h"

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

void app_main(void)
{
    ESP_LOGI(TAG, "Starting main task");

    esp_err_t err;

    err = nvs_flash_init();

    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    ESP_ERROR_CHECK(err);

    nvs_handle_t nvs_handler;

    ESP_ERROR_CHECK(nvs_open("app", NVS_READWRITE, &nvs_handler));

    size_t size;

    err = nvs_get_str(nvs_handler, "USD", NULL, &size);

    // If found
    if (err == ESP_OK) {
        char *out = malloc(size);
        ESP_ERROR_CHECK(nvs_get_str(nvs_handler, "USD", out, &size));
        ESP_LOGI(TAG, "%s", out);
    }

    nvs_close(nvs_handler);

    if (err == ESP_ERR_NVS_NOT_FOUND) {
        if (wifi_connect() == ESP_OK) {
            ESP_LOGI(TAG, "WiFi connected");
            xTaskCreate(&http_test_task, "http_test_task", 8192, NULL, 5, NULL);
        } else {
            ESP_LOGE(TAG, "WiFi connection failed");
        }
    }
}
