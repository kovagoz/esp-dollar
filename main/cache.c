#include "esp_log.h"
#include "esp_err.h"
#include "nvs.h"
#include "cache.h"

static const char *TAG = "cache";
static nvs_handle_t nvs_handler;

cache_item_t *cache_read(char *key)
{
    esp_err_t err;
    ESP_ERROR_CHECK(nvs_open("app", NVS_READWRITE, &nvs_handler));

    size_t size;
    err = nvs_get_str(nvs_handler, key, NULL, &size);

    cache_item_t *item = malloc(sizeof(cache_item_t));

    switch(err) {
        case ESP_OK:
            ESP_LOGI(TAG, "Found %s", key);
            item->found = CACHE_FOUND;
            item->value = malloc(size);
            ESP_ERROR_CHECK(nvs_get_str(nvs_handler, key, item->value, &size));
            break;

        case ESP_ERR_NVS_NOT_FOUND:
            ESP_LOGI(TAG, "Not found %s", key);
            item->found = CACHE_NOT_FOUND;
            break;

        default:
            ESP_ERROR_CHECK(err);
    }

    nvs_close(nvs_handler);

    return item;
}
