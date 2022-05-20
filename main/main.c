#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
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

    if (wifi_connect() == ESP_OK) {
        ESP_LOGI(TAG, "WiFi connected");
        xTaskCreate(&http_test_task, "http_test_task", 8192, NULL, 5, NULL);
    } else {
        ESP_LOGE(TAG, "WiFi connection failed");
    }
}
