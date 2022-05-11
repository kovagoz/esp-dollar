#ifndef HTTP_H
#define HTTP_H

#include "esp_http_client.h"

typedef esp_http_client_method_t http_method_t;

typedef struct {
    const char *url;
    http_method_t method;
    int timeout_ms;
} http_request_t;

typedef struct {
    char *body;
    int status_code;
} http_response_t;

http_response_t *http_exec(http_request_t *request);

#endif
