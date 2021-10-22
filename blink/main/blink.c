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
        gpio_set_level(GPIO_NUM_2, state);
        state ^= 1;
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

void main_task(void *null)
{
    volatile UBaseType_t i, taskCount;
    static TaskStatus_t *tasks;

    for (;;)
    {
        taskCount = uxTaskGetNumberOfTasks();
        tasks = pvPortMalloc(taskCount * sizeof(TaskStatus_t));
        uxTaskGetSystemState(tasks, taskCount, NULL);

        for (i = 0; i < taskCount; i++)
        {
            printf("%u %s %u %u %u %u\n",
                   tasks[i].xTaskNumber,
                   tasks[i].pcTaskName,
                   tasks[i].uxBasePriority,
                   tasks[i].uxCurrentPriority,
                   tasks[i].ulRunTimeCounter,
                   tasks[i].usStackHighWaterMark);
        }
        printf("Tasks: %u\n----------------\n", taskCount);
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}

void app_main()
{
    printf("100ms is %d ticks\n", pdMS_TO_TICKS(100));

    configure_pins();

    xTaskCreate(&blink_task, "blink", 158, NULL, 1, NULL);
    xTaskCreate(&main_task, "main", 1000, NULL, 2, NULL);
}