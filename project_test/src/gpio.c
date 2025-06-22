#include "gpio.h"
#include <gpiod.h>
#include <stdio.h>
#include <stdlib.h>

struct gpio_handle dir_gpio = {0};   // GPIO1_B0 => 8
struct gpio_handle en_gpio  = {0};   // GPIO1_A4 => 4

int gpio_init(struct gpio_handle *handle, const char *chipname, unsigned int line_num) {
    handle->chip = gpiod_chip_open_by_name(chipname);
    if (!handle->chip) {
        perror("Open chip failed");
        return -1;
    }

    handle->line = gpiod_chip_get_line(handle->chip, line_num);
    if (!handle->line) {
        perror("获取引脚失败");
        gpiod_chip_close(handle->chip);
        return -1;
    }

    if (gpiod_line_request_output(handle->line, "gpio_control", 0) < 0) {
        perror("设置引脚为输出失败");
        gpiod_chip_close(handle->chip);
        return -1;
    }

    return 0;
}

int gpio_set_value(struct gpio_handle *handle, int value) {
    if (!handle->line) {
        fprintf(stderr, "引脚未初始化\n");
        return -1;
    }
    return gpiod_line_set_value(handle->line, value);
}

void gpio_release(struct gpio_handle *handle) {
    if (handle->line) {
        gpiod_line_release(handle->line);
        handle->line = NULL;
    }
    if (handle->chip) {
        gpiod_chip_close(handle->chip);
        handle->chip = NULL;
    }
}
