#include <esp_sntp.h>

#define CLOCK_TZ_EUROPE_BUDAPEST "CET-1CEST,M3.5.0,M10.5.0/3"

void timezone_set(const char *timezone);
void ntp_sync();
