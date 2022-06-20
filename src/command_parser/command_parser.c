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

static const uint8_t max_cmd_size = 5;              // 最大指令长度

/**
* @brief 信息类型描述
*/
static const char *info_type_str[] = {
    [ID]  = "id",
    [NAME]  = "name",
    [DATE]  = "date",
    [DEPT]  = "dept",
    [POS]   = "pos",
};

/**
* @brief 日志类型描述
*/
static const char *log_level_str[] = {
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
 * @brief           判断字符串是否为合法日期格式
 * @param string    待判断字符串
 * @return          false表示非法，否则为合法
 */
static bool is_date_valid(const char *string) {
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
static bool is_name_valid(const char *string) {
    size_t str_len = strlen(string);
    for (size_t i = 0; i < str_len; ++i) {
        if (isalpha(string[i]) == 0) {
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
 * @brief           解析排序方式
 * @param string    待解析字符串
 * @return          排序方式
 */
static sort_type_t parse_sort_type(const char *string) {
    char sort_begin[] = "--sort:";
    uint8_t begin = strlen(sort_begin);
    char id[] = "id";
    char date[] = "date";
    int space_count = 0;

    get_split_site(string, &space_count);
    string += space_count;

    if (string == NULL || strlen(string) <= strlen(sort_begin)) {
        return SORT_NONE;
    }
    if (is_string_prefix(string, sort_begin)) {
        if (strcmp(string+begin, id) == 0) {
            LOG_C(LOG_DEBUG, "Sort by staff id.")
            return SORT_ID;
        }
        if (strcmp(string+begin, date) == 0) {
            LOG_C(LOG_DEBUG, "Sort by date.")
            return SORT_DATE;
        }
    }

    return SORT_NONE;
}

/**
 * @brief           解析日志等级
 * @param string    待解析字符串
 * @return          false表示解析失败，否则为成功
 */
static bool parse_log_level(const char *string) {
    int space_count = 0;

    get_split_site(string, &space_count);
    string += space_count;

    for (uint8_t i = LOG_OFF; i <= LOG_DEBUG; ++i) {
        if (is_string_prefix(string, log_level_str[i])) {
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
static info_type_t parse_info_type(const char *string, size_t *end) {
    info_type_t type = TYPE_NONE;

    // 匹配信息字符串前缀，获取有效信息类型
    for (info_type_t i = TYPE_NONE + 1; i < MAX_TYPE; ++i) {
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
static bool parse_staff_info(const char *string, staff_info_t *info) {
    if (string == NULL) {
        return false;
    }
    
    info_type_t type = TYPE_NONE;
    size_t size = strlen(string);
    size_t end = 0;
    int space_count = 0;

    get_split_site(string, &space_count);
    string += space_count;
    type = parse_info_type(string, &end);
    if (type == TYPE_NONE || end >= size) {
        return false;
    }
    
    switch (type) {
        case ID:
            info->staff_id = atoi(string + end);
            if (info->staff_id == 0) {
                LOG_C(LOG_ERROR, "Input staff id is invalid.")
                return false;
            }
            LOG_C(LOG_DEBUG, "Staff id is [%llu].", info->staff_id)
            break;
        case NAME:
            if (!is_name_valid(string + end)) {
                LOG_C(LOG_ERROR, "Input name is invalid.")
                return false;
            }
            info->name = strndup(string + end, size - end);
            break;
        case DATE:
            if (!is_date_valid(string + end)) {
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
 * @brief           解析输入指令
 * @param input     待解析字符串
 * @param command   指令
 * @return          -1表示解析失败，否则为解析位置
 */
static int parse_input_command(const char *input, user_command_t *command) {
    char message[BUFSIZ] = {'\0'};      // 待解析字符串
    size_t start = 0;                   // 待解析串起点
    int index = 0;                      // 当前解析位置
    int space_count = 0;                // 待解析串前空格数量

    index = get_split_site(input, &space_count);
    start += space_count;
    if (index == -1 || (index - space_count) >= max_cmd_size) {
        LOG_C(LOG_ERROR, "Input is invalid [invalid command].")
        return -1;
    }
    index++;
    strlcpy(message, input + start, index - start);

    // 指令大小写不敏感
    string_to_upper(message);
    for (uint8_t i = NUL + 1; i < MAX_CMD; ++i) {
        if (strcmp(message, g_cmd_infos[i].name) == 0) {
            *command = i;
            LOG_C(LOG_DEBUG, "Input command is [%s].", g_cmd_infos[i].name)
            break;
        }
    }
    if (*command == NUL) {
        LOG_C(LOG_ERROR, "Input is invalid [unknown command].")
        return -1;
    }
    
    return index;
}

/**
 * @brief       解析输入标识
 * @param input 待解析字符串
 * @param query 查询信息
 * @return      -1表示解析失败，否则为解析位置
 */
static int parse_input_flags(const char *input, query_info_t *query) {
    char message[BUFSIZ] = {'\0'};  // 待解析字符串
    size_t start = 0;               // 待解析串起点
    int index = 0;                  // 当前解析位置
    int space_count = 0;            // 待解析串前空格数量

    // 检查--sort标志
    if (query->command == GET && input[index] == '-') {
        start = index;
        index = get_split_site(input + start, &space_count) + start;
        start += space_count;
        if (index == -1) {
            LOG_C(LOG_ERROR, "Input is invalid [invalid job number].")
            return -1;
        }
        index++;
        strlcpy(message, input + start, index - start);
        query->sort_type = parse_sort_type(message);
        bzero(message, BUFSIZ);
    }
    
    // 检查*通配符
    if ((query->command == DEL || query->command == GET) && input[index] == '*') {
        if (input[index+1] == '\n') {
            query->is_opt_all = true;
            LOG_C(LOG_DEBUG, "Flag of Operating all is found.")
            return index;
        }
        else {
            LOG_C(LOG_ERROR, "Input is invalid ['*' should be followed by a line break].")
            return -1;
        }
    }

    // 检查log级别
    if (query->command == LOG) {
        if (parse_log_level(input)) {
            return index;
        }
        else {
            LOG_C(LOG_ERROR, "Input is invalid [unknown log level].")
            return -1;
        }
    }

    return index;
}

/**
 * @brief       解析输入员工信息
 * @param input 待解析字符串
 * @param info  员工信息
 * @return      -1表示解析失败，否则为解析位置
 */
static int parse_input_info(const char *input, staff_info_t *info) {
    char message[BUFSIZ] = {'\0'};      // 待解析字符串
    size_t start = 0;                   // 待解析串起点
    int index = 0;                      // 当前解析位置
    int space_count = 0;                // 待解析串前空格数量
    size_t size = strlen(input);        // 原始输入大小
    
    // 获取员工信息
    while (index < size) {
        start = index;
        index = get_split_site(input + start, &space_count) + start;
        start += space_count;
        if (index == -1) {
            LOG_C(LOG_ERROR, "Input is invalid [invalid staff info].")
            break;
        }
        index++;
        strlcpy(message, input + start, index - start);
        if (!parse_staff_info(message, info)) {
            return -1;
        }
        bzero(message, BUFSIZ);
    }

    return index;
}

/**
 * @brief               处理用户请求
 * @param user_request  用户请求信息
 */
void process_input_messgae(user_request_t *user_request) {
    if (user_request == NULL) {
        return;
    }

    staff_info_t info = {0};            // 员工信息
    query_info_t query_info = {0};      // 查询详情
    int index = 0;                      // 当前解析位置
    int ret_idx = 0;

    query_info.command = NUL;
    query_info.info = &info;
    query_info.is_opt_all = false;
    query_info.sort_type = SORT_NONE;
    
    // 获取操作指令
    index = parse_input_command(user_request->request, &query_info.command);
    if (index < 0) {
        goto END;
    }
    if (user_request->input_fd != STDIN_FILENO && query_info.command == EXIT) {
        goto END;
    }
    if (query_info.command == HELP || query_info.command == EXIT) {
        goto EXEC;
    }

    // 获取操作标志
    ret_idx = parse_input_flags(user_request->request+index, &query_info);
    index += ret_idx;
    if (ret_idx < 0) {
        goto END;
    }
    if (query_info.command == LOG || query_info.is_opt_all) {
        goto EXEC;
    }

    // 获取员工信息
    ret_idx =  parse_input_info(user_request->request+index, &info);
    index += ret_idx;
    if (ret_idx < 0) {
        goto END;
    }
    
EXEC:
    execute_input_command(&query_info, user_request);
END:
    FREE(info.name)
    FREE(info.position)
    FREE(info.department)
}
