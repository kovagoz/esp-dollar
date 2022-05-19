#include "esp_err.h"
#include "esp_http_client.h"
#include "esp_tls.h"
#include "esp_log.h"
#include "http.h"

static const char *TAG = "http";
static http_response_t *response;

static esp_err_t http_event_handler(esp_http_client_event_t *evt)
{
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
            break;

        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;

        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;

        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;

        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);

            // Allocate memory for body in the response structure
            // Use calloc() to make sure that the terminal character is zero
            response->body = calloc(evt->data_len + 1, sizeof(char));
            memcpy(response->body, evt->data, evt->data_len);

            break;

        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");

            int mbedtls_err = 0;

            esp_err_t err = esp_tls_get_and_clear_last_error((esp_tls_error_handle_t) evt->data, &mbedtls_err, NULL);

            if (err != 0) {
                ESP_LOGI(TAG, "Last esp error code: 0x%x", err);
                ESP_LOGI(TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
            }

            break;

        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            break;

        case HTTP_EVENT_REDIRECT:
            ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
            break;
    }

    return ESP_OK;
}

http_response_t *http_exec(http_request_t *request)
{
    response = malloc(sizeof(http_response_t));

    esp_http_client_config_t config = {
        .url = request->url,
        .method = request->method ?: HTTP_METHOD_GET,
        .event_handler = http_event_handler,
        .disable_auto_redirect = true,
        .timeout_ms = request->timeout_ms ?: 500,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        response->status_code = esp_http_client_get_status_code(client);

        ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %lld",
            response->status_code,
            esp_http_client_get_content_length(client));
    } else {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);

    return response;
}
