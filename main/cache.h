#define CACHE_OK 0
#define CACHE_EXPIRED 1
#define CACHE_NOT_FOUND 2

#define CACHE_ITEM_FIELD_VALUE "val"
#define CACHE_ITEM_FIELD_EXPIRATION "exp"

typedef struct {
    unsigned char status;
    double value;
} cache_item_t;

cache_item_t *cache_read(char *key);
void cache_write(char *key, double value, unsigned int ttl);
