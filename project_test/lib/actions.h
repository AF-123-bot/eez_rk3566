#ifndef EEZ_LVGL_UI_EVENTS_H
#define EEZ_LVGL_UI_EVENTS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void action_save_setting_func(lv_event_t * e);
extern void action_btn_auto_take_cw(lv_event_t * e);
extern void action_user_shutdown_reboot(lv_event_t * e);
extern void action_btn_sample_motor(lv_event_t * e);
extern void action_btn_rotate_motor(lv_event_t * e);
extern void action_btn_factory_reset(lv_event_t * e);
extern void action_back_page(lv_event_t * e);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_EVENTS_H*/