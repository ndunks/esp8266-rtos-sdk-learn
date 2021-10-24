# ESP8266 RTOS SDK Learning Journey for Beginner

The journey of learning ESP8266. I switched from Non-OS (Arduino) SDK to RTOS (FreeRTOS) SDK, because I want to use official SDK that espressif support.

## First Impressions

- RTOS SDK (not yet) beginner friendly, you must know build system, toolchain, and C macro.
while Arduino IDE manything is handled by IDE.
- Its simple. no matter what IDE you use, you just need SDK, toolchain, and command line.

## Development Setup

There's many way to setup SDK, you can follow [official instruction](https://github.com/espressif/ESP8266_RTOS_SDK) too (but you need to change some options). when you can build & flash hello-world project (make all, make flash) without error, its mean your setup its OK.

in my Linux (Debian) machine, my setup is:

- Clone the [SDK Repo](https://github.com/espressif/ESP8266_RTOS_SDK/tree/release/v3.4) to `$HOME/app/ESP8266_RTOS_SDK`
- Download [toolchain](https://dl.espressif.com/dl/xtensa-lx106-elf-gcc8_4_0-esp-2020r3-linux-amd64.tar.gz) to `$HOME/app/xtensa-lx106-elf`
- edit .bashrc add `export IDF_PATH="$HOME/app/ESP8266_RTOS_SDK"` and `export PATH="$PATH:$HOME/app/xtensa-lx106-elf"`
- Logout & login or reinvoke `. bashrc`

## IDE
I use Visual Studio Code, there's an official extension that suport ESP-IDF. Bu I can't find that fit to ESP8266-RTOS-SDK, so I made an extension that do simple thing:

- Add command to call make (build & monitor) with custom argument
- And kill the last run program (if any) when monitoring serial.

So I just need one-key or one-click to rebuild and monitor result.
You can find it here: [build-monitor-code-extension](https://github.com/ndunks/build-monitor-code-extension)

## Learning Key
After just try to run [hello-world](hello-world) project, there's key that you need to learn in the next:

### FreeRTOS
Learn FreeRTOS is very fundamental, you must know about task. Official documentation is very good for read: ["Mastering the FreeRTOS Real Time Kernel - a Hands On Tutorial Guide"](https://www.freertos.org/Documentation/RTOS_book.html)


### Blink (basic GPIO)

to set GPIO Mode, you must call `gpio_config` with `gpio_config_t` struct param. GPIO set using bit mode, so we can set multiple GPIO pins at once.

to write GPIO output as high or low, we need to call `gpio_set_level`


### Task

Even in single processor, RTOS run multitasking/multithread out of the box. but lowering the tick to 100Hz by default. in that 100Hz RTOS can switch to any task that need to run (based on priority) and leave the task that is IDLE or in call of `vTaskDelay` or waiting external event (buffer, ISR, etc). so, calling delay or do blocking call in Task that have priority `5` mean allowing another task with priority `5` or bellow to run.
see my exploration about it here:
- [task](task)
- [uart-blink-control](uart-blink-control)


### UART Console

in RTOS SDK, you can use standar C function to print (`printf`) or read message from console. Console
in UART by default is non-blocking mode. if you want to do more like reading input, you must setup it as blocking mode by calling `esp_vfs_dev_uart_use_driver`, so your task must wait for buffer arrive. [Read Here for More](https://github.com/espressif/ESP8266_RTOS_SDK/tree/master/components/vfs#standard-io-streams-stdin-stdout-stderr).
see my exploration about it here:
- [uart-blink-control](uart-blink-control)
