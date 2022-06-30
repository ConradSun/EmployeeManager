//
//  command_execution.h
//  EmployeeManager
//
//  Created by 孙康 on 2022/06/10.
//

#ifndef command_execution_h
#define command_execution_h

#include "common.h"
#include <stdio.h>
#include <unistd.h>

/**
 * @brief 用户指令
 */
typedef enum {
    CMD_NUL,    // 空指令[错误指令]
    
    CMD_ADD,    // 增
    CMD_DEL,    // 删
    CMD_MOD,    // 改
    CMD_GET,    // 查
    
    CMD_LOG,    // 日志
    CMD_HELP,   // 帮助
    CMD_EXIT,   // 退出
    CMD_MAX
} user_command_t;

/**
 * @brief 员工信息类型
 */
typedef enum {
    INFO_NONE,
    INFO_ID,     // 工号
    INFO_NAME,   // 姓名
    INFO_DATE,   // 日期
    INFO_DEPT,   // 部门
    INFO_POS,    // 职位
    INFO_MAX
} info_type_t;

/**
 * @brief 输入参数类型
 */
typedef enum {
    INPUT_SORT      = 1 << 0,   // 排序方式
    INPUT_GLOBAL    = 1 << 1,   // 全局操作标志
    INPUT_LOG       = 1 << 2,   // log级别
    INPUT_ID        = 1 << 3,   // 员工工号[INPUT_INFO子集，必选]
    INPUT_INFO      = 1 << 4    // 员工信息[表示可选]
} param_type_t;

/**
 * @brief 排序方式
 */
typedef enum {
    SORT_NONE,  // 无需排序
    SORT_ID,    // 按工号排序
    SORT_DATE,  // 按日期排序
    SORT_MAX
} sort_type_t;

/**
 * @brief 用户请求处理
 */
typedef struct{
    char request[BUFSIZ];   // 用户原始输入
    char result[BUFSIZ];    // 用户查询结果
    uint8_t input_fd;       // 输入描述符[0-stdin, >0-remote]
    bool is_success;        // 请求处理成功标志
} user_request_t;

/**
 * @brief 查询信息
 */
typedef struct{
    user_command_t command; // 操作指令
    staff_info_t info;      // 员工信息
    bool is_opt_all;        // 全局操作标志[仅DEL、GET指令支持]
    sort_type_t sort_type;  // 排序方式[仅GET指令支持]
} query_info_t;

typedef void (*execute_func_t)(query_info_t *, user_request_t *);  // 执行指令函数指针

/**
 * @brief 指令信息
 */
typedef struct{
    char *name;     // 指令名称
    uint16_t param; // 输入参数[input_type_t位与]
    char *usage;    // 指令用法
    execute_func_t func;    // 指令回调函数
} command_info_t;

extern command_info_t g_cmd_infos[];

void execute_input_command(query_info_t *query, user_request_t *request);

#endif /* command_execution_h */