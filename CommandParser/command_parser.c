//
//  command_parser.c
//  EmployeeManager
//
//  Created by 孙康 on 2022/5/30.
//

#include "command_parser.h"
#include "command_execution.h"
#include "common.h"
#include "log.h"

static fd_set input_set = {0};                      // 标准输入管理
static const uint8_t date_str_size = 10;            // 日期字符串大小
static const uint8_t max_cmd_size = 5;              // 最大指令长度
static const uint16_t buffer_size = 1024;           // 输入缓存大小
static char input_msg[buffer_size] = {'\0'};        // 输入缓存

/**
* @brief 信息类型描述
*/
static const char *type_str[] = {
    [NAME]  = "name",
    [DATE]  = "date",
    [DEPT]  = "dept",
    [POS]   = "pos",
};

/**
 * @brief           字符串中的小写字母转换为大写
 * @param string    待判断字符串
 */
static void string_to_upper(char *string) {
    if (string == NULL) {
        return;
    }

    size_t size = strlen(string);
    for (size_t i = 0; i < size; i++) {
        if (string[i] >= 'a' && string[i] <= 'z') {
            string[i] -= ('a' - 'A');
        }
    }
}

/**
 * @brief           判断字符串是否匹配指定前缀
 * @param string    待判断字符串
 * @param prefix    匹配前缀
 * @return          false表示匹配失败，否则为成功
 */
static bool is_string_prefix(const char *string, const char *prefix) {
    size_t str_len = strlen(string);
    size_t pre_len = strlen(prefix);
    
    if (str_len < pre_len) {
        return false;
    }
    for (size_t i = 0; i < pre_len; ++i) {
        if (string[i] != prefix[i]) {
            return false;
        }
    }
    return true;
}

/**
 * @brief               获取分割符位置[空格或换行字符]
 * @param string        待处理字符串
 * @param space_count   无效空格数量
 * @return              -1表示不存在，否则为分割符位置
 */
static int get_split_site(const char *string, int *space_count) {
    size_t index = 0;
    size_t size = strlen(string);
    
    // 跳过无效空格
    while (index < size) {
        if (string[index] == ' ') {
            index++;
        }
        else {
            break;
        }
    }
    *space_count = index;
    
    // 查找分隔符
    while (index < size) {
        if (string[index] == ' ' || string[index] == '\n') {
            return index;
        }
        index++;
    }
    
    return -1;
}

/**
 * @brief   获取终端输入信息
 * @return  false表示获取失败，否则为成功
 */
bool get_input_message(void) {
    FD_ZERO(&input_set);
    FD_SET(STDIN_FILENO, &input_set);
    int result = select(1, &input_set, NULL, NULL, NULL);
    if (result < 0) {
        LOG_C(LOG_ERROR, "Failed to select input fd.")
        return false;
    }
    
    if (!FD_ISSET(STDIN_FILENO, &input_set)) {
        return false;
    }
    
    bzero(input_msg, buffer_size);
    fgets(input_msg, buffer_size, stdin);
    
    return true;
}

/**
 * @brief           解析输入指令
 * @param string    待解析字符串
 * @return          指令
 */
user_command_t parse_input_command(char *string) {
    if (string == NULL) {
        return NUL;
    }
    
    user_command_t cmd = NUL;
    // 指令大小写不敏感
    string_to_upper(string);
    for (uint8_t i = NUL + 1; i < MAX_CMD; ++i) {
        if (strcmp(string, g_cmd_infos[i].name) == 0) {
            cmd = i;
            break;
        }
    }
    
    return cmd;
}

/**
 * @brief           解析排序方式
 * @param string    待解析字符串
 * @return          排序方式
 */
sort_type_t parse_sort_type(const char *string) {
    char sort_begin[] = "--sort:";
    if (string == NULL || strlen(string) <= strlen(sort_begin)) {
        return SORT_NONE;
    }

    uint8_t begin = strlen(sort_begin);
    char id[] = "id";
    char date[] = "date";
    if (is_string_prefix(string, sort_begin)) {
        if (strcmp(string+begin, id) == 0) {
            return SORT_ID;
        }
        if (strcmp(string+begin, date) == 0) {
            return SORT_DATE;
        }
    }

    return SORT_NONE;
}

/**
 * @brief           解析员工信息
 * @param string    待解析字符串
 * @param info      信息填充地址
 * @return          false表示解析失败，否则为成功
 */
bool parse_staff_info(const char *string, staff_info_t *info) {
    if (string == NULL) {
        return false;
    }
    
    int type = -1;
    size_t size = strlen(string);
    size_t end = 0;
    // 匹配信息字符串前缀，获取有效信息类型
    for (uint8_t i = NAME; i < MAX_TYPE; ++i) {
        if (is_string_prefix(string, type_str[i])) {
            size_t len = strlen(type_str[i]);
            if (string[len] == ':') {
                end = len + 1;
                type = i;
                break;
            }
            else {
                return false;
            }
        }
    }
    if (type == -1 || end >= size) {
        return false;
    }
    
    switch (type) {
        case NAME:
            info->name = strndup(string + end, size - end);
            break;
        case DATE:
            // 后面补充日期格式校验
            if (size - end != date_str_size) {
                LOG_C(LOG_ERROR, "Input date is invalid.")
                return false;
            }
            info->date.year = (string[end] - '0') * 1000 + (string[end+1] - '0') * 100 + (string[end+2] - '0') * 10 + (string[end+3] - '0');
            info->date.month = (string[end+5] - '0') * 10 + (string[end+6] - '0');
            info->date.day = (string[end+8] - '0') * 10 + (string[end+9] - '0');
            break;
        case DEPT:
            info->department = strndup(string + end, size - end);
            break;
        case POS:
            info->position = strndup(string + end, size - end);
            break;
            
        default:
            break;
    }
    
    return true;
}

/**
 * @brief 解析输入信息
 */
void parse_input_messgae(void) {
    user_command_t command = NUL;
    uint64_t job_number = 0;
    staff_info_t info = {0};
    bool is_opt_all = false;
    sort_type_t sort_type = SORT_NONE;
    
    char message[buffer_size] = {'\0'}; // 待解析字符串
    size_t size = strlen(input_msg);    // 原始输入大小
    size_t start = 0;                   // 待解析串起点
    int index = 0;                      // 当前解析位置
    int space_count = 0;                // 待解析串前空格数量
    
    // 获取操作指令
    index = get_split_site(input_msg, &space_count);
    start += space_count;
    if (index == -1 || (index - space_count) >= max_cmd_size) {
        LOG_C(LOG_ERROR, "Input is invalid [too long command].")
        return;
    }
    index++;
    strlcpy(message, input_msg + start, index - start);
    command = parse_input_command(message);
    bzero(message, buffer_size);
    if (command == NUL) {
        LOG_C(LOG_ERROR, "Input is invalid [unknown command].")
        return;
    }

    // 检查--sort标志
    if (command == GET && input_msg[index] == '-') {
        start = index;
        index = get_split_site(input_msg + start, &space_count) + start;
        start += space_count;
        if (index == -1) {
            LOG_C(LOG_ERROR, "Input is invalid [invalid job number].")
            return;
        }
        index++;
        strlcpy(message, input_msg + start, index - start);
        sort_type = parse_sort_type(message);
        bzero(message, buffer_size);
    }
    
    // 检查*通配符
    if ((command == DEL || command == GET) && input_msg[index] == '*') {
        if (input_msg[index+1] == '\n') {
            is_opt_all = true;
            goto END;
        }
        else {
            LOG_C(LOG_ERROR, "Input is invalid ['*' should be followed by a line break].")
        }
        return;
    }

    // 获取员工工号
    start = index;
    index = get_split_site(input_msg + start, &space_count) + start;
    start += space_count;
    if (index == -1) {
        LOG_C(LOG_ERROR, "Input is invalid [invalid job number].")
        return;
    }
    index++;
    strlcpy(message, input_msg + start, index - start);
    job_number = atoi(message);
    // 当前输入不含工号，则解析为员工信息
    if (job_number == 0) {
        index = start;
    }
    bzero(message, buffer_size);
    
    // 获取员工信息
    while (index < size) {
        start = index;
        index = get_split_site(input_msg + start, &space_count) + start;
        start += space_count;
        if (index == -1) {
            LOG_C(LOG_ERROR, "Input is invalid [invalid staff info].")
            break;
        }
        index++;
        strlcpy(message, input_msg + start, index - start);
        parse_staff_info(message, &info);
        bzero(message, buffer_size);
    }
    
END:
    execute_input_command(command, job_number, &info, is_opt_all, sort_type);
    FREE(info.name)
    FREE(info.position)
    FREE(info.department)
}
