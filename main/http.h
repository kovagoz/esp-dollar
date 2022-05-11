#ifndef HTTP_H
#define HTTP_H

typedef struct {
    const char *url;
    int timeout_ms;
} http_request_t;

typedef struct {
    char *body;
    int status_code;
} http_response_t;

http_response_t *http_get(http_request_t *request);

#endif
