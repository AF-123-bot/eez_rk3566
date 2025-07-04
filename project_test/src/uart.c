#include "uart.h"
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <errno.h>

int open_serial(const char *device, int baudrate) 
{
    int fd = open(device, O_RDWR | O_NOCTTY);
    if (fd < 0) {
        perror("打开串口失败");
        return -1;
    }

    struct termios tty;
    memset(&tty, 0, sizeof tty);

    if (tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr失败");
        close(fd);
        return -1;
    }

    // 设置波特率
    speed_t speed;
    switch (baudrate) {
        case 9600: speed = B9600; break;
        case 19200: speed = B19200; break;
        case 38400: speed = B38400; break;
        case 57600: speed = B57600; break;
        case 115200: speed = B115200; break;
        default:
            fprintf(stderr, "不支持的波特率，默认9600\n");
            speed = B9600;
    }
    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);

    // 8N1设置
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;     // 8位数据
    tty.c_cflag &= ~PARENB; // 无奇偶校验
    tty.c_cflag &= ~CSTOPB; // 1位停止位

    tty.c_cflag |= (CLOCAL | CREAD);  // 启动接收器，忽略调制解调器状态线
    tty.c_cflag &= ~CRTSCTS;           // 关闭硬件流控

    // 设置为原始模式（无特殊处理）
    tty.c_lflag = 0;
    tty.c_iflag = 0;
    tty.c_oflag = 0;

    // 设置读超时
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 10; // 1秒超时

    tcflush(fd, TCIOFLUSH);

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr失败");
        close(fd);
        return -1;
    }

    return fd;
}

int motor_forward(int fd, int speed) 
{
    /* 检查速度范围是否合法 */
    if (speed < 0 || speed > 40000) return -1;

    /* 构造正向控制指令数据包
     * 固定格式: 55 aa 06 09 [速度低字节] [速度高字节] 00 00 00 C3
     */
    unsigned char packet[10] = {
        0x55, 0xaa, 0x06, 0x09,          // 固定包头和指令
        (unsigned char)(speed & 0xFF), // 速度低字节
        (unsigned char)((speed >> 8) & 0xFF),     // 速度高字节
        0x00, 0x00, 0x00,                 // 保留位
        0xC3                              // 固定结束符(非校验和)
    };

    /* 发送数据包 */
    int n = write(fd, packet, sizeof(packet));
    if (n != sizeof(packet)) {
        perror("发送转盘正转指令失败");
        return -1;
    }
    tcdrain(fd);

    return 0;
}

int motor_reverse(int fd, int speed) 
{
    /* 检查速度范围是否合法 */
    if (speed < 0 || speed > 40000) return -1;

    /* 构造反向控制指令数据包
     * 固定格式: 55 AA 06 0A [速度低字节] [速度高字节] 00 00 00 C3
     */
    unsigned char packet[10] = {
        0x55, 0xAA, 0x06, 0x0A,          // 固定包头和指令
        (unsigned char)(speed & 0xFF), // 速度低字节
        (unsigned char)((speed >> 8) & 0xFF),    // 速度高字节
        0x00, 0x00, 0x00,                // 保留位
        0xC3                             // 固定结束符(非校验和)
    };

    /* 发送数据包 */
    int n = write(fd, packet, sizeof(packet));
    if (n != sizeof(packet)) {
        perror("发送转盘反转指令失败");
        return -1;
    }

    return 0;
}

// 电机绝对运动
// steps 等于运动到的绝对位置，一圈为180000 ， 则1/3为60000。2/3为120000
int motor_move_absolute(int fd, int speed, int32_t radius) 
{
    if (speed < 0 || speed > 40000) return -1;
    int32_t steps = radius * 1000;

    unsigned char packet[10] = {
        0x55, 0xAA, 0x07,                 // 固定包头和指令
        (unsigned char)(speed & 0xFF),    // 速度低字节
        (unsigned char)((speed >> 8) & 0xFF), // 速度高字节
        (unsigned char)(steps & 0xFF),            // 步数低字节
        (unsigned char)((steps >> 8) & 0xFF),
        (unsigned char)((steps >> 16) & 0xFF),
        (unsigned char)((steps >> 24) & 0xFF),
        0xC3                             // 固定结束符(非校验和)
    };


    int n = write(fd, packet, sizeof(packet));
    if (n != sizeof(packet)) {
        perror("发送电机绝对运动指令失败");
        return -1;
    }

    return 0;
}

// 电机增量运动
// steps 等于增加的运动距离，一圈为180000 ， 则增加1/3为60000。增加2/3为120000
int motor_move_incremental(int fd, int speed, int32_t radius) 
{
    if (speed < 0 || speed > 40000) return -1;

    int32_t steps = radius * 1000;

    unsigned char packet[10] = {
        0x55, 0xAA, 0x08,                 // 固定包头和指令
        (unsigned char)(speed & 0xFF),    // 速度低字节
        (unsigned char)((speed >> 8) & 0xFF), // 速度高字节
        (unsigned char)(steps & 0xFF),            // 步数低字节
        (unsigned char)((steps >> 8) & 0xFF),
        (unsigned char)((steps >> 16) & 0xFF),
        (unsigned char)((steps >> 24) & 0xFF),
        0xC3                             // 固定结束符(非校验和)
    };


    int n = write(fd, packet, sizeof(packet));
    if (n != sizeof(packet)) {
        perror("发送电机绝对运动指令失败");
        return -1;
    }

    return 0;
}

/**
 * @brief 发送电机“暂停运行”指令。
 * @param fd 串口文件描述符
 * @return 发送成功返回 0，失败返回 -1
 */
int motor_pause(int fd) 
{
    unsigned char packet[10] = {
        0x55, 0xAA, 0x02,
        0x00, 0x00, 0x00,
        0x00, 0x00, 0x00,
        0xC3
    };

    int n = write(fd, packet, sizeof(packet));
    if (n != sizeof(packet)) {
        perror("发送电机暂停指令失败");
        return -1;
    }

    return 0;
}

/**
 * @brief 电机回程序零位置
 * @param fd 串口文件描述符
 * @param speed 运动速度（400 ~ 40000）
 * @return 成功返回 0，失败返回 -1
 */
int motor_return_home(int fd, int speed)        //回程序零是回到开机时的地方
{
    if (speed < 0 || speed > 40000) return -1;

    unsigned char packet[10] = {
        0x55, 0xAA, 0x07,
        (unsigned char)(speed & 0xFF),        // 速度低字节
        (unsigned char)((speed >> 8) & 0xFF), // 速度高字节
        0x00, 0x00, 0x00, 0x00,               
        0xC3
    };

    int n = write(fd, packet, sizeof(packet));
    if (n != sizeof(packet)) {
        perror("发送回程序零指令失败");
        return -1;
    }

    return 0;
}

/**
 * @brief 控制电机回机械零位
 * @param fd 串口文件描述符
 * @param speed 运动速度（400 ~ 40000）
 * @param direction 方向（MOTOR_HOME_FORWARD 或 MOTOR_HOME_REVERSE）
 * @return 成功返回 0，失败返回 -1
 */
int motor_go_home(int fd, int speed, motor_home_direction_t direction)  //回机械零是回到红外扫描的地方
{
    if (speed < 0 || speed > 40000) return -1;

    unsigned char direction_code = (direction == MOTOR_HOME_FORWARD) ? 0x09 : 0x0A;

    unsigned char packet[10] = {
        0x55, 0xAA, 0x0B, direction_code,
        (unsigned char)(speed & 0xFF),        // 速度低字节
        (unsigned char)((speed >> 8) & 0xFF), // 速度高字节
        0x00, 0x00, 0x00,
        0xC3
    };

    int n = write(fd, packet, sizeof(packet));
    if (n != sizeof(packet)) {
        perror("发送回机械零指令失败");
        return -1;
    }

    return 0;
}

//motor_status_t status;   motor_get_status(fd, &status);
int motor_get_status(int fd, motor_status_t *status_out) 
{
    // 发送读取状态指令
    unsigned char cmd[10] = { 0x55, 0xAA, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC3 };
    int n = write(fd, cmd, sizeof(cmd));
    if (n != sizeof(cmd)) {
        perror("发送状态查询指令失败");
        return -1;
    }

    // 等待数据返回（可根据具体设备延时 10~50ms）
    usleep(20000);

    unsigned char buff[8];
    n = read(fd, buff, 8);
    if (n != 8) {
        perror("读取状态返回失败");
        return -1;
    }

    // 解析坐标（高位在前，低位在后）
    int32_t position = (buff[0] << 24) | (buff[1] << 16) | (buff[2] << 8) | buff[3];

    // 解析速度（timer 值，16 位，公式提供）
    uint16_t timer_val = (buff[4] << 8) | buff[5];
    int speed = 14745600 / (65535 + 46 - timer_val);

    // 输入状态位（第 6 个字节的 bit2~bit5）
    uint8_t input_flags = buff[6];
    bool limit_positive = !(input_flags & (1 << 2));
    bool limit_zero     = !(input_flags & (1 << 3));
    bool limit_negative = !(input_flags & (1 << 4));
    bool input_4        = !(input_flags & (1 << 5));

    // 工作状态
    uint8_t work_status = buff[7];

    // 保存到结构体
    if (status_out) {
        status_out->position = position;
        status_out->speed = speed;
        status_out->limit_positive = limit_positive;
        status_out->limit_zero = limit_zero;
        status_out->limit_negative = limit_negative;
        status_out->input_4 = input_4;
        status_out->work_status = work_status;
    }

    return 0;
}

// CRC计算函数
uint16_t modbus_crc16(const uint8_t *buf, int len) 
{
    uint16_t crc = 0xFFFF;
    for (int pos = 0; pos < len; pos++) {
        crc ^= (uint16_t)buf[pos];
        for (int i = 0; i < 8; i++) {
            if ((crc & 0x0001) != 0) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

// 读取温度函数  read_temperature(fd, &temp) = 0成功 =-1失败
int read_temperature(int fd, float *temperature) 
{
    // 构造请求帧：01 04 00 00 00 01 CRC低 CRC高
    uint8_t request[8] = {0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00};
    uint16_t crc = modbus_crc16(request, 6);
    request[6] = crc & 0xFF;         // CRC低位
    request[7] = (crc >> 8) & 0xFF;  // CRC高位

    // 发送请求
    if (write(fd, request, sizeof(request)) != sizeof(request)) {
        perror("发送Modbus温度请求失败");
        return -1;
    }

    // 接收响应：01 04 02 response CRC低 CRC高
    uint8_t response[7];
    int n = read(fd, response, sizeof(response));
    if (n != sizeof(response)) {
        perror("接收Modbus温度响应失败");
        return -1;
    }

    // 校验CRC
    uint16_t resp_crc = (response[6] << 8) | response[5];
    uint16_t calc_crc = modbus_crc16(response, 5);
    if (resp_crc != calc_crc) {
        fprintf(stderr, "CRC校验失败: 收到=0x%04X, 计算=0x%04X\n", resp_crc, calc_crc);
        return -1;
    }

    // 解析温度
    uint16_t raw = (response[3] << 8) | response[4];
    if (raw > 10000) {
        fprintf(stderr, "温度值异常: %u\n", raw);
        return -1;
    }

    *temperature = raw * 0.1f;
    return 0;
}

//读取湿度函数  read_humidity(fd, &humidity) =0 成功  =-1失败
int read_humidity(int fd, float *humidity) 
{
    // 构造请求帧：01 04 00 01 00 01 CRC低 CRC高
    uint8_t request[8] = {0x01, 0x04, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00};
    uint16_t crc = modbus_crc16(request, 6);
    request[6] = crc & 0xFF;         // CRC低位
    request[7] = (crc >> 8) & 0xFF;  // CRC高位

    // 发送请求
    if (write(fd, request, sizeof(request)) != sizeof(request)) {
        perror("发送Modbus湿度请求失败");
        return -1;
    }

    // 接收响应：01 04 02 response CRC低 CRC高
    uint8_t response[7];
    int n = read(fd, response, sizeof(response));
    if (n != sizeof(response)) {
        perror("接收Modbus湿度响应失败");
        return -1;
    }

    // 校验CRC
    uint16_t resp_crc = (response[6] << 8) | response[5];
    uint16_t calc_crc = modbus_crc16(response, 5);
    if (resp_crc != calc_crc) {
        fprintf(stderr, "CRC校验失败: 收到=0x%04X, 计算=0x%04X\n", resp_crc, calc_crc);
        return -1;
    }

    // 解析湿度
    uint16_t raw = (response[3] << 8) | response[4];
    if (raw > 10000) {
        fprintf(stderr, "湿度值异常: %u\n", raw);
        return -1;
    }

    *humidity = raw * 0.1f;
    return 0;
}

void close_serial(int fd) 
{
    close(fd);
}
