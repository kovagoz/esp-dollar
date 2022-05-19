#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "wifi.h"
#include "http.h"
#include "cJSON.h"

static const char *TAG = "app";

static void http_test_task(void *pvParameters)
{
    // TODO set auth header
    http_request_t request = {
        .url = "http://kovi.local:8000/",
        .timeout_ms = 3000,
    };

    http_response_t *response = http_exec(&request);

    if (response->status_code == 200) {
        // Extract the exchange rate from the HTTP response
        cJSON *root = cJSON_Parse(response->body);
        double exchange_rate = cJSON_GetObjectItem(root, "result")->valuedouble;
        cJSON_Delete(root);

        ESP_LOGI(TAG, "1 USD = %f HUF", exchange_rate);

        // Calculate how much HUF the 50 USD is worth
        int result = (int) round(exchange_rate * 50);

        // Zero means error (problem with the atof() conversion)
        if (result == 0) {
            ESP_LOGE(TAG, "Failed to fetch the exchange rate");
        } else {
            ESP_LOGI(TAG, "50 USD to HUF = %i", result);
        }
    } else {
        ESP_LOGE(TAG, "Something went wrong");
    }

    free(response->body);
    free(response);

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
