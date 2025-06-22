#include "data_save_load.h"
#include <stdio.h>
#include <stdlib.h>
#include <cjson/cJSON.h>

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
    const char *filepath = "/home/cat/test_1/data.json";
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
) {
    FILE *fp = fopen("/home/cat/test_1/data.json", "r");
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
) {
    FILE *fp = fopen("/home/cat/test_1/data.json", "r");
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
    fp = fopen("/home/cat/test_1/data.json", "w");
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
) {
    FILE *fp = fopen("/home/cat/test_1/data.json", "r");
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

void save_communicate_data(const char *address, const char *gateway, const char *dns) {
    const char *filepath = "/home/cat/test_1/data.json";
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

void load_communicate_data(char *address, char *gateway, char *dns, size_t max_len) {
    const char *filepath = "/home/cat/test_1/data.json";
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


void save_test_data(int32_t rotational_speed) {
    const char *filepath = "/home/cat/test_1/data.json";
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

    // 若文件不存在或 JSON 解析失败，则新建根对象
    if (!root) {
        root = cJSON_CreateObject();
    }

    // 获取或创建 last_data 对象
    cJSON *last_data = cJSON_GetObjectItem(root, "last_data");
    if (!last_data) {
        last_data = cJSON_CreateObject();
        cJSON_AddItemToObject(root, "last_data", last_data);
    }

    // 替换或添加 rotational_speed 字段
    cJSON *speed_item = cJSON_GetObjectItem(last_data, "rotational_speed");
    if (speed_item) {
        cJSON_ReplaceItemInObject(last_data, "rotational_speed", cJSON_CreateNumber(rotational_speed));
    } else {
        cJSON_AddNumberToObject(last_data, "rotational_speed", rotational_speed);
    }

    // 写回 JSON 文件
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

void load_test_data(int32_t *rotational_speed) {
    if (!rotational_speed) return;  // 防止空指针

    const char *filepath = "/home/cat/test_1/data.json";
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

    cJSON_Delete(root);
}