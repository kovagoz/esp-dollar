#include "esp_err.h"

typedef struct {
    char *value;
    int expires;
} cache_result_t;

esp_err_t cache_read(char *currency, char **out);
