#include "esp_log.h"
#include "esp_err.h"
#include "nvs.h"
#include "cache.h"

static const char *TAG = "cache";
static nvs_handle_t nvs_handler;

cache_result_t *cache_read(char *key)
{
    esp_err_t err;
    ESP_ERROR_CHECK(nvs_open("app", NVS_READWRITE, &nvs_handler));

    size_t size;
    err = nvs_get_str(nvs_handler, key, NULL, &size);

    cache_result_t *result = malloc(sizeof(cache_result_t));

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Found %s", key);
        result->found = CACHE_FOUND;
        result->value = malloc(size);
        ESP_ERROR_CHECK(nvs_get_str(nvs_handler, key, result->value, &size));
    }

    if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGI(TAG, "Not found %s", key);
        result->found = CACHE_NOT_FOUND;
    }

    nvs_close(nvs_handler);

    return result;
}
