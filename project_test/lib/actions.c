#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>
#include <lvgl.h>
#include <string.h>
#include <unistd.h>
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "screens.h"
#include "gpio.h"
#include "uart.h"
#include "data_save_load.h"

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

volatile bool motor_thread_stop_flag = false;   //线程标志
int32_t sample_motor_flag = 0; // 0电机空闲中 ， 1电机运行中
extern int uart_fd;  // 声明全局串口文件描述符
enum ScreensEnum current_page = 0;

void *motor_thread_func(void *arg); // 声明电机线程函数

// 定时器处理函数声明
static lv_timer_t *manual_timer_cw = NULL;
static lv_timer_t *manual_timer_ccw = NULL;
static int manual_time_cw = 0;
static int manual_time_ccw = 0;

// 手动清灰和取灰的定时器计数
static void manual_cw_timer_cb(lv_timer_t * timer)
{
    manual_time_cw++;

    int hours = manual_time_cw / 3600;
    int minutes = (manual_time_cw % 3600) / 60;
    int seconds = manual_time_cw % 60;

    char buf[128];
    if (hours > 0)
        snprintf(buf, sizeof(buf), "手动取灰已执行%d小时%d分%d秒\n推荐时间为30秒", hours, minutes, seconds);
    else if (minutes > 0)
        snprintf(buf, sizeof(buf), "手动取灰已执行%d分%d秒\n推荐时间为30秒", minutes, seconds);
    else
        snprintf(buf, sizeof(buf), "手动取灰已执行%d秒\n推荐时间为30秒", seconds);

    lv_label_set_text(objects.tips_sample, buf);
}

static void manual_ccw_timer_cb(lv_timer_t * timer)
{
    manual_time_ccw++;

    int hours = manual_time_ccw / 3600;
    int minutes = (manual_time_ccw % 3600) / 60;
    int seconds = manual_time_ccw % 60;

    char buf[128];
    if (hours > 0)
        snprintf(buf, sizeof(buf), "手动清灰已执行%d小时%d分%d秒\n推荐时间为30秒", hours, minutes, seconds);
    else if (minutes > 0)
        snprintf(buf, sizeof(buf), "手动清灰已执行%d分%d秒\n推荐时间为30秒", minutes, seconds);
    else
        snprintf(buf, sizeof(buf), "手动清灰已执行%d秒\n推荐时间为30秒", seconds);

    lv_label_set_text(objects.tips_sample, buf);
}

enum ScreensEnum judge_page(lv_obj_t *parent)
{
    while (parent != NULL) {
        if (parent == objects.main_page) {
            return SCREEN_ID_MAIN_PAGE;
        } else if (parent == objects.shift_setting_page) {
            return SCREEN_ID_SHIFT_SETTING_PAGE;
        } else if (parent == objects.sample_setting_page) {
            return SCREEN_ID_SAMPLE_SETTING_PAGE;
        } else if (parent == objects.communicate_setting_page) {
            return SCREEN_ID_COMMUNICATE_SETTING_PAGE;
        } else if (parent == objects.motor_test_setting_page) {
            return SCREEN_ID_MOTOR_TEST_SETTING_PAGE;
        }
        parent = lv_obj_get_parent(parent);
    }
    return 0;  // 未匹配任何页面
}

void show_warning_page(const char *msg, lv_obj_t *parent) 
{
    current_page = judge_page(parent);
    lv_label_set_text(objects.warning_l, msg);
    lv_scr_load(objects.warning_page);
}

void add_log_to_container(const char *message) 
{
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char time_buf[32];
    strftime(time_buf, sizeof(time_buf), "[%Y.%m.%d %H:%M] ", tm_info);

    char full_msg[256];
    snprintf(full_msg, sizeof(full_msg), "%s%s", time_buf, message);

    // 创建新的 label
    lv_obj_t *label = lv_label_create(objects.sampling_frequency_container);
    lv_label_set_text(label, full_msg);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(label, lv_obj_get_width(objects.sampling_frequency_container));
    lv_obj_set_style_text_font(label, &ui_font_24, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(label, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_margin_bottom(label, 5, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ⚠️ 将 label 移动到最前面（顶部）
    lv_obj_move_to_index(label, 0);

    // 限制最大日志数为 100（删除最后一个，即最旧的）
    uint32_t count = lv_obj_get_child_cnt(objects.sampling_frequency_container);
    if (count > 100) {
        lv_obj_t *last = lv_obj_get_child(objects.sampling_frequency_container, count - 1);
        lv_obj_del(last);
    }

    // 滚动到顶部显示最新日志
    lv_obj_scroll_to_y(objects.sampling_frequency_container, 0, LV_ANIM_OFF);
}

// 检查某班时间是否有效，不满足就弹警告，返回1表示有警告，需要停止后续判断
int check_shift_time(int start_time, int stop_time, int shift_index, lv_obj_t *parent) 
{
    if (start_time == 0 || stop_time == 0) {
        char buf[64];
        snprintf(buf, sizeof(buf), "警告:请设置第%d班时间！", shift_index);
        show_warning_page(buf, parent);
        return 1;
    }
    if (stop_time < start_time) {
        show_warning_page("警告:结束时间必须晚于\n开始时间！", parent);
        return 1;
    }
    return 0;
}

void action_back_page(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_PRESSED)
    {
        switch(current_page)
        {
            case SCREEN_ID_MAIN_PAGE:
                lv_scr_load(objects.main_page);
                break;
            case SCREEN_ID_SHIFT_SETTING_PAGE:
                lv_scr_load(objects.shift_setting_page);
                break;
            case SCREEN_ID_SAMPLE_SETTING_PAGE:
                lv_scr_load(objects.sample_setting_page);
                break;
            case SCREEN_ID_COMMUNICATE_SETTING_PAGE:
                lv_scr_load(objects.communicate_setting_page);
                break;
            case SCREEN_ID_MOTOR_TEST_SETTING_PAGE:
                lv_scr_load(objects.motor_test_setting_page);
                break;
            case SCREEN_ID_HISTORICAL_RECORD_PAGE:
                lv_scr_load(objects.historical_record_page);
                break;
            default:
                // 默认跳转，比如跳到主页面
                lv_scr_load(objects.main_page);
                break;
        }
        current_page = 0;
    }
}

void action_save_setting_func(lv_event_t * e) 
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    lv_obj_t *parent = lv_obj_get_parent(obj);      //得到当前页面
    if(code == LV_EVENT_PRESSED)    //检测到是按钮按下
    {
        if(obj == objects.btn_save_shift_setting)    //shift_setting页面保存触发
        {
            start_time_1 = get_var_start_time_1();
            start_time_2 = get_var_start_time_2();
            start_time_3 = get_var_start_time_3();
            stop_time_1 = get_var_stop_time_1();
            stop_time_2 = get_var_stop_time_2();
            stop_time_3 = get_var_stop_time_3();
            sample_times_1 = get_var_sample_times_1();
            sample_times_2 = get_var_sample_times_2();
            sample_times_3 = get_var_sample_times_3();

            // 全部均为0
            if (start_time_1 == 0 && stop_time_1 == 0 && sample_times_1 == 0 &&
                start_time_2 == 0 && stop_time_2 == 0 && sample_times_2 == 0 &&
                start_time_3 == 0 && stop_time_3 == 0 && sample_times_3 == 0)
            {
                show_warning_page("警告:至少填写一班的时\n间设置！", parent);
                return;
            }

            // 检查每一班的时间是否合法
            if ((start_time_1 != 0 || stop_time_1 != 0) && check_shift_time(start_time_1, stop_time_1, 1, parent)) return;
            if ((start_time_2 != 0 || stop_time_2 != 0) && check_shift_time(start_time_2, stop_time_2, 2, parent)) return;
            if ((start_time_3 != 0 || stop_time_3 != 0) && check_shift_time(start_time_3, stop_time_3, 3, parent)) return;

            // 开始时间和结束时间填写后，取样次数的检查
            for (int i = 1; i <= 3; i++) 
            {
                int start_time = (i == 1) ? start_time_1 : (i == 2) ? start_time_2 : start_time_3;
                int stop_time  = (i == 1) ? stop_time_1  : (i == 2) ? stop_time_2  : stop_time_3;
                int sample     = (i == 1) ? sample_times_1 : (i == 2) ? sample_times_2 : sample_times_3;

                if ((start_time != 0 || stop_time != 0) && sample == 0) {
                    char buf[64];
                    snprintf(buf, sizeof(buf), "警告:请填写第%d班取样\n次数！", i);
                    show_warning_page(buf, parent);
                    return;
                }
            }

            // 班次之间的检查 
            if ((stop_time_1 != 0 && start_time_2 != 0) && (stop_time_1 > start_time_2)) 
            {
                show_warning_page("警告:结束时间必须早于\n下一班开始时间！", parent);
                return;
            }
            if ((stop_time_2 != 0 && start_time_3 != 0) && (stop_time_2 > start_time_3)) 
            {
                show_warning_page("警告:结束时间必须早于\n下一班开始时间！", parent);
                return;
            }

            //检查通过，执行保存
            save_shift_data(start_time_1,start_time_2,start_time_3,stop_time_1,stop_time_2,stop_time_3,sample_times_1,sample_times_2,sample_times_3); //保存shift页面的数据
        }
        else if(obj == objects.btn_save_sample_setting)     //sample_setting页面保存触发
        {
            sample_motor_ccw_seconds = get_var_sample_motor_ccw_seconds();
            sample_motor_cw_seconds = get_var_sample_motor_cw_seconds();
            sample_motor_stop_seconds = get_var_sample_motor_stop_seconds();
            
            save_sample_data(sample_motor_stop_seconds,sample_motor_cw_seconds,sample_motor_ccw_seconds);   //保存sample页面内容
        }
        else if(obj == objects.btn_save_communicate_setting)        //communicate_setting页面保存触发
        {
            strcpy(address, get_var_address());
            strcpy(dns, get_var_dns());
            strcpy(gateway, get_var_gateway());

            save_communicate_data(address,gateway,dns);
        }
        else if(obj == objects.btn_save_motor_test_setting)         //motor_test_setting页面保存触发
        {
            rotational_speed = get_var_rotational_speed();
            
            save_test_data(rotational_speed);   //保存test页面内容
        }
    }
}

void action_user_shutdown_reboot(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if(code == LV_EVENT_PRESSED)
    {
        if(obj == objects.btn_user)
        {
            lv_obj_clear_flag(objects.sr_sure_container, LV_OBJ_FLAG_HIDDEN);   //显示确认是否关机界面
            // 将页面控件都disabled 
            lv_obj_add_state(objects.btn_user, LV_STATE_DISABLED);
            lv_obj_add_state(objects.btn_factory_reset, LV_STATE_DISABLED);
            lv_obj_add_state(objects.setting8, LV_STATE_DISABLED);
            /////////////////////////////////////////////////////////////////////
        }
        else if(obj == objects.btn_user_confuse)
        {
            lv_obj_add_flag(objects.sr_sure_container, LV_OBJ_FLAG_HIDDEN);     //隐藏界面
            // 将页面空间abled
            lv_obj_clear_state(objects.btn_user, LV_STATE_DISABLED);
            lv_obj_clear_state(objects.btn_factory_reset, LV_STATE_DISABLED);
            lv_obj_clear_state(objects.setting8, LV_STATE_DISABLED);
        }
        else if(obj == objects.btn_user_reboot)
        {
            // save_logs_to_json();
            system("reboot");
        }
        else if(obj == objects.btn_user_shutdown)
        {
            // save_logs_to_json();
            system("shutdown -h now");
        }
    }
}

void action_btn_factory_reset(lv_event_t * e)
{
    (void)e;
    //初始化shift页面数据
    save_shift_data(0, 0, 0, 0, 0, 0, 0, 0, 0);
    set_var_start_time_1(0);
    set_var_start_time_2(0);
    set_var_start_time_3(0);
    set_var_stop_time_1(0);
    set_var_stop_time_2(0);
    set_var_stop_time_3(0);
    set_var_sample_times_1(0);
    set_var_sample_times_2(0);
    set_var_sample_times_3(0);
    //初始化sample页面数据
    save_sample_data(0,0,0);
    set_var_sample_motor_ccw_seconds(0);
    set_var_sample_motor_cw_seconds(0);
    set_var_sample_motor_stop_seconds(0);
    //初始化communicate页面数据

    //初始化test页面数据
    save_test_data(0);
    set_var_rotational_speed(0);
}

void action_btn_sample_motor(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if(code == LV_EVENT_PRESSED)
    {
        if(obj == objects.btn_sample_motor_cw && sample_motor_flag == 0)   //进行电机正转操作
        {
            sample_motor_flag = 1;
            add_log_to_container("执行了一次采样电机正转");
            lv_obj_add_state(objects.btn_sample_motor_ccw, LV_STATE_DISABLED);   //将反转按钮设为disabled
            gpio_set_value(&en_gpio,1);     //使能电机
            gpio_set_value(&dir_gpio,0);    //设置电机正转
        }
        else if(obj == objects.btn_sample_motor_ccw && sample_motor_flag == 0)   //进行电机反转操作
        {
            sample_motor_flag = 1;
            add_log_to_container("执行了一次采样电机反转");
            lv_obj_add_state(objects.btn_sample_motor_cw, LV_STATE_DISABLED);   //将手动取灰按钮设为disabled
            gpio_set_value(&en_gpio,1);     //使能电机
            gpio_set_value(&dir_gpio,1);    //设置电机反转
        }
        else if(obj == objects.btn_manual_take_cw && sample_motor_flag == 0)
        {
            sample_motor_flag = 1;

            lv_obj_clear_flag(objects.tips_sample, LV_OBJ_FLAG_HIDDEN); // 显示提示
            lv_obj_add_state(objects.btn_manual_take_ccw, LV_STATE_DISABLED);   //将手动清灰按钮设为disabled
            manual_time_cw = 0;
            lv_label_set_text(objects.tips_sample, "手动取灰已执行0秒\n推荐时间为30秒");

            // 启动定时器，每秒更新
            if (manual_timer_cw) lv_timer_del(manual_timer_cw);
            manual_timer_cw = lv_timer_create(manual_cw_timer_cb, 1000, NULL);

            gpio_set_value(&en_gpio,1);     // 使能电机
            gpio_set_value(&dir_gpio,0);    // 正转
        }

        else if(obj == objects.btn_manual_take_ccw && sample_motor_flag == 0)
        {
            sample_motor_flag = 1;

            lv_obj_clear_flag(objects.tips_sample, LV_OBJ_FLAG_HIDDEN); // 显示提示
            lv_obj_add_state(objects.btn_manual_take_cw, LV_STATE_DISABLED);   //将手动取灰按钮设为disabled
            manual_time_ccw = 0;
            lv_label_set_text(objects.tips_sample, "手动清灰已执行0秒\n推荐时间为30秒");

            // 启动定时器，每秒更新
            if (manual_timer_ccw) lv_timer_del(manual_timer_ccw);
            manual_timer_ccw = lv_timer_create(manual_ccw_timer_cb, 1000, NULL);

            gpio_set_value(&en_gpio,1);     // 使能电机
            gpio_set_value(&dir_gpio,1);    // 反转
        }
        else if((obj == objects.btn_sample_stop && sample_motor_flag == 1) || (obj == objects.btn_sample_motor_stop && sample_motor_flag == 1))       //采样电机停止操作
        {
            sample_motor_flag = 0;

            if (manual_timer_cw) 
            {
                lv_timer_del(manual_timer_cw);
                manual_timer_cw = NULL;

                char log_buf[64];
                snprintf(log_buf, sizeof(log_buf), "执行了%d秒手动取灰", manual_time_cw);
                add_log_to_container(log_buf);
                lv_obj_add_flag(objects.tips_sample, LV_OBJ_FLAG_HIDDEN); // 关闭提示
                manual_time_cw = 0;
            }

            if (manual_timer_ccw) 
            {
                lv_timer_del(manual_timer_ccw);
                manual_timer_ccw = NULL;

                char log_buf[64];
                snprintf(log_buf, sizeof(log_buf), "执行了%d秒手动清灰", manual_time_ccw);
                add_log_to_container(log_buf);
                lv_obj_add_flag(objects.tips_sample, LV_OBJ_FLAG_HIDDEN); // 关闭提示
                manual_time_ccw = 0;
            }

            add_log_to_container("执行了一次电机停止");
            motor_thread_stop_flag = true;
            lv_obj_clear_state(objects.btn_manual_take_cw, LV_STATE_DISABLED);
            lv_obj_clear_state(objects.btn_manual_take_ccw, LV_STATE_DISABLED);
            lv_obj_clear_state(objects.btn_sample_motor_cw, LV_STATE_DISABLED);
            lv_obj_clear_state(objects.btn_sample_motor_ccw, LV_STATE_DISABLED);
            lv_obj_clear_state(objects.btn_auto_take_cw, LV_STATE_DISABLED);
            gpio_set_value(&en_gpio,0);     //关闭电机
            gpio_set_value(&dir_gpio,0);
        }
    }
}

void action_btn_auto_take_cw(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if(code == LV_EVENT_PRESSED)
    {
        if(obj == objects.btn_auto_take_cw)
        {
            sample_motor_ccw_seconds = get_var_sample_motor_ccw_seconds();
            sample_motor_cw_seconds = get_var_sample_motor_cw_seconds();
            sample_motor_stop_seconds = get_var_sample_motor_stop_seconds();
            motor_thread_stop_flag = false;
            pthread_t motor_thread;
            if (pthread_create(&motor_thread, NULL, motor_thread_func, NULL) != 0) {
                perror("pthread_create failed");
            } else {
                pthread_detach(motor_thread); // 不阻塞主线程
            }
        }
    }
}

void *motor_thread_func(void *arg)  //自动取灰线程
{
    printf(">> 电机自动取灰启动\n");

    lv_obj_add_state(objects.btn_manual_take_cw, LV_STATE_DISABLED);   //将手动取灰按钮设为disabled
    lv_obj_add_state(objects.btn_sample_motor_cw, LV_STATE_DISABLED);   //将手动取灰按钮设为disabled
    lv_obj_add_state(objects.btn_manual_take_ccw, LV_STATE_DISABLED);   //将手动取灰按钮设为disabled
    lv_obj_add_state(objects.btn_sample_motor_ccw, LV_STATE_DISABLED);   //将手动取灰按钮设为disabled
    lv_obj_add_state(objects.btn_auto_take_cw, LV_STATE_DISABLED);   //将自动取灰按钮设为disabled

    sample_motor_flag = 1;
    time_t start_time = time(NULL);  // 获取当前时间，作为第一个阶段计时起点
    time_t now;
    int elapsed = 0;    //已经经过的秒数

    // 电机控制状态机四个阶段：
    enum { PHASE_CCW, PHASE_STOP, PHASE_CW, PHASE_DONE } phase = PHASE_CCW;
    bool ccw_started = false, stop_started = false, cw_started = false;
    while (phase != PHASE_DONE && !motor_thread_stop_flag)
    {
        now = time(NULL);
        elapsed = now - start_time;

        if (motor_thread_stop_flag == true)     //检测是否按下停止按钮
        {
            gpio_set_value(&en_gpio, 0);
            gpio_set_value(&dir_gpio, 0);
            printf(">> 电机自动取灰线程收到停止请求，立即退出\n");
            break;
        }

        switch (phase)
        {
            case PHASE_CCW:
                if (!ccw_started) {
                    gpio_set_value(&en_gpio, 1); // 使能
                    gpio_set_value(&dir_gpio, 1); // 逆时针
                    printf(">> 阶段 1：逆时针启动 (%d 秒)\n", sample_motor_ccw_seconds);
                    ccw_started = true;
                }
                if (elapsed >= sample_motor_ccw_seconds) {
                    gpio_set_value(&en_gpio, 0); // 停止电机
                    gpio_set_value(&dir_gpio, 0);
                    phase = PHASE_STOP;
                    start_time = time(NULL); // 重置起点时间
                }
                break;

            case PHASE_STOP:
                if (!stop_started) {
                    gpio_set_value(&en_gpio, 0); // 保持停止
                    gpio_set_value(&dir_gpio, 0);
                    printf(">> 阶段 2：停止 (%d 秒)\n", sample_motor_stop_seconds);
                    stop_started = true;
                }
                if (elapsed >= sample_motor_stop_seconds) {
                    phase = PHASE_CW;
                    start_time = time(NULL);
                }
                break;

            case PHASE_CW:
                if (!cw_started) {
                    gpio_set_value(&en_gpio, 1); // 启动
                    gpio_set_value(&dir_gpio, 0); // 顺时针
                    printf(">> 阶段 3：顺时针启动 (%d 秒)\n", sample_motor_cw_seconds);
                    cw_started = true;
                }
                if (elapsed >= sample_motor_cw_seconds) {
                    gpio_set_value(&en_gpio, 0); // 关闭
                    gpio_set_value(&dir_gpio, 0);
                    phase = PHASE_DONE;
                }
                break;

            default:
                break;
        }

        usleep(100 * 1000); // 每 100ms 检查一次，降低CPU占用
    }

    motor_thread_stop_flag = false;
    sample_motor_flag = 0;
    char auto_log[128];
    snprintf(auto_log, sizeof(auto_log),
            "执行了一次自动采样 正转时间为%ds，反转时间为%ds，停止时间为%ds",
            sample_motor_cw_seconds, sample_motor_ccw_seconds, sample_motor_stop_seconds);
    add_log_to_container(auto_log);
    lv_obj_clear_state(objects.btn_manual_take_cw, LV_STATE_DISABLED);
    lv_obj_clear_state(objects.btn_manual_take_ccw, LV_STATE_DISABLED);
    lv_obj_clear_state(objects.btn_sample_motor_cw, LV_STATE_DISABLED);
    lv_obj_clear_state(objects.btn_sample_motor_ccw, LV_STATE_DISABLED);
    lv_obj_clear_state(objects.btn_auto_take_cw, LV_STATE_DISABLED);   //将自动取灰按钮设为disabled
    printf(">> 电机自动取灰完成\n");
    pthread_exit(NULL);
}

void action_btn_rotate_motor(lv_event_t * e)    //旋转电机测试按钮
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if(code == LV_EVENT_PRESSED)
    {
        if(obj == objects.btn_rotate_motor_cw)      //执行旋转电机正转操作
        {
            lv_obj_add_state(objects.btn_rotate_motor_ccw, LV_STATE_DISABLED);   //将反转按钮设为disabled
            rotational_speed = get_var_rotational_speed();
            motor_forward(uart_fd,rotational_speed);        //发送旋转电机反转指令
        }
        else if(obj == objects.btn_rotate_motor_ccw)        //执行旋转电机反转操作
        {
            lv_obj_add_state(objects.btn_rotate_motor_cw, LV_STATE_DISABLED);   //将正转按钮设为disabled
            rotational_speed = get_var_rotational_speed();
            motor_reverse(uart_fd,rotational_speed);
        }
        else if(obj == objects.btn_rotate_motor_stop)       //执行旋转电机停止操作
        {
            lv_obj_clear_state(objects.btn_rotate_motor_cw, LV_STATE_DISABLED);   //将正转按钮设为abled
            lv_obj_clear_state(objects.btn_rotate_motor_ccw, LV_STATE_DISABLED);   //将反转按钮设为abled
            motor_pause(uart_fd);
        }
    }
}