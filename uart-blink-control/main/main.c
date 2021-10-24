#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "sdkconfig.h"
#include "esp_vfs.h"
#include "esp_vfs_dev.h"
#include "main.h"

static const char *TAG = "APP";
static volatile uint ledDelayMS = 1000;

void print_color(const char *cLOG_COLOR, const char *fmt, ...)
{
    va_list va;
    // if(fmt[0] == '\n'){
    //     putchar('\n');
    //     fmt++;
    // }
    printf(cLOG_COLOR);
    va_start(va, fmt);
    vprintf(fmt, va);
    va_end(va);
    printf(LOG_COLOR_END);
}

esp_err_t configure_uart()
{
    // Initialize VFS & UART so we can use std::cout/cin
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    /* Install UART driver for interrupt-driven reads and writes */
    ESP_ERROR_CHECK(uart_driver_install((uart_port_t)CONFIG_ESP_CONSOLE_UART_NUM,
                                        256, 0, 0, NULL, 0));
    /* Tell VFS to use UART driver, enable blockin read/write stdin/stdout */
    esp_vfs_dev_uart_use_driver(CONFIG_ESP_CONSOLE_UART_NUM);
    //esp_vfs_dev_uart_set_rx_line_endings(ESP_LINE_ENDINGS_CR);
    /* Move the caret to the beginning of the next line on '\n' */
    //esp_vfs_dev_uart_set_tx_line_endings(ESP_LINE_ENDINGS_CRLF);
    return ESP_OK;
}

esp_err_t configure_pins()
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
    return ESP_OK;
}

void blink_task(void *null)
{
    int state = 0;
    while (1)
    {
        // Delay and turn on
        vTaskDelay(ledDelayMS / portTICK_PERIOD_MS);
        gpio_set_level(GPIO_NUM_2, state ^= 1);
    }
    vTaskDelete(NULL);
}

/**
 * Handle UART Input
 */
void uart_task(void *null)
{
    uint buf_size = 5;
    volatile int index;
    int newDelay;
    bool task_is_active;
    char c, *buf = (char *)malloc(buf_size);
    TaskHandle_t task;
    eTaskState state;

    xTaskCreate(&blink_task, "blink", 1000, NULL, 1, &task);

    while (1)
    {
    retry:
        printf("Set Delay (%d): ", ledDelayMS);
        index = 0;
        for (;;)
        {
            c = getchar();
            // Echo back
            putchar(c);
            if (c == '\r')
            {
                // ignore CR
                continue;
            }
            else if (c == '\n')
            {
                buf[index] = 0;
                break;
            }
            else
            {
                if (index == buf_size - 1)
                {
                    print_color(LOG_COLOR_E, "\nMax Char is: %d\n", buf_size - 1);
                    goto retry;
                }
                if (c >= '0' && c <= '9')
                {
                    buf[index] = c;
                }
                else
                {
                    print_color(LOG_COLOR_E, "\nInput Only Number!\n");
                    goto retry;
                }
            }
            index++;
        }
        newDelay = atoi(buf);
        state = eTaskGetState(task);
        task_is_active = state != eSuspended;
        if (newDelay == 0)
        {
            if (task_is_active)
            {
                print_color(LOG_COLOR_I, "Stoping Blink..\n");
                vTaskSuspend(task);
            }
            else
            {
                print_color(LOG_COLOR_W, "Blink not running (%u)\n", state);
            }
        }
        else
        {
            ledDelayMS = newDelay;
            if (!task_is_active)
            {
                print_color(LOG_COLOR_I, "Starting Blink..\n");
                vTaskResume(task);
            }
        }
    }

    free(buf);
    buf = NULL;
    vTaskDelete(NULL);
}

void __attribute__((weak)) app_main()
{
    ESP_ERROR_CHECK(configure_pins());
    ESP_ERROR_CHECK(configure_uart());

    ESP_LOGI(TAG, "STDIN %d, STDOUT %d, STDERR %d\n", stdin->_file, stdout->_file, stderr->_file);

    xTaskCreate(&uart_task, "uart", 5000, NULL, 2, NULL);
}
