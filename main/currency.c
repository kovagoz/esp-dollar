#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "esp_log.h"
#include "cJSON.h"
#include "cache.h"
#include "http.h"

static const char *TAG = "currency";

/**
 * Extract exchange rate from the HTTP response.
 */
static double extract_exchange_rate(http_response_t *response)
{
    cJSON *root = cJSON_Parse(response->body);
    double exchange_rate = cJSON_GetObjectItem(root, "result")->valuedouble;
    cJSON_Delete(root);

    return exchange_rate;
}

static double fetch_exchange_rate(char currency[3])
{
    return 365.14;

    double exchange_rate = -1;
    // TODO set the proper URL
    char uri[] = "http://kovi.local:8000/?amount=1&to=HUF&from=";

    strcat(uri, currency);

    http_client_t client = http_create_client(uri);
    // TODO read from sdkconfig
    http_set_header(client, "apikey", "...");
    http_response_t *response = http_exec(client);

    if (response->status_code == 200) {
        exchange_rate = extract_exchange_rate(response);
    } else {
        ESP_LOGE(TAG, "Something went wrong");
    }

    free(response->body);
    free(response);

    return exchange_rate;
}

double get_exchange_rate(char currency[3])
{
    cache_item_t *cache_item = cache_read(currency);

    if (cache_item->found) {
        return strtod(cache_item->value, NULL);
    }

    double exchange_rate = fetch_exchange_rate(currency);

    // TODO store in chache

    return exchange_rate;
}
