#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "esp_err.h"
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "cJSON.h"
#include "cache.h"
#include "clock.h"
#include "http.h"
#include "led.h"
#include "wifi.h"

static const char *TAG = "main";

typedef struct {
    double rate;
    unsigned int timestamp;
} exchange_t;

/**
 * Parse the HTTP response body received from APILayer.
 *
 * @return exchange_t* NULL if JSON parsing is failed
 */
exchange_t *parse_data(char *json)
{
    cJSON *root = cJSON_Parse(json);

    if (root == NULL) {
        const char *err = cJSON_GetErrorPtr();

        if (err != NULL) {
            ESP_LOGE(TAG, "Error before: %s", err);
        }
    } else {
        exchange_t *exchange = malloc(sizeof(exchange_t));
        exchange->rate = cJSON_GetObjectItem(root, "result")->valuedouble;
        cJSON *info = cJSON_GetObjectItem(root, "info");
        exchange->timestamp = cJSON_GetObjectItem(info, "timestamp")->valueint;
        cJSON_Delete(root);

        return exchange;
    }

    return NULL;
}

/**
 * Prepare the non-volatile storage to use.
 */
static void nvs_init()
{
    esp_err_t err = nvs_flash_init();

    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    ESP_ERROR_CHECK(err);
}

TaskHandle_t spinner_task;

void app_main(void)
{
    xTaskCreate(led_spinner_task, "LED spinner", 3072, NULL, configMAX_PRIORITIES / 2, &spinner_task);

    exchange_t *exchange = NULL;

    nvs_init();

    char *data = cache_read();

    if (data != NULL) {
        exchange = parse_data(data);
    }

    if (wifi_connect() == ESP_OK) {
        if (exchange != NULL) {
            ntp_sync(); // Need the accurate time to check cache expiration
        }

        // If no cache or is expired, try to update
        if (exchange == NULL || time(NULL) - exchange->timestamp > 3600) {
            ESP_LOGI(TAG, "Fetch recent data");

            if (data != NULL) {
                free(data); // Drop the data read from cache before
            }

            data = http_fetch_data();

            if (data != NULL) {
                cache_write(data);
                exchange = parse_data(data);
            }
        } else {
            ESP_LOGI(TAG, "Cache is up to date");
        }
    }

    vTaskDelete(spinner_task);
    led_clear();

    if (exchange != NULL) {
        // Use stale cache if network is not available
        ESP_LOGI(TAG, "1 USD = %f HUF", exchange->rate);
        led_show_number(50 * exchange->rate);
    } else {
        led_show_text("ERROR");
    }
}
