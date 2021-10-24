#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

void blink_task(void *null)
{
    int state = 0;
    while (1)
    {
        // Delay and turn on
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        printf("BLINK... %d\n", state);
        gpio_set_level(GPIO_NUM_2, state ^= 1);
    }
    vTaskDelete(NULL);
}

void configure_pins()
{
    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO2
    io_conf.pin_bit_mask = 1ULL << GPIO_NUM_2;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
}

void app_main()
{
    printf("100ms is %d ticks\n", pdMS_TO_TICKS(100));
    configure_pins();
    xTaskCreate(&blink_task, "blink", 800, NULL, 1, NULL);
}