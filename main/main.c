#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "wifi.h"
#include "http.h"

static const char *TAG = "app";

static void http_test_task(void *pvParameters)
{
    http_request_t request = {
        .url = "http://raspberrypi.local/usd",
        .timeout_ms = 3000,
    };

    http_response_t *response = http_exec(&request);

    if (response->status_code == 200) {
        ESP_LOGI(TAG, "|%s|", response->body);
    } else {
        ESP_LOGE(TAG, "Something went wrong");
    }

    free(response);

    // Zero means error
//    int result = (int) round(atof(local_response_buffer) * 50);
//
//    ESP_LOGI(TAG, "USD to HUF: %i", result);

    ESP_LOGI(TAG, "Finish http example");
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
