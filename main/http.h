#ifndef HTTP_H
#define HTTP_H

#include "esp_http_client.h"

typedef esp_http_client_method_t http_method_t;
typedef esp_http_client_handle_t http_client_t;

typedef struct {
    char *body;
    int status_code;
} http_response_t;

http_client_t http_create_client(char *uri);
void http_set_header(http_client_t client, char *name, char *value);
http_response_t *http_exec(http_client_t client);

#endif
