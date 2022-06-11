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
    NUL,
    
    ADD,
    DEL,
    MOD,
    GET,
    
    HELP,
    EXIT,
    MAX_CMD
} user_command_t;

/**
 * @brief 员工信息类型
 */
typedef enum {
    NAME,
    DATE,
    DEPT,
    POS,
    MAX_TYPE
} info_type_t;

/**
 * @brief 排序方式
 */
typedef enum {
    SORT_NONE,
    SORT_ID,
    SORT_DATE,
} sort_type_t;

void execute_input_command(user_command_t command, uint64_t job_number, staff_info_t *info, bool is_opt_all, sort_type_t type);

#endif /* command_execution_h */