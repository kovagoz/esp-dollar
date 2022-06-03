/**
 * Show spinning animation on each digit.
 */
void led_spinner_task(void *pvParameters);

/**
 * Turn off all segments on all digits.
 */
void led_clear();

/**
 * Display the integer part of a floating point number.
 */
void led_show_number(double num);

/**
 * Display text.
 */
void led_show_text(char *message);
