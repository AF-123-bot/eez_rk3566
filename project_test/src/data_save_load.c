#include "data_save_load.h"
#include "screens.h"
#include "actions.h"
#include "fonts.h"
#include <stdio.h>
#include <lvgl.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>

extern objects_t objects;

void add_log_to_container_1(const char *message) 
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

void save_shift_data(
    int32_t start_time_1,
    int32_t start_time_2,
    int32_t start_time_3,
    int32_t stop_time_1,
    int32_t stop_time_2,
    int32_t stop_time_3,
    int32_t sample_times_1,
    int32_t sample_times_2,
    int32_t sample_times_3)
{
    const char *filepath = "/home/cat/eez_rk3566/data.json";
    FILE *fp = fopen(filepath, "r");
    char *file_content = NULL;
    long filesize = 0;
    cJSON *root = NULL;

    if (fp) {
        fseek(fp, 0, SEEK_END);
        filesize = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        file_content = (char *)malloc(filesize + 1);
        if (file_content) {
            fread(file_content, 1, filesize, fp);
            file_content[filesize] = '\0';
        }
        fclose(fp);

        if (file_content) {
            root = cJSON_Parse(file_content);
            free(file_content);
        }
    }

    if (!root) {
        root = cJSON_CreateObject();
    }

    cJSON *last_data = cJSON_GetObjectItem(root, "last_data");
    if (!last_data) {
        last_data = cJSON_CreateObject();
        cJSON_AddItemToObject(root, "last_data", last_data);
    }

    // 用数字替换或添加字段
    cJSON_ReplaceItemInObject(last_data, "start_time_1", cJSON_CreateNumber(start_time_1));
    cJSON_ReplaceItemInObject(last_data, "start_time_2", cJSON_CreateNumber(start_time_2));
    cJSON_ReplaceItemInObject(last_data, "start_time_3", cJSON_CreateNumber(start_time_3));

    cJSON_ReplaceItemInObject(last_data, "stop_time_1", cJSON_CreateNumber(stop_time_1));
    cJSON_ReplaceItemInObject(last_data, "stop_time_2", cJSON_CreateNumber(stop_time_2));
    cJSON_ReplaceItemInObject(last_data, "stop_time_3", cJSON_CreateNumber(stop_time_3));

    cJSON_ReplaceItemInObject(last_data, "sample_times_1", cJSON_CreateNumber(sample_times_1));
    cJSON_ReplaceItemInObject(last_data, "sample_times_2", cJSON_CreateNumber(sample_times_2));
    cJSON_ReplaceItemInObject(last_data, "sample_times_3", cJSON_CreateNumber(sample_times_3));

    char *json_str = cJSON_Print(root);
    fp = fopen(filepath, "w");
    if (fp) {
        fputs(json_str, fp);
        fclose(fp);
    } else {
        perror("无法写入文件");
    }

    free(json_str);
    cJSON_Delete(root);
}

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
) 
{
    FILE *fp = fopen("/home/cat/eez_rk3566/data.json", "r");
    if (!fp) {
        perror("无法打开文件");
        return;
    }

    fseek(fp, 0, SEEK_END);
    long length = ftell(fp);
    rewind(fp);

    char *data = (char *)malloc(length + 1);
    if (!data) {
        perror("内存分配失败");
        fclose(fp);
        return;
    }

    fread(data, 1, length, fp);
    data[length] = '\0';
    fclose(fp);

    cJSON *root = cJSON_Parse(data);
    free(data);
    if (!root) {
        fprintf(stderr, "JSON解析失败\n");
        return;
    }

    cJSON *last_data = cJSON_GetObjectItem(root, "last_data");
    if (!last_data) {
        fprintf(stderr, "找不到last_data对象\n");
        cJSON_Delete(root);
        return;
    }

    *start_time_1 = (int32_t)(cJSON_GetObjectItem(last_data, "start_time_1")->valuedouble);
    *start_time_2 = (int32_t)(cJSON_GetObjectItem(last_data, "start_time_2")->valuedouble);
    *start_time_3 = (int32_t)(cJSON_GetObjectItem(last_data, "start_time_3")->valuedouble);

    *stop_time_1 = (int32_t)(cJSON_GetObjectItem(last_data, "stop_time_1")->valuedouble);
    *stop_time_2 = (int32_t)(cJSON_GetObjectItem(last_data, "stop_time_2")->valuedouble);
    *stop_time_3 = (int32_t)(cJSON_GetObjectItem(last_data, "stop_time_3")->valuedouble);

    *sample_times_1 = (int32_t)(cJSON_GetObjectItem(last_data, "sample_times_1")->valuedouble);
    *sample_times_2 = (int32_t)(cJSON_GetObjectItem(last_data, "sample_times_2")->valuedouble);
    *sample_times_3 = (int32_t)(cJSON_GetObjectItem(last_data, "sample_times_3")->valuedouble);

    cJSON_Delete(root);
}

void save_sample_data(
    int32_t motor_stop_seconds,
    int32_t motor_cw_seconds,
    int32_t motor_ccw_seconds
) 
{
    FILE *fp = fopen("/home/cat/eez_rk3566/data.json", "r");
    cJSON *root = NULL;
    if (fp) {
        fseek(fp, 0, SEEK_END);
        long length = ftell(fp);
        rewind(fp);

        char *data = (char *)malloc(length + 1);
        fread(data, 1, length, fp);
        data[length] = '\0';
        fclose(fp);

        root = cJSON_Parse(data);
        free(data);
    }

    if (!root) {
        root = cJSON_CreateObject();
    }

    cJSON *last_data = cJSON_GetObjectItem(root, "last_data");
    if (!last_data) {
        last_data = cJSON_CreateObject();
        cJSON_AddItemToObject(root, "last_data", last_data);
    }

    cJSON_ReplaceItemInObject(last_data, "motor_cw_seconds", cJSON_CreateNumber(motor_cw_seconds));
    cJSON_ReplaceItemInObject(last_data, "motor_ccw_seconds", cJSON_CreateNumber(motor_ccw_seconds));
    cJSON_ReplaceItemInObject(last_data, "motor_stop_seconds", cJSON_CreateNumber(motor_stop_seconds));

    char *json_str = cJSON_Print(root);
    fp = fopen("/home/cat/eez_rk3566/data.json", "w");
    if (fp) {
        fputs(json_str, fp);
        fclose(fp);
    } else {
        perror("无法写入文件");
    }

    free(json_str);
    cJSON_Delete(root);
}

void load_sample_data(
    int32_t *motor_stop_seconds,
    int32_t *motor_cw_seconds,
    int32_t *motor_ccw_seconds
) 
{
    FILE *fp = fopen("/home/cat/eez_rk3566/data.json", "r");
    if (!fp) {
        perror("无法打开文件");
        return;
    }

    fseek(fp, 0, SEEK_END);
    long length = ftell(fp);
    rewind(fp);

    char *data = (char *)malloc(length + 1);
    if (!data) {
        perror("内存分配失败");
        fclose(fp);
        return;
    }

    fread(data, 1, length, fp);
    data[length] = '\0';
    fclose(fp);

    cJSON *root = cJSON_Parse(data);
    free(data);
    if (!root) {
        fprintf(stderr, "JSON解析失败\n");
        return;
    }

    cJSON *last_data = cJSON_GetObjectItem(root, "last_data");
    if (!last_data) {
        fprintf(stderr, "找不到last_data对象\n");
        cJSON_Delete(root);
        return;
    }

    cJSON *item;

    item = cJSON_GetObjectItem(last_data, "motor_stop_seconds");
    if (cJSON_IsNumber(item)) {
        *motor_stop_seconds = item->valueint;
    } else {
        *motor_stop_seconds = 0; // 或者设置默认值
    }

    item = cJSON_GetObjectItem(last_data, "motor_cw_seconds");
    if (cJSON_IsNumber(item)) {
        *motor_cw_seconds = item->valueint;
    } else {
        *motor_cw_seconds = 0;
    }

    item = cJSON_GetObjectItem(last_data, "motor_ccw_seconds");
    if (cJSON_IsNumber(item)) {
        *motor_ccw_seconds = item->valueint;
    } else {
        *motor_ccw_seconds = 0;
    }

    cJSON_Delete(root);
}

void save_communicate_data(const char *address, const char *gateway, const char *dns) 
{
    const char *filepath = "/home/cat/eez_rk3566/data.json";
    FILE *fp = fopen(filepath, "r");
    cJSON *root = NULL;

    if (fp) {
        fseek(fp, 0, SEEK_END);
        long length = ftell(fp);
        rewind(fp);

        char *data = (char *)malloc(length + 1);
        if (data) {
            fread(data, 1, length, fp);
            data[length] = '\0';
            root = cJSON_Parse(data);
            free(data);
        }
        fclose(fp);
    }

    if (!root) {
        root = cJSON_CreateObject();
    }

    cJSON *last_data = cJSON_GetObjectItem(root, "last_data");
    if (!last_data) {
        last_data = cJSON_CreateObject();
        cJSON_AddItemToObject(root, "last_data", last_data);
    }

    // 添加或更新 address、gateway、dns 字段
    cJSON_ReplaceItemInObject(last_data, "address", cJSON_CreateString(address));
    cJSON_ReplaceItemInObject(last_data, "gateway", cJSON_CreateString(gateway));
    cJSON_ReplaceItemInObject(last_data, "dns", cJSON_CreateString(dns));

    // 写回 JSON 文件
    char *json_str = cJSON_Print(root);
    fp = fopen(filepath, "w");
    if (fp) {
        fputs(json_str, fp);
        fclose(fp);
    } else {
        perror("无法写入文件");
    }

    free(json_str);
    cJSON_Delete(root);
}

void load_communicate_data(char *address, char *gateway, char *dns, size_t max_len) 
{
    const char *filepath = "/home/cat/eez_rk3566/data.json";
    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        perror("无法打开文件");
        return;
    }

    fseek(fp, 0, SEEK_END);
    long length = ftell(fp);
    rewind(fp);

    char *data = (char *)malloc(length + 1);
    if (!data) {
        perror("内存分配失败");
        fclose(fp);
        return;
    }

    fread(data, 1, length, fp);
    data[length] = '\0';
    fclose(fp);

    cJSON *root = cJSON_Parse(data);
    free(data);
    if (!root) {
        fprintf(stderr, "JSON 解析失败\n");
        return;
    }

    cJSON *last_data = cJSON_GetObjectItem(root, "last_data");
    if (!last_data) {
        fprintf(stderr, "找不到 last_data 对象\n");
        cJSON_Delete(root);
        return;
    }

    cJSON *addr_item = cJSON_GetObjectItem(last_data, "address");
    cJSON *gate_item = cJSON_GetObjectItem(last_data, "gateway");
    cJSON *dns_item  = cJSON_GetObjectItem(last_data, "dns");

    if (addr_item && cJSON_IsString(addr_item)) {
        strncpy(address, addr_item->valuestring, max_len - 1);
        address[max_len - 1] = '\0';
    }

    if (gate_item && cJSON_IsString(gate_item)) {
        strncpy(gateway, gate_item->valuestring, max_len - 1);
        gateway[max_len - 1] = '\0';
    }

    if (dns_item && cJSON_IsString(dns_item)) {
        strncpy(dns, dns_item->valuestring, max_len - 1);
        dns[max_len - 1] = '\0';
    }

    cJSON_Delete(root);
}

void save_test_data(int32_t rotational_speed, int32_t rotational_radius) 
{
    const char *filepath = "/home/cat/eez_rk3566/data.json";
    FILE *fp = fopen(filepath, "r");
    cJSON *root = NULL;

    if (fp) {
        fseek(fp, 0, SEEK_END);
        long length = ftell(fp);
        rewind(fp);

        char *data = (char *)malloc(length + 1);
        if (data) {
            fread(data, 1, length, fp);
            data[length] = '\0';
            root = cJSON_Parse(data);
            free(data);
        }
        fclose(fp);
    }

    if (!root) {
        root = cJSON_CreateObject();
    }

    cJSON *last_data = cJSON_GetObjectItem(root, "last_data");
    if (!last_data) {
        last_data = cJSON_CreateObject();
        cJSON_AddItemToObject(root, "last_data", last_data);
    }

    // 更新 rotational_speed
    if (cJSON_HasObjectItem(last_data, "rotational_speed")) {
        cJSON_ReplaceItemInObject(last_data, "rotational_speed", cJSON_CreateNumber(rotational_speed));
    } else {
        cJSON_AddNumberToObject(last_data, "rotational_speed", rotational_speed);
    }

    // 更新 rotational_radius
    if (cJSON_HasObjectItem(last_data, "rotational_radius")) {
        cJSON_ReplaceItemInObject(last_data, "rotational_radius", cJSON_CreateNumber(rotational_radius));
    } else {
        cJSON_AddNumberToObject(last_data, "rotational_radius", rotational_radius);
    }

    char *json_str = cJSON_Print(root);
    fp = fopen(filepath, "w");
    if (fp && json_str) {
        fputs(json_str, fp);
        fclose(fp);
    } else {
        perror("无法写入文件");
    }

    free(json_str);
    cJSON_Delete(root);
}

void load_test_data(int32_t *rotational_speed, int32_t *rotational_radius) 
{
    if (!rotational_speed || !rotational_radius) return;

    const char *filepath = "/home/cat/eez_rk3566/data.json";
    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        perror("无法打开文件");
        return;
    }

    fseek(fp, 0, SEEK_END);
    long length = ftell(fp);
    rewind(fp);

    char *data = (char *)malloc(length + 1);
    if (!data) {
        perror("内存分配失败");
        fclose(fp);
        return;
    }

    fread(data, 1, length, fp);
    data[length] = '\0';
    fclose(fp);

    cJSON *root = cJSON_Parse(data);
    free(data);
    if (!root) {
        fprintf(stderr, "JSON解析失败\n");
        return;
    }

    cJSON *last_data = cJSON_GetObjectItem(root, "last_data");
    if (!last_data) {
        fprintf(stderr, "未找到 last_data\n");
        cJSON_Delete(root);
        return;
    }

    cJSON *speed_item = cJSON_GetObjectItem(last_data, "rotational_speed");
    if (cJSON_IsNumber(speed_item)) {
        *rotational_speed = (int32_t)(speed_item->valuedouble);
    } else {
        fprintf(stderr, "rotational_speed 字段不存在或不是数字\n");
    }

    cJSON *radius_item = cJSON_GetObjectItem(last_data, "rotational_radius");
    if (cJSON_IsNumber(radius_item)) {
        *rotational_radius = (int32_t)(radius_item->valuedouble);
    } else {
        fprintf(stderr, "rotational_radius 字段不存在或不是数字\n");
    }

    cJSON_Delete(root);
}


void save_logs_to_json()
{
    const char *filepath = "/home/cat/eez_rk3566/data.json";
    FILE *fp = fopen(filepath, "r");
    cJSON *root = NULL;

    printf("[保存] 尝试打开 JSON 文件：%s\n", filepath);

    if (fp) {
        fseek(fp, 0, SEEK_END);
        long length = ftell(fp);
        rewind(fp);

        char *data = (char *)malloc(length + 1);
        if (data) {
            fread(data, 1, length, fp);
            data[length] = '\0';
            root = cJSON_Parse(data);
            free(data);
            printf("[保存] 读取文件成功，大小：%ld 字节\n", length);
        } else {
            printf("[保存] 内存分配失败\n");
        }
        fclose(fp);
    }

    if (!root) {
        printf("[保存] 文件为空或解析失败，创建新 JSON 对象\n");
        root = cJSON_CreateObject();
    }

    // 创建新的 logs 数组
    cJSON *logs = cJSON_CreateArray();

    uint32_t count = lv_obj_get_child_cnt(objects.sampling_frequency_container);
    int logs_added = 0;
    for (uint32_t i = 0; i < count; ++i) {
        lv_obj_t *child = lv_obj_get_child(objects.sampling_frequency_container, i);
        if (lv_obj_check_type(child, &lv_label_class)) {
            const char *text = lv_label_get_text(child);
            if (text && strlen(text) > 0) {
                cJSON_AddItemToArray(logs, cJSON_CreateString(text));
                logs_added++;
            }
        }
    }
    printf("[保存] 从容器中采集到日志项数量：%d\n", logs_added);

    // 替换 root 中的 logs 数组（如果不存在也会新增）
    cJSON_ReplaceItemInObject(root, "logs", logs);

    // 格式化输出（带缩进换行）
    char *json_str = cJSON_PrintBuffered(root, 8192, 1);

    if (!json_str) {
        printf("[保存] cJSON_PrintBuffered 失败\n");
        cJSON_Delete(root);
        return;
    }

    // 写回文件（覆盖）
    fp = fopen(filepath, "w");
    if (fp) {
        fputs(json_str, fp);
        fflush(fp);
        int fd = fileno(fp);
        fsync(fd);
        fclose(fp);
        printf("[保存] 写入 JSON 文件成功，路径：%s\n", filepath);
    } else {
        perror("[保存] 打开文件写入失败");
    }

    free(json_str);
    cJSON_Delete(root);
}

void load_logs_from_json()
{
    const char *filepath = "/home/cat/eez_rk3566/data.json";
    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        perror("[读取] 打开日志文件失败");
        return;
    }

    fseek(fp, 0, SEEK_END);
    long length = ftell(fp);
    rewind(fp);

    char *data = (char *)malloc(length + 1);
    if (!data) {
        perror("[读取] 内存分配失败");
        fclose(fp);
        return;
    }

    fread(data, 1, length, fp);
    data[length] = '\0';
    fclose(fp);

    printf("[读取] JSON 文件读取成功，大小：%ld 字节\n", length);

    cJSON *root = cJSON_Parse(data);
    free(data);

    if (!root) {
        fprintf(stderr, "[读取] JSON 解析失败\n");
        return;
    }

    cJSON *logs = cJSON_GetObjectItem(root, "logs");
    if (logs && cJSON_IsArray(logs)) {
        int size = cJSON_GetArraySize(logs);
        printf("[读取] 读取到日志数组，数量：%d\n", size);

        // 先清空 UI 容器旧内容
        lv_obj_clean(objects.sampling_frequency_container);

        for (int i = 0; i < size; ++i) {
            cJSON *log_item = cJSON_GetArrayItem(logs, i);
            if (cJSON_IsString(log_item)) {
                printf("[读取] 添加日志项：%s\n", log_item->valuestring);
                add_log_to_container_1(log_item->valuestring);
            }
        }
    } else {
        fprintf(stderr, "[读取] logs 字段为空或格式不正确\n");
    }

    cJSON_Delete(root);
}