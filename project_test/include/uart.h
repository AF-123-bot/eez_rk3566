#ifndef UART_H
#define UART_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    MOTOR_HOME_FORWARD = 0,  // 朝正向
    MOTOR_HOME_REVERSE = 1   // 朝负向
} motor_home_direction_t;

typedef struct {
    int32_t position;     // 当前坐标
    int     speed;        // 当前设定速度
    bool    limit_positive;
    bool    limit_zero;
    bool    limit_negative;
    bool    input_4;
    uint8_t work_status;  // 工作状态原始值
} motor_status_t;

int open_serial(const char *device, int baudrate);
int set_serial_attr(int fd, int baudrate, int databits, char parity, int stopbits, int timeout_sec);
void close_serial(int fd);
int motor_forward(int fd, int speed);
int motor_reverse(int fd, int speed);
int motor_move_absolute(int fd, int speed, int32_t steps);
int motor_pause(int fd);
int motor_return_home(int fd, int speed);
int motor_go_home(int fd, int speed, motor_home_direction_t direction);
int motor_get_status(int fd, motor_status_t *status_out);

extern int uart_fd;  // 声明全局串口文件描述符

#ifdef __cplusplus
}
#endif

#endif
