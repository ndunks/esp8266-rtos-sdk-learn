#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/uart.h"

#define BUF_SIZE (1024)

volatile uint delayMs = 200U;

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

/**
 * Handle UART command
 */
void command_task(void *null)
{
    // Configure parameters of an UART driver,
    // communication pins and install the driver
    uart_config_t uart_config = {
        .baud_rate = CONFIG_ESPTOOLPY_MONITOR_BAUD,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_0, &uart_config);
    uart_driver_install(UART_NUM_0, BUF_SIZE * 2, 0, 0, NULL, 0);
    // Configure a temporary buffer for the incoming data
    uint8_t *buffer = (uint8_t *)malloc(BUF_SIZE);
    while (1)
    {
        // // Read data from the UART
        int len = uart_read_bytes(UART_NUM_0, buffer, BUF_SIZE, 20 / portTICK_PERIOD_MS);
        // // Write data back to the UART
        uart_write_bytes(UART_NUM_0, (const char *)buffer, len);
        //
    }
    vTaskDelete(NULL);
}

void app_main()
{

    configure_pins();
    printf("STDIN %d, STDOUT %d, STDERR %d\n", stdin->_file, stdout->_file, stderr->_file);

    xTaskCreate(&blink_task, "blink", 158, NULL, 1, NULL);
    xTaskCreate(&command_task, "command", 1000, NULL, 2, NULL);
}