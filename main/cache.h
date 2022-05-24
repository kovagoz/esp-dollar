#include "esp_err.h"

#define CACHE_FOUND 1
#define CACHE_NOT_FOUND 0

typedef struct {
    unsigned char found;
    char *value;
    int expires;
} cache_item_t;

cache_item_t *cache_read(char *key);
