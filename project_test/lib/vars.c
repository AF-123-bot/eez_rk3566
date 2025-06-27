#include "vars.h"
#include "screens.h"
#include "lvgl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int32_t start_time_1;
int32_t start_time_2;
int32_t start_time_3;
int32_t stop_time_1;
int32_t stop_time_2;
int32_t stop_time_3;
int32_t sample_times_1;
int32_t sample_times_2;
int32_t sample_times_3;
int32_t tem;
char address[100] = { 0 };
char gateway[100] = { 0 };
char dns[100] = { 0 };
int32_t rotational_speed;
int32_t sample_motor_ccw_seconds;
int32_t sample_motor_cw_seconds;
int32_t sample_motor_stop_seconds;


int32_t get_var_tem() 
{
    return tem;
}

void set_var_tem(int32_t value) 
{
    tem = value;
}

const char *get_var_address() 
{
    return address;
}

void set_var_address(const char *value) 
{
    strncpy(address, value, sizeof(address) / sizeof(char));
    address[sizeof(address) / sizeof(char) - 1] = 0;
}

const char *get_var_dns() 
{
    return dns;
}

void set_var_dns(const char *value) 
{
    strncpy(dns, value, sizeof(dns) / sizeof(char));
    dns[sizeof(dns) / sizeof(char) - 1] = 0;
}

const char *get_var_gateway() 
{
    return gateway;
}

void set_var_gateway(const char *value) 
{
    strncpy(gateway, value, sizeof(gateway) / sizeof(char));
    gateway[sizeof(gateway) / sizeof(char) - 1] = 0;
}

int32_t get_var_rotational_speed() 
{
    return rotational_speed;
}

void set_var_rotational_speed(int32_t value) 
{
    rotational_speed = value;
}

int32_t get_var_start_time_1() 
{
    char buf[16];
    lv_dropdown_get_selected_str(objects.input_start_time1, buf, sizeof(buf));
    start_time_1 = atoi(buf);
    return start_time_1;
}

int32_t get_var_start_time_2() 
{
    char buf[16];
    lv_dropdown_get_selected_str(objects.input_start_time2, buf, sizeof(buf));
    start_time_2 = atoi(buf);
    return start_time_2;
}

int32_t get_var_start_time_3() 
{
    char buf[16];
    lv_dropdown_get_selected_str(objects.input_start_time3, buf, sizeof(buf));
    start_time_3 = atoi(buf);
    return start_time_3;
}

int32_t get_var_stop_time_1() 
{
    char buf[16];
    lv_dropdown_get_selected_str(objects.input_stop_time1, buf, sizeof(buf));
    stop_time_1 = atoi(buf);
    return stop_time_1;
}

int32_t get_var_stop_time_2() 
{
    char buf[16];
    lv_dropdown_get_selected_str(objects.input_stop_time2, buf, sizeof(buf));
    stop_time_2 = atoi(buf);
    return stop_time_2;
}

int32_t get_var_stop_time_3() 
{
    char buf[16];
    lv_dropdown_get_selected_str(objects.input_stop_time3, buf, sizeof(buf));
    stop_time_3 = atoi(buf);
    return stop_time_3;
}

void set_var_start_time_1(int32_t value) {
    start_time_1 = value;
    lv_dropdown_set_selected(objects.input_start_time1, (uint32_t)start_time_1);
}

void set_var_start_time_2(int32_t value) {
    start_time_2 = value;
    lv_dropdown_set_selected(objects.input_start_time2, (uint32_t)start_time_2);
}

void set_var_start_time_3(int32_t value) {
    start_time_3 = value;
    lv_dropdown_set_selected(objects.input_start_time3, (uint32_t)start_time_3);
}

void set_var_stop_time_1(int32_t value) 
{
    stop_time_1 = value;
    lv_dropdown_set_selected(objects.input_stop_time1, (uint32_t)stop_time_1);
}

void set_var_stop_time_2(int32_t value) 
{
    stop_time_2 = value;
    lv_dropdown_set_selected(objects.input_stop_time2, (uint32_t)stop_time_2);
}

void set_var_stop_time_3(int32_t value)
{
    stop_time_3 = value;
    lv_dropdown_set_selected(objects.input_stop_time3, (uint32_t)stop_time_3);
}

int32_t get_var_sample_motor_ccw_seconds() 
{
    return sample_motor_ccw_seconds;
}

void set_var_sample_motor_ccw_seconds(int32_t value) 
{
    sample_motor_ccw_seconds = value;
}

int32_t get_var_sample_motor_cw_seconds() 
{
    return sample_motor_cw_seconds;
}

void set_var_sample_motor_cw_seconds(int32_t value) 
{
    sample_motor_cw_seconds = value;
}

int32_t get_var_sample_motor_stop_seconds() 
{
    return sample_motor_stop_seconds;
}

void set_var_sample_motor_stop_seconds(int32_t value) 
{
    sample_motor_stop_seconds = value;
}

int32_t get_var_sample_times_1() 
{
    char buf[16];
    lv_dropdown_get_selected_str(objects.input_sample_times1, buf, sizeof(buf));
    sample_times_1 = atoi(buf);
    return sample_times_1;
}

void set_var_sample_times_1(int32_t value) 
{
    sample_times_1 = value;
    lv_dropdown_set_selected(objects.input_sample_times1, (uint32_t)sample_times_1);
}

int32_t get_var_sample_times_2() 
{
    char buf[16];
    lv_dropdown_get_selected_str(objects.input_sample_times2, buf, sizeof(buf));
    sample_times_2 = atoi(buf);
    return sample_times_2;
}

void set_var_sample_times_2(int32_t value) 
{
    sample_times_2 = value;
    lv_dropdown_set_selected(objects.input_sample_times2, (uint32_t)sample_times_2);
}

int32_t get_var_sample_times_3() 
{
    char buf[16];
    lv_dropdown_get_selected_str(objects.input_sample_times3, buf, sizeof(buf));
    sample_times_3 = atoi(buf);
    return sample_times_3;
}

void set_var_sample_times_3(int32_t value) 
{
    sample_times_3 = value;
    lv_dropdown_set_selected(objects.input_sample_times3, (uint32_t)sample_times_3);
}

