//
//  common.h
//  EmployeeManager
//
//  Created by 孙康 on 2022/06/10.
//

#ifndef common_h
#define common_h

#include <stdlib.h>
#include <stdbool.h>

static const uint8_t time_str_size = 20;        // 时间字符串长度
#define FREE(ptr)   if (ptr != NULL) {free(ptr); ptr = NULL;}

/**
 * @brief 员工信息
 */
typedef struct {
    uint64_t staff_id;      // 工号
    uint64_t date;          // 入职日期
    char *name;             // 姓名
    char *department;       // 部门
    char *position;         // 职位
} staff_info_t;

#endif /* common_h */
