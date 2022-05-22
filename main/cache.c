#include "esp_log.h"
#include "esp_err.h"
#include "nvs.h"

static const char *TAG = "cache";
static nvs_handle_t nvs_handler;

esp_err_t cache_read(char *key, char **out)
{
    esp_err_t err;
    ESP_ERROR_CHECK(nvs_open("app", NVS_READWRITE, &nvs_handler));

    size_t size;
    err = nvs_get_str(nvs_handler, key, NULL, &size);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Found %s", key);
        char *out = malloc(size);
        ESP_ERROR_CHECK(nvs_get_str(nvs_handler, key, out, &size));
    }

    if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGI(TAG, "Not found %s", key);
    }

    nvs_close(nvs_handler);

    return err;
}
