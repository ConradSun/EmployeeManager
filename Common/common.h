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
    uint16_t year;
    uint8_t month;
    uint8_t day;
} short_date_t;

/**
 * @brief 员工信息
 */
typedef struct {
    uint64_t job_number;
    char *name;
    short_date_t date;
    char *department;
    char *position;
} staff_info_t;

#endif /* common_h */
