#include <time.h>
#include "esp_log.h"
#include "esp_err.h"
#include "nvs.h"
#include "cache.h"
#include "cJSON.h"

static const char *TAG = "cache";
static nvs_handle_t nvs_handler;

cache_item_t *cache_read(char *key)
{
    esp_err_t err;
    ESP_ERROR_CHECK(nvs_open("app", NVS_READONLY, &nvs_handler));

    // Get the length of the string item stored in the cache
    size_t size;
    err = nvs_get_str(nvs_handler, key, NULL, &size);

    cache_item_t *item = malloc(sizeof(cache_item_t));

    switch(err) {
        case ESP_OK:
            ESP_LOGI(TAG, "Found %s", key);

            char *json = malloc(size);
            ESP_ERROR_CHECK(nvs_get_str(nvs_handler, key, json, &size));

            cJSON *root = cJSON_Parse(json);
            item->value = cJSON_GetObjectItem(root, CACHE_ITEM_FIELD_VALUE)->valuedouble;
            double expires_at = cJSON_GetObjectItem(root, CACHE_ITEM_FIELD_EXPIRATION)->valuedouble;
            cJSON_Delete(root);

            item->status = time(NULL) > expires_at ? CACHE_EXPIRED : CACHE_OK;

            free(json);
            break;

        case ESP_ERR_NVS_NOT_FOUND:
            ESP_LOGI(TAG, "Not found %s", key);
            item->status = CACHE_NOT_FOUND;
            break;

        default:
            ESP_ERROR_CHECK(err);
    }

    nvs_close(nvs_handler);

    return item;
}

void cache_write(char *key, double value, unsigned int ttl)
{
    time_t expires_at = time(NULL) + ttl;

    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, CACHE_ITEM_FIELD_VALUE, value);
    cJSON_AddNumberToObject(root, CACHE_ITEM_FIELD_EXPIRATION, expires_at);

    ESP_ERROR_CHECK(nvs_open("app", NVS_READWRITE, &nvs_handler));
    ESP_ERROR_CHECK(nvs_set_str(nvs_handler, key, cJSON_Print(root)));
    nvs_close(nvs_handler);
}
