//
//  command_parser.c
//  EmployeeManager
//
//  Created by 孙康 on 2022/5/30.
//

#include "command_parser.h"
#include "common.h"
#include "log.h"
#include <ctype.h>

static const uint8_t max_input_params = 32;     // 最多输入参数组
static const char sort_flag[] = "--sort:";      // 排序标识
static const char global_flag[] = "*";          // 全局操作标识

/**
* @brief 信息类型描述
*/
static const char *info_type_str[] = {
    [INFO_ID]   = "id",
    [INFO_NAME] = "name",
    [INFO_DATE] = "date",
    [INFO_DEPT] = "dept",
    [INFO_POS]  = "pos",
};

/**
* @brief 排序方式描述
*/
static const char *sort_type_str[] = {
    [SORT_ID]   = "id",
    [SORT_DATE] = "date",
};

/**
* @brief 日志类型描述
*/
STATIC const char *log_level_str[] = {
    [LOG_OFF]   = "off",
    [LOG_FAULT] = "fault",
    [LOG_ERROR] = "error",
    [LOG_INFO]  = "info",
    [LOG_DEBUG] = "debug",
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
STATIC bool is_string_prefix(const char *string, const char *prefix) {
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
STATIC int get_split_site(const char *string, int *space_count) {
    size_t index = 0;
    size_t size = strlen(string);
    *space_count = 0;
    
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
    
    // 查找分隔符[空格、换行、结束符]
    while (index < size) {
        if (string[index] == ' ' || string[index] == '\n' || string[index] == '\0') {
            return index;
        }
        index++;
    }
    
    return -1;
}

/**
 * @brief           分割用户输入位参数数组
 * @param string    待分割字符串
 * @param params    待填充参数数组
 * @return          解析参数个数
 */
STATIC uint8_t get_split_params(const char *string, char params[][BUFSIZ]) {
    uint8_t param_cnt = 0;          // 参数数量
    size_t size = strlen(string);   // 字符串长度
    int start = 0;                  // 待解析串起点
    int index = 0;                  // 当前解析位置
    int space_count = 0;            // 待解析串前空格数量

    for (uint8_t i = 0; i < max_input_params; ++i) {
        if (index >= size) {
            break;
        }
        int temp = get_split_site(string+index, &space_count);
        if (temp <= 0) {
            break;
        }
        start = index + space_count;
        index += temp + 1;
        if (index-start >= BUFSIZ) {
            LOG_C(LOG_ERROR, "Input param is too long.")
            break;
        }
        strlcpy(params[i], string+start, index-start);
        param_cnt++;
    }

    return param_cnt;
}

/**
 * @brief           判断字符串是否为合法日期格式
 * @param string    待判断字符串
 * @return          false表示非法，否则为合法
 */
STATIC bool is_date_valid(const char *string) {
    uint8_t date_str_size = 10;     // 日期字符串大小
    uint8_t year_split_site = 4;    // 分隔符位置1
    uint8_t month_split_site = 7;   // 分隔符位置2
    size_t str_len = strlen(string);
    
    if (str_len != date_str_size) {
        return false;
    }
    if (string[year_split_site] != '-' || string[month_split_site] != '-') {
        return false;
    }

    for (size_t i = 0; i < str_len; ++i) {
        if (i == year_split_site || month_split_site) {
            continue;
        }
        if (isdigit(string[i]) == 0) {
            return false;
        }
    }
    return true;
}

/**
 * @brief           判断字符串是否为合法姓名格式
 * @param string    待判断字符串
 * @return          false表示非法，否则为合法
 */
STATIC bool is_name_valid(const char *string) {
    size_t str_len = strlen(string);
    for (size_t i = 0; i < str_len; ++i) {
        if (isalpha(string[i]) == 0) {
            return false;
        }
    }
    return true;
}

/**
 * @brief           解析排序方式
 * @param string    待解析字符串
 * @return          排序方式
 */
STATIC sort_type_t parse_sort_type(const char *string) {
    sort_type_t type = SORT_NONE;
    uint8_t begin = strlen(sort_flag);

    if (!is_string_prefix(string, sort_flag)) {
        return SORT_NONE;
    }
    for (sort_type_t i = SORT_NONE + 1; i < SORT_MAX; i++) {
        if (strcmp(string+begin, sort_type_str[i]) == 0) {
            LOG_C(LOG_DEBUG, "Sort by staff [%s].", sort_type_str[i])
            type = i;
            break;
        }
    }

    return type;
}

/**
 * @brief           解析日志等级
 * @param string    待解析字符串
 * @return          false表示解析失败，否则为成功
 */
STATIC bool parse_log_level(const char *string) {
    for (log_level_t i = LOG_OFF; i <= LOG_DEBUG; ++i) {
        if (strcmp(string, log_level_str[i]) == 0) {
            g_log_level = i;
            LOG_C(LOG_DEBUG, "Now log level is [%s].", log_level_str[i])
            return true;
        }
    }
    return false;
}

/**
 * @brief           解析信息类型
 * @param string    待解析字符串
 * @return          false表示解析失败，否则为成功
 */
STATIC info_type_t parse_info_type(const char *string, size_t *end) {
    info_type_t type = INFO_NONE;

    // 匹配信息字符串前缀，获取有效信息类型
    for (info_type_t i = INFO_NONE + 1; i < INFO_MAX; ++i) {
        if (is_string_prefix(string, info_type_str[i])) {
            size_t len = strlen(info_type_str[i]);
            if (string[len] == ':') {
                *end = len + 1;
                type = i;
            }
            break;
        }
    }

    return type;
}

/**
 * @brief           解析员工信息
 * @param string    待解析字符串
 * @param info      信息填充地址
 * @return          false表示解析失败，否则为成功
 */
STATIC bool parse_staff_info(const char *string, staff_info_t *info) {
    if (string == NULL) {
        return false;
    }
    
    size_t size = strlen(string);
    size_t end = 0;
    info_type_t type = parse_info_type(string, &end);
    if (type == INFO_NONE || end >= size) {
        return false;
    }
    
    switch (type) {
        case INFO_ID:
            info->staff_id = atoi(string + end);
            if (info->staff_id == 0) {
                LOG_C(LOG_ERROR, "Input staff id is invalid.")
                return false;
            }
            LOG_C(LOG_DEBUG, "Staff id is [%llu].", info->staff_id)
            break;
        case INFO_NAME:
            if (!is_name_valid(string + end)) {
                LOG_C(LOG_ERROR, "Input name is invalid.")
                return false;
            }
            info->name = strndup(string + end, size - end);
            break;
        case INFO_DATE:
            if (!is_date_valid(string + end)) {
                LOG_C(LOG_ERROR, "Input date is invalid.")
                return false;
            }
            info->date.year = (string[end] - '0') * 1000 + (string[end+1] - '0') * 100 + (string[end+2] - '0') * 10 + (string[end+3] - '0');
            info->date.month = (string[end+5] - '0') * 10 + (string[end+6] - '0');
            info->date.day = (string[end+8] - '0') * 10 + (string[end+9] - '0');
            break;
        case INFO_DEPT:
            info->department = strndup(string + end, size - end);
            break;
        case INFO_POS:
            info->position = strndup(string + end, size - end);
            break;
            
        default:
            break;
    }
    
    return true;
}

/**
 * @brief       解析输入指令
 * @param input 待解析字符串
 * @return      解析指令
 */
STATIC user_command_t parse_input_command(char *input) {
    user_command_t command = CMD_NUL;
    // 指令大小写不敏感
    string_to_upper(input);
    for (user_command_t i = CMD_NUL + 1; i < CMD_MAX; ++i) {
        if (strcmp(input, g_cmd_infos[i].name) == 0) {
            command = i;
            LOG_C(LOG_DEBUG, "Input command is [%s].", g_cmd_infos[i].name)
            break;
        }
    }
    
    return command;
}

/**
 * @brief               解析命令关联参数
 * @param params        存放参数数组
 * @param count         数组大小
 * @param query_info    存放地址
 * @return              false表示解析失败，否则为成功
 */
STATIC bool parse_input_params(const char params[][BUFSIZ], uint8_t count, query_info_t *query_info) {
    uint16_t param_type = g_cmd_infos[query_info->command].param;

    for (uint8_t i = 0; i < count; ++i) {
        // 检查是否为排序标志[最多输入一次]
        if (param_type & INPUT_SORT) {
            query_info->sort_type = parse_sort_type(params[i]);
            if (query_info->sort_type != SORT_NONE) {
                param_type ^= INPUT_SORT;   // 不允许重复输入
                continue;
            }
        }
        // 检查是否为全局操作标志[最多输入一次]
        if (param_type & INPUT_GLOBAL) {
            if (strcmp(params[i], global_flag) == 0) {
                query_info->is_opt_all = true;
                param_type ^= INPUT_GLOBAL; // 不允许重复输入
                continue;
            }
        }
        // 检查是否为日志标志[最多输入一次]
        if (param_type & INPUT_LOG) {
            if (parse_log_level(params[i])) {
                param_type ^= INPUT_LOG;    // 不允许重复输入
                continue;
            }
        }
        // 检查是否为日志标志[可重复输入，相同信息以最后输入为准]
        if (param_type & INPUT_ID || param_type & INPUT_INFO) {
            if (parse_staff_info(params[i], query_info->info)) {
                continue;
            }
        }

        // 以上标志均未匹配，则输入格式错误
        LOG_C(LOG_ERROR, "Failed to parse [%s] for repeat or invalid format.", params[i])
        return false;
    }
    // 输入参数中含ID表示用户输入必须包含ID
    if (param_type & INPUT_ID && query_info->info->staff_id == 0) {
        return false;
    }

    return true;
}

/**
 * @brief               处理用户请求
 * @param user_request  用户请求信息
 */
void process_input_messgae(user_request_t *user_request) {
    if (user_request == NULL) {
        return;
    }

    char params[max_input_params][BUFSIZ] = {0};    // 参数数组
    bzero(params, max_input_params*BUFSIZ);
    uint8_t param_cnt = 0;              // 数组大小
    staff_info_t info = {0};            // 员工信息
    query_info_t query_info = {0};      // 查询详情

    query_info.command = CMD_NUL;
    query_info.info = &info;
    query_info.is_opt_all = false;
    query_info.sort_type = SORT_NONE;
    
    // 获取分割参数
    param_cnt = get_split_params(user_request->request, params);
    // 获取操作指令
    query_info.command = parse_input_command(params[0]);
    if (query_info.command == CMD_NUL) {
        goto ERROR;
    }
    // 获取指令操作数据
    if (!parse_input_params(params+1, param_cnt-1, &query_info)) {
        goto ERROR;
    }
    // 禁止非本地用户执行EXIT
    if (user_request->input_fd != STDIN_FILENO && query_info.command == CMD_EXIT) {
        goto ERROR;
    }
    goto EXEC;

ERROR:
    snprintf(user_request->result, BUFSIZ, "Failed to parse user input for invalid command or info.");
    goto END;
EXEC:
    execute_input_command(&query_info, user_request);
END:
    FREE(info.name)
    FREE(info.position)
    FREE(info.department)
}
