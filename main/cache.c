#include "esp_err.h"
#include "esp_log.h"
#include "nvs.h"

#define CACHE_NS  "app"
#define CACHE_KEY "usd"

static const char *TAG = "cache";

char *cache_read()
{
    ESP_LOGI(TAG, "Read cache");

    char *cache_item = NULL;
    nvs_handle_t nvs_handler;

    ESP_ERROR_CHECK(nvs_open(CACHE_NS, NVS_READONLY, &nvs_handler));

    size_t size;
    esp_err_t err = nvs_get_str(nvs_handler, CACHE_KEY, NULL, &size);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Cache found");

        cache_item = malloc(size);

        if (cache_item == NULL) {
            ESP_LOGE(TAG, "Fail to allocate memory for cache item");
            return NULL;
        }

        ESP_ERROR_CHECK(nvs_get_str(nvs_handler, CACHE_KEY, cache_item, &size));
    } else {
        ESP_LOGI(TAG, "Cache not found");
    }

    nvs_close(nvs_handler);

    return cache_item;
}

esp_err_t cache_write(char *data)
{
    ESP_LOGI(TAG, "Write cache");
    nvs_handle_t nvs_handler;
    ESP_ERROR_CHECK(nvs_open(CACHE_NS, NVS_READWRITE, &nvs_handler));
    ESP_ERROR_CHECK(nvs_set_str(nvs_handler, CACHE_KEY, data));
    nvs_close(nvs_handler);
    // TODO really handle the errors
    return ESP_OK;
}
