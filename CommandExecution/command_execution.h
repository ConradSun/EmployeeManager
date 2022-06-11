//
//  command_execution.h
//  EmployeeManager
//
//  Created by 孙康 on 2022/06/10.
//

#ifndef command_execution_h
#define command_execution_h

#include <unistd.h>
#include <stdbool.h>
#include "common.h"

/**
 * @brief 有效用户指令
 */
typedef enum {
    NUL,    // 空指令[错误指令]
    
    ADD,    // 增
    DEL,    // 删
    MOD,    // 改
    GET,    // 查
    
    HELP,   // 帮助
    EXIT,   // 退出
    MAX_CMD
} user_command_t;

/**
 * @brief 员工信息类型
 */
typedef enum {
    NAME,   // 姓名
    DATE,   // 日期
    DEPT,   // 部门
    POS,    // 职位
    MAX_TYPE
} info_type_t;

/**
 * @brief 排序方式
 */
typedef enum {
    SORT_NONE,  // 无需排序
    SORT_ID,    // 按工号排序
    SORT_DATE,  // 按日期排序
} sort_type_t;

typedef void (*execute_func_t)(uint64_t, staff_info_t *, bool, sort_type_t); // 执行指令函数指针

/**
 * @brief 指令信息
 */
typedef struct{
    char *name;     // 指令名称
    char *param;    // 指令参数[暂未使用]
    char *usage;    // 指令用法
    execute_func_t func;    // 指令回调函数
} command_info_t;

extern command_info_t g_cmd_infos[];

void init_all_cmd_info(void);
void execute_input_command(user_command_t command, uint64_t job_number, staff_info_t *info, bool is_opt_all, sort_type_t type);

#endif /* command_execution_h */