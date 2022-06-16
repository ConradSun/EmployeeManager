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

#define FREE(ptr)   if (ptr != NULL) {free(ptr); ptr = NULL;}

/**
 * @brief 日期
 */
typedef struct {
    uint16_t year;      // 年
    uint8_t month;      // 月
    uint8_t day;        // 日
} short_date_t;

/**
 * @brief 员工信息
 */
typedef struct {
    uint64_t staff_id;      // 工号
    char *name;             // 姓名
    short_date_t date;      // 日期
    char *department;       // 部门
    char *position;         // 职位
} staff_info_t;

#endif /* common_h */
