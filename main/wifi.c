#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <esp_event.h>
#include <esp_err.h>
#include <esp_log.h>
#include <esp_wifi.h>

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

static const char *TAG = "wifi";
static EventGroupHandle_t wifi_event_group;

static void wifi_event_listener(
    void *arg,
    esp_event_base_t event_base,
    int32_t event_id,
    void *event_data
) {
    // Station mode has been initialized successfully, let's get connected.
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        ESP_ERROR_CHECK(esp_wifi_connect());
    // Connection failed :(
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        // TODO retry
        xEventGroupSetBits(wifi_event_group, WIFI_FAIL_BIT);
    // We've got our IP address from the access point.
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

static void wifi_event_init()
{
    // The default event loop is an event loop the system uses to post and
    // handle events (e.g. Wi-Fi events).
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Listen for all WiFi events...
    ESP_ERROR_CHECK(esp_event_handler_register(
        WIFI_EVENT,
        ESP_EVENT_ANY_ID, // ...regardless of the event ID
        &wifi_event_listener,
        NULL // Handler args
    ));

    // Listen for the event fired when we get our IP address
    ESP_ERROR_CHECK(esp_event_handler_register(
        IP_EVENT,
        IP_EVENT_STA_GOT_IP,
        &wifi_event_listener,
        NULL // Handler args
    ));
}

esp_err_t wifi_connect()
{
    // We use this event group to signal the result of connection.
    wifi_event_group = xEventGroupCreate();

    wifi_event_init();

    // Network interface initialization
    ESP_ERROR_CHECK(esp_netif_init());
    esp_netif_create_default_wifi_sta();

    // Initialize the WiFi module
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // WiFi settings
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = CONFIG_ESP_WIFI_SSID,
            .password = CONFIG_ESP_WIFI_PASSWORD,
            // The weakest authmode to accept
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        }
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    // Waiting for the connection status
    EventBits_t status = xEventGroupWaitBits(
        wifi_event_group,
        WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
        pdTRUE, // xClearOnExit
        pdFALSE, // Wait until any of the event bits get set
        portMAX_DELAY // xTicksToWait
    );

    if (status & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "Connected");
        return ESP_OK;
    }

    ESP_LOGE(TAG, "Connection failed");

    return ESP_ERR_WIFI_NOT_CONNECT;
}
