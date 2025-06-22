#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <lvgl.h>

#include <SDL2/SDL.h>

extern int32_t start_time_1;
extern int32_t start_time_2;
extern int32_t start_time_3;
extern int32_t stop_time_1;
extern int32_t stop_time_2;
extern int32_t stop_time_3;
extern int32_t sample_times_1;
extern int32_t sample_times_2;
extern int32_t sample_times_3;
extern int32_t sample_motor_ccw_seconds;
extern int32_t sample_motor_cw_seconds;
extern int32_t sample_motor_stop_seconds;
extern int32_t rotational_speed;
extern char address[100];
extern char gateway[100];
extern char dns[100];



#endif /* MAIN_H */
