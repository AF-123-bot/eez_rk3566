#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _groups_t {
    lv_group_t *left_button;
} groups_t;

extern groups_t groups;

void ui_create_groups();

typedef struct _objects_t {
    lv_obj_t *main_page;
    lv_obj_t *shift_setting_page;
    lv_obj_t *sample_setting_page;
    lv_obj_t *communicate_setting_page;
    lv_obj_t *motor_test_setting_page;
    lv_obj_t *historical_record_page;
    lv_obj_t *warning_page;
    lv_obj_t *setting8;
    lv_obj_t *btn_user_reboot;
    lv_obj_t *btn_user_shutdown;
    lv_obj_t *btn_user_confuse;
    lv_obj_t *btn_user;
    lv_obj_t *btn_start_all;
    lv_obj_t *obj0;
    lv_obj_t *btn_save_shift_setting;
    lv_obj_t *obj1;
    lv_obj_t *obj2;
    lv_obj_t *obj3;
    lv_obj_t *obj4;
    lv_obj_t *obj5;
    lv_obj_t *obj6;
    lv_obj_t *obj7;
    lv_obj_t *obj8;
    lv_obj_t *obj9;
    lv_obj_t *obj10;
    lv_obj_t *motor_ccw_seconds;
    lv_obj_t *motor_cw_seconds;
    lv_obj_t *motor_stop_seconds;
    lv_obj_t *btn_save_sample_setting;
    lv_obj_t *btn_manual_take_ccw;
    lv_obj_t *btn_manual_take_cw;
    lv_obj_t *btn_auto_take_cw;
    lv_obj_t *btn_sample_stop;
    lv_obj_t *obj11;
    lv_obj_t *obj12;
    lv_obj_t *obj13;
    lv_obj_t *obj14;
    lv_obj_t *obj15;
    lv_obj_t *obj16;
    lv_obj_t *input_address;
    lv_obj_t *input_gateway;
    lv_obj_t *input_dns;
    lv_obj_t *input_phonenum;
    lv_obj_t *btn_save_communicate_setting;
    lv_obj_t *obj17;
    lv_obj_t *obj18;
    lv_obj_t *obj19;
    lv_obj_t *obj20;
    lv_obj_t *obj21;
    lv_obj_t *obj22;
    lv_obj_t *btn_sample_motor_cw;
    lv_obj_t *btn_sample_motor_stop;
    lv_obj_t *btn_sample_motor_ccw;
    lv_obj_t *btn_rotate_motor_cw;
    lv_obj_t *btn_rotate_motor_ccw;
    lv_obj_t *btn_rotate_motor_stop;
    lv_obj_t *btn_rotate_motor_absolute_motion;
    lv_obj_t *btn_rotate_motor_incremental_motion;
    lv_obj_t *btn_rotate_motor_return_program0;
    lv_obj_t *btn_rotate_motor_return_mechanical0;
    lv_obj_t *input_rotational_speed;
    lv_obj_t *btn_save_motor_test_setting;
    lv_obj_t *input_rotational_radius;
    lv_obj_t *obj23;
    lv_obj_t *obj24;
    lv_obj_t *obj25;
    lv_obj_t *obj26;
    lv_obj_t *obj27;
    lv_obj_t *btn_warning_ok;
    lv_obj_t *main1;
    lv_obj_t *obj28;
    lv_obj_t *information;
    lv_obj_t *obj29;
    lv_obj_t *temperature;
    lv_obj_t *obj30;
    lv_obj_t *obj31;
    lv_obj_t *time;
    lv_obj_t *date;
    lv_obj_t *obj32;
    lv_obj_t *obj33;
    lv_obj_t *sample_motor_status;
    lv_obj_t *rotate_motor_status;
    lv_obj_t *barrel_1;
    lv_obj_t *obj34;
    lv_obj_t *barrel_2;
    lv_obj_t *obj35;
    lv_obj_t *barrel_3;
    lv_obj_t *obj36;
    lv_obj_t *main_label;
    lv_obj_t *sr_sure_container;
    lv_obj_t *title_2;
    lv_obj_t *button_left_shift;
    lv_obj_t *obj37;
    lv_obj_t *input_main_shift;
    lv_obj_t *obj38;
    lv_obj_t *obj39;
    lv_obj_t *obj40;
    lv_obj_t *input_start_time1;
    lv_obj_t *input_start_time2;
    lv_obj_t *input_stop_time3;
    lv_obj_t *input_start_time3;
    lv_obj_t *input_stop_time2;
    lv_obj_t *input_stop_time1;
    lv_obj_t *input_sample_times3;
    lv_obj_t *input_sample_times2;
    lv_obj_t *input_sample_times1;
    lv_obj_t *obj41;
    lv_obj_t *obj42;
    lv_obj_t *obj43;
    lv_obj_t *label_successful_shift;
    lv_obj_t *title_4;
    lv_obj_t *button_left_auto;
    lv_obj_t *obj44;
    lv_obj_t *input_auto_sampling;
    lv_obj_t *obj45;
    lv_obj_t *keyboard_sample;
    lv_obj_t *obj46;
    lv_obj_t *label_successful_sample;
    lv_obj_t *ccw_counting_time;
    lv_obj_t *cw_counting_time;
    lv_obj_t *tips_sample;
    lv_obj_t *title_3;
    lv_obj_t *button_left_communication;
    lv_obj_t *obj47;
    lv_obj_t *obj48;
    lv_obj_t *input_main_communication;
    lv_obj_t *keyboard_communication;
    lv_obj_t *label_successful_communication;
    lv_obj_t *title_1;
    lv_obj_t *button_left_test;
    lv_obj_t *obj49;
    lv_obj_t *input_main_test;
    lv_obj_t *label_successful_test;
    lv_obj_t *keyboard_test;
    lv_obj_t *btn_factory_reset;
    lv_obj_t *title_6;
    lv_obj_t *button_left_history;
    lv_obj_t *obj50;
    lv_obj_t *obj51;
    lv_obj_t *obj52;
    lv_obj_t *history_record;
    lv_obj_t *obj53;
    lv_obj_t *tem_chart;
    lv_obj_t *obj54;
    lv_obj_t *sampling_frequency_container;
    lv_obj_t *failure_number_container;
    lv_obj_t *title_5;
    lv_obj_t *warning_l;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN_PAGE = 1,
    SCREEN_ID_SHIFT_SETTING_PAGE = 2,
    SCREEN_ID_SAMPLE_SETTING_PAGE = 3,
    SCREEN_ID_COMMUNICATE_SETTING_PAGE = 4,
    SCREEN_ID_MOTOR_TEST_SETTING_PAGE = 5,
    SCREEN_ID_HISTORICAL_RECORD_PAGE = 6,
    SCREEN_ID_WARNING_PAGE = 7,
};

void create_screen_main_page();
void tick_screen_main_page();

void create_screen_shift_setting_page();
void tick_screen_shift_setting_page();

void create_screen_sample_setting_page();
void tick_screen_sample_setting_page();

void create_screen_communicate_setting_page();
void tick_screen_communicate_setting_page();

void create_screen_motor_test_setting_page();
void tick_screen_motor_test_setting_page();

void create_screen_historical_record_page();
void tick_screen_historical_record_page();

void create_screen_warning_page();
void tick_screen_warning_page();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/