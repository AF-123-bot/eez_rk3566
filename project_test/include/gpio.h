#ifndef GPIO_H
#define GPIO_H

#ifdef __cplusplus
extern "C" {
#endif


struct gpio_handle {
    struct gpiod_chip *chip;
    struct gpiod_line *line;
};

// 全局变量声明（在 gpio.c 中定义）
extern struct gpio_handle dir_gpio;
extern struct gpio_handle en_gpio;

// 函数声明
int gpio_init(struct gpio_handle *handle, const char *chipname, unsigned int line_num);
int gpio_set_value(struct gpio_handle *handle, int value);
void gpio_release(struct gpio_handle *handle);


#ifdef __cplusplus
}
#endif

#endif // GPIO_H
