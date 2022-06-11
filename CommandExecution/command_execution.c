//
//  command_execution.h
//  EmployeeManager
//
//  Created by 孙康 on 2022/06/10.
//

#include "command_execution.h"
#include "hash_table.h"
#include "log.h"

static const uint16_t default_hash_size = 1024;     // 默认哈希表容量
static hash_table_t *s_hash_table = NULL;           // 哈希表
command_info_t g_cmd_infos[MAX_CMD];                // 指令操作信息

/**
 * @brief           比较员工工号
 * @param staff1    员工1
 * @param staff1    员工2
 * @return          比较结果
 */
static int compare_staff_id(const void *staff1, const void *staff2) {
    if (staff1 == NULL || staff2 == NULL) {
        return 0;
    }

    staff_info_t *info1 = (staff_info_t *)staff1;
    staff_info_t *info2 = (staff_info_t *)staff2;
    return info1->job_number - info2->job_number;
}

/**
 * @brief           比较员工入职日期
 * @param staff1    员工1
 * @param staff1    员工2
 * @return          比较结果
 */
static int compare_staff_date(const void *staff1, const void *staff2) {
    if (staff1 == NULL || staff2 == NULL) {
        return 0;
    }

    staff_info_t *info1 = (staff_info_t *)staff1;
    staff_info_t *info2 = (staff_info_t *)staff2;
    uint16_t diff = info1->date.year - info2->date.year;
    if (diff == 0) {
        diff = info1->date.month - info2->date.month;
        if (diff == 0) {
            diff = info1->date.day - info2->date.day;
        }
    }
    return diff;
}

/**
 * @brief       打印指定员工信息
 * @param value 员工信息
 */
static void print_a_staff_info(staff_info_t *value) {
    if (value == NULL) {
        return;
    }
    LOG_O("job_number: %llu, name: %s, date: %04d-%02d-%02d, department: %s, position: %s.", \
    value->job_number, value->name, value->date.year, value->date.month, value->date.day, \
    value->department, value->position)
}

/**
 * @brief           打印所有员工信息
 * @param values    员工信息数组
 * @param count     数组大小
 */
static void print_staffs_info(staff_info_t **values, uint64_t count, sort_type_t type) {
    if (values == NULL) {
        return;
    }

    switch (type) {
        case SORT_ID:
            qsort(values, count, sizeof(staff_info_t *), compare_staff_id);
            break;
        case SORT_DATE:
            qsort(values, count, sizeof(staff_info_t *), compare_staff_date);
            break;

        default:
            break;
    }

    for (uint64_t i = 0; i < count; i++) {
        print_a_staff_info(values[i]);
    }
}

/**
 * @brief               新增员工
 * @param job_number    工号
 * @param info          员工信息
 * @param is_opt_all    全局操作标志[仅DEL、GET指令支持]
 * @param type          排序方式[仅DGET指令支持]
 */
static void add_employee(uint64_t job_number, staff_info_t *info, bool is_opt_all, sort_type_t type) {
    if (s_hash_table == NULL) {
        s_hash_table = create_hash_table(default_hash_size);
        if (s_hash_table == NULL) {
            return;
        }
    }
    if (add_item_to_table(&s_hash_table, job_number, info, true)) {
        LOG_O("The staff [%llu] is added.", job_number)
    }
}

/**
 * @brief               删除员工
 * @param job_number    工号
 * @param info          员工信息
 * @param is_opt_all    全局操作标志[仅DEL、GET指令支持]
 * @param type          排序方式[仅DGET指令支持]
 */
static void del_employee(uint64_t job_number, staff_info_t *info, bool is_opt_all, sort_type_t type) {
    if (is_opt_all) {
        delete_hash_table(&s_hash_table);
        LOG_O("The database is deleted.")
    }
    else {
        if (remove_item_from_table(s_hash_table, job_number)) {
            LOG_O("The staff [%llu] is removed.", job_number)
        }
    }
}

/**
 * @brief               修改员工信息
 * @param job_number    工号
 * @param info          员工信息
 * @param is_opt_all    全局操作标志[仅DEL、GET指令支持]
 * @param type          排序方式[仅DGET指令支持]
 */
static void mod_employee(uint64_t job_number, staff_info_t *info, bool is_opt_all, sort_type_t type) {
    if (modify_item_from_table(s_hash_table, job_number, info)) {
        LOG_O("Info of the staff [%llu] is modified.", job_number)
    }
}

/**
 * @brief               获取员工信息
 * @param job_number    工号
 * @param info          员工信息
 * @param is_opt_all    全局操作标志[仅DEL、GET指令支持]
 * @param type          排序方式[仅DGET指令支持]
 */
static void get_employee(uint64_t job_number, staff_info_t *info, bool is_opt_all, sort_type_t type) {
    if (is_opt_all) {
        uint64_t count = 0;
        staff_info_t **staff_infos = get_all_items_of_table(s_hash_table, &count);
        print_staffs_info(staff_infos, count, type);
    }
    else {
        if (job_number > 0) {
            staff_info_t *staff_info = get_item_from_table(s_hash_table, job_number);
            print_a_staff_info(staff_info);
        }
        else {
            uint64_t count = 0;
            staff_info_t **staff_infos = get_items_by_info(s_hash_table, info, &count);
            print_staffs_info(staff_infos, count, type);
        }
    }
}

/**
 * @brief 初始化所有指令信息
 */
void init_all_cmd_info(void) {
    bzero(g_cmd_infos, sizeof(g_cmd_infos));
    g_cmd_infos[ADD].name = "ADD";
    g_cmd_infos[ADD].func = add_employee;
    g_cmd_infos[ADD].usage = "Use ADD cmd to add a staff to the database.\n"
        "\te.g. ADD 10086 name:Zhangsan date:2022-05-11 dept:ZTA pos:engineer\n";

    g_cmd_infos[DEL].name = "DEL";
    g_cmd_infos[DEL].func = del_employee;
    g_cmd_infos[DEL].usage = "Use DEL cmd to remove a/all staff from the database.\n"
        "\te.g. DEL 10086 to remove a staff, or DEL * to clear the database.\n";

    g_cmd_infos[MOD].name = "MOD";
    g_cmd_infos[MOD].func = mod_employee;
    g_cmd_infos[MOD].usage = "Use MOD cmd to modify a staff's info.\n"
        "\te.g. MOD 10086 dept:CWPP name:Lisi\n";

    g_cmd_infos[GET].name = "GET";
    g_cmd_infos[GET].func = get_employee;
    g_cmd_infos[GET].usage = "Use GET cmd to obtain a/all staff's info.\n"
        "\te.g. GET 10086 to obtain a staff's info, or GET name:Lisi dept:ZTA to obtain one or more staff's info, "
        "or GET * to print all staff's info.\n"
        "\tIf you want output being sorted, use --sort:xx, e.g. GET --sort:id * to sort output by staff id.";

    g_cmd_infos[HELP].name = "HELP";
    g_cmd_infos[EXIT].name = "EXIT";
}

/**
 * @brief               执行输入指令
 * @param command       指令
 * @param job_number    工号
 * @param info          员工信息
 * @param is_opt_all    全局操作标志[仅DEL、GET指令支持]
 * @param type          排序方式[仅DGET指令支持]
 */
void execute_input_command(user_command_t command, uint64_t job_number, staff_info_t *info, bool is_opt_all, sort_type_t type) {
    command_info_t *cmd_info = NULL;
    switch (command) {
        case ADD:
        case DEL:
        case MOD:
        case GET:
            cmd_info = &g_cmd_infos[command];
            break;
            
        case HELP:
            for (user_command_t i = NUL+1; i < MAX_CMD-2; i++) {
                LOG_O("%s", g_cmd_infos[i].usage)
            }
            return;
        case EXIT:
            if (s_hash_table != NULL) {
                delete_hash_table(&s_hash_table);
            }
            LOG_O("Process is over, now quit.")
            exit(0);
            return;
            
        default:
            return;
    }

    cmd_info->func(job_number, info, is_opt_all, type);
}