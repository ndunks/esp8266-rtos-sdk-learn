#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "APP";

void __attribute__((weak)) app_main()
{
    ESP_LOGI(TAG, "App Started\n");

    // xTaskCreate(&mytask, "my", 5000, NULL, 2, NULL);
}
