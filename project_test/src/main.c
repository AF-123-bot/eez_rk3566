#include "main.h"
#include "gpio.h"
#include "uart.h"
#include "ui.h"
#include "screens.h"
#include "vars.h"
#include "data_save_load.h"
#include <lvgl.h>

static lv_obj_t * loading_bar = NULL;
static lv_obj_t * loading_label = NULL;
static lv_timer_t * loading_timer = NULL;

static int dot_count = 0;
static int loading_progress = 0;

static lv_display_t * hal_init(int32_t w, int32_t h);

void update_time_cb(lv_timer_t *timer);
void lvgl_data_init(void);
void show_startup_screen(void);
void show_startup_screen(void);

bool sample_motor_status = true;
bool rotate_motor_status = true;

int main() {
    setenv("SDL_VIDEODRIVER", "x11", 1);
    setenv("SDL_RENDER_DRIVER", "opengl", 1);

    if (gpio_init(&dir_gpio, "gpiochip1", 8) < 0) sample_motor_status = false;
    if (gpio_init(&en_gpio, "gpiochip1", 4) < 0) sample_motor_status = false;

    lv_init();
    printf("LVGL 初始化完成\n");
    hal_init(1024, 768);
    ui_init();
    printf("UI 初始化完成\n");

    show_startup_screen();  // 显示加载动画界面

    lv_timer_create(update_time_cb, 1000, NULL);  // 每秒更新时间显示

    while (1) 
    {
        lv_timer_handler();
        usleep(5000);
        ui_tick();
    }

    return 0;
}

/**
 * Initialize the Hardware Abstraction Layer (HAL) for the LVGL graphics
 * library using DRM/KMS (Direct Rendering Manager / Kernel Mode Setting)
 */
static lv_display_t * hal_init(int32_t w, int32_t h)
{

  lv_group_set_default(lv_group_create());

  lv_display_t * disp = lv_sdl_window_create(w, h);

  lv_indev_t * mouse = lv_sdl_mouse_create();
  lv_indev_set_group(mouse, lv_group_get_default());
  lv_indev_set_display(mouse, disp);
  lv_display_set_default(disp);

  // LV_IMAGE_DECLARE(mouse_cursor_icon); /*Declare the image file.*/
  // lv_obj_t * cursor_obj;
  // cursor_obj = lv_image_create(lv_screen_active()); /*Create an image object for the cursor */
  // lv_image_set_src(cursor_obj, &mouse_cursor_icon);           /*Set the image source*/
  // lv_indev_set_cursor(mouse, cursor_obj);             /*Connect the image  object to the driver*/

// 打开键鼠
//   lv_indev_t * mousewheel = lv_sdl_mousewheel_create();
//   lv_indev_set_display(mousewheel, disp);
//   lv_indev_set_group(mousewheel, lv_group_get_default());

//   lv_indev_t * kb = lv_sdl_keyboard_create();
//   lv_indev_set_display(kb, disp);
//   lv_indev_set_group(kb, lv_group_get_default());

  return disp;
}

void update_time_cb(lv_timer_t *timer)
{
    (void) timer;

    static int last_min = -1;
    static int last_hour = -1;
    static int last_day = -1;
    static int last_month = -1;
    static int last_year = -1;

    time_t raw_time;
    struct tm *time_info;

    time(&raw_time);
    time_info = localtime(&raw_time);

    int hour = time_info->tm_hour;
    int min = time_info->tm_min;
    int day = time_info->tm_mday;
    int month = time_info->tm_mon + 1;
    int year = time_info->tm_year + 1900;

    // 只在变化时更新变量，避免闪烁
    if (year != last_year || month != last_month || day != last_day) 
    {
        char date_buf[32];
        snprintf(date_buf, sizeof(date_buf), "%04d . %02d . %02d", year, month, day);

        lv_label_set_text(objects.date, date_buf);

        last_year = year;
        last_month = month;
        last_day = day;
    }
    if (hour != last_hour || min != last_min) 
    {
        char time_buf[16];
        snprintf(time_buf, sizeof(time_buf), "%02d : %02d", hour, min);
        lv_label_set_text(objects.time, time_buf);
        last_hour = hour;
        last_min = min;
    }
}

void lvgl_data_init(void)
{
    //确定电机状态
    if(sample_motor_status == false)    //采样电机故障
    {
        lv_obj_set_style_text_color(objects.sample_motor_status, lv_color_hex(0xFF0000), 0);
        lv_label_set_text(objects.sample_motor_status,"故障");
    }
    if(rotate_motor_status == false)    //旋转电机故障
    {
        lv_obj_set_style_text_color(objects.rotate_motor_status, lv_color_hex(0xFF0000), 0);
        lv_label_set_text(objects.rotate_motor_status,"故障");
    }
    //读取shift页面上一次保存的数据
    load_shift_data(&start_time_1, &start_time_2, &start_time_3, &stop_time_1, &stop_time_2, &stop_time_3, &sample_times_1, &sample_times_2, &sample_times_3);
    set_var_start_time_1(start_time_1);
    set_var_start_time_2(start_time_2);
    set_var_start_time_3(start_time_3);
    set_var_stop_time_1(stop_time_1);
    set_var_stop_time_2(stop_time_2);
    set_var_stop_time_3(stop_time_3);
    set_var_sample_times_1(sample_times_1);
    set_var_sample_times_2(sample_times_2);
    set_var_sample_times_3(sample_times_3);
    //读取sample页面上一次保存的数据
    load_sample_data(&sample_motor_stop_seconds,&sample_motor_cw_seconds,&sample_motor_ccw_seconds);
    set_var_sample_motor_ccw_seconds(sample_motor_ccw_seconds);
    set_var_sample_motor_cw_seconds(sample_motor_cw_seconds);
    set_var_sample_motor_stop_seconds(sample_motor_stop_seconds);
    //读取communicate页面上一次保存的数据
    load_communicate_data(&address,&gateway,&dns,100);
    set_var_address(address);
    set_var_dns(dns);
    set_var_gateway(gateway);
    //读取motor_test页面上一次保存的数据
    load_test_data(&rotational_speed,&rotational_radius);
    set_var_rotational_speed(rotational_speed);
    set_var_rotational_radius(rotational_radius);
    //读取历史记录数据
    // load_logs_from_json();
}

static void loading_timer_cb(lv_timer_t * timer)
{
    dot_count++;
    if(dot_count > 3) dot_count = 1;

    static char buf[16];
    snprintf(buf, sizeof(buf), "loading%.*s", dot_count, "...");
    lv_label_set_text(loading_label, buf);

    loading_progress += 10;
    if (loading_progress > 100) loading_progress = 100;
    lv_bar_set_value(loading_bar, loading_progress, LV_ANIM_ON);

    if (loading_progress >= 100) {
        lv_scr_load(objects.main_page);
        lvgl_data_init();  // 主界面加载后初始化数据
        lv_timer_del(timer);
    }
}

void show_startup_screen(void)
{
    lv_obj_t * scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(scr, lv_color_black(), 0);
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * logo = lv_label_create(scr);
    lv_label_set_text(logo, "Lubancat RK3566 starting...");
    lv_obj_set_style_text_color(logo, lv_color_white(), 0);
    lv_obj_set_style_text_font(logo, &lv_font_montserrat_24, 0);
    lv_obj_align(logo, LV_ALIGN_TOP_MID, 0, 60);

    loading_bar = lv_bar_create(scr);
    lv_obj_set_size(loading_bar, 300, 20);
    lv_obj_align(loading_bar, LV_ALIGN_CENTER, 0, 20);
    lv_bar_set_range(loading_bar, 0, 100);
    lv_bar_set_value(loading_bar, 0, LV_ANIM_OFF);

    loading_label = lv_label_create(scr);
    lv_label_set_text(loading_label, "loading");
    lv_obj_set_style_text_color(loading_label, lv_color_white(), 0);
    lv_obj_align(loading_label, LV_ALIGN_CENTER, 0, 50);

    lv_scr_load(scr);

    loading_progress = 0;
    dot_count = 0;

    if (loading_timer) lv_timer_del(loading_timer);
    loading_timer = lv_timer_create(loading_timer_cb, 500, NULL);
}