#ifndef DATA_SAVE_LOAD_H
#define DATA_SAVE_LOAD_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// shift_page
void save_shift_data(
    int32_t start_time_1,
    int32_t start_time_2,
    int32_t start_time_3,
    int32_t stop_time_1,
    int32_t stop_time_2,
    int32_t stop_time_3,
    int32_t sample_times_1,
    int32_t sample_times_2,
    int32_t sample_times_3
);

void load_shift_data(
    int32_t *start_time_1,
    int32_t *start_time_2,
    int32_t *start_time_3,
    int32_t *stop_time_1,
    int32_t *stop_time_2,
    int32_t *stop_time_3,
    int32_t *sample_times_1,
    int32_t *sample_times_2,
    int32_t *sample_times_3
);

// sample_page
void save_sample_data(
    int32_t motor_stop_seconds,
    int32_t motor_cw_seconds,
    int32_t motor_ccw_seconds
);

void load_sample_data(
    int32_t *motor_stop_seconds,
    int32_t *motor_cw_seconds,
    int32_t *motor_ccw_seconds
);

// communicate_page
void save_communicate_data(const char *address, const char *gateway, const char *dns);
void load_communicate_data(char *address, char *gateway, char *dns, size_t max_len);

// test_page
void save_test_data(int32_t rotational_speed);
void load_test_data(int32_t *rotational_speed);

#ifdef __cplusplus
}
#endif

#endif
