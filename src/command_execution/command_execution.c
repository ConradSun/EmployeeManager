//
//  command_execution.h
//  EmployeeManager
//
//  Created by 孙康 on 2022/06/10.
//

#include "command_execution.h"
#include "database_manager.h"
#include "manager_server.h"
#include "log.h"
#include <string.h>

command_info_t g_cmd_infos[CMD_MAX];    // 指令操作信息

/**
 * @brief           比较员工工号
 * @param staff1    员工1
 * @param staff1    员工2
 * @return          比较结果
 */
static int compare_staff_id(const void *staff1, const void *staff2) {
    if (staff1 == NULL || staff2 == NULL) {
        LOG_C(LOG_DEBUG, "The staff's info is empty.")
        return 0;
    }

    staff_info_t *info1 = *(staff_info_t **)staff1;
    staff_info_t *info2 = *(staff_info_t **)staff2;
    return info1->staff_id - info2->staff_id;
}

/**
 * @brief           比较员工入职日期
 * @param staff1    员工1
 * @param staff1    员工2
 * @return          比较结果
 */
static int compare_staff_date(const void *staff1, const void *staff2) {
    if (staff1 == NULL || staff2 == NULL) {
        LOG_C(LOG_DEBUG, "The staff's info is empty.")
        return 0;
    }
    
    staff_info_t *info1 = *(staff_info_t **)staff1;
    staff_info_t *info2 = *(staff_info_t **)staff2;
    uint64_t date1 = info1->date.year * 10000 + info1->date.month * 100 + info1->date.day;
    uint64_t date2 = info2->date.year * 10000 + info2->date.month * 100 + info2->date.day;
    return date1 - date2;
}

/**
 * @brief           打印指定员工信息
 * @param value     员工信息
 * @param output    缓存数组
 * @param size      缓存大小
 */
static void print_a_staff_info(const staff_info_t *value, char *output, size_t size) {
    if (value == NULL || output == NULL || size == 0) {
        return;
    }
    snprintf(output, size, "staff id: %llu, name: %s, date: %04d-%02d-%02d, department: %s, position: %s.\n", \
    value->staff_id, value->name, value->date.year, value->date.month, value->date.day, \
    value->department, value->position);
}

/**
 * @brief           打印所有员工信息
 * @param values    员工信息数组
 * @param count     数组大小
 * @param output    缓存数组
 */
static void print_staffs_info(staff_info_t **values, uint64_t count, sort_type_t type, char *output) {
    if (values == NULL) {
        return;
    }
    size_t rest = 0;
    size_t len = 0;
    LOG_C(LOG_DEBUG, "Total [%llu] staffs meet the criteria.", count)

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
        len = strlen(output);
        rest = BUFSIZ - len;
        print_a_staff_info(values[i], output+len, rest);
    }
}

/**
 * @brief           新增员工
 * @param query     查询信息
 * @param request   原始请求
 */
static void add_employee(query_info_t *query, user_request_t *request) {
    if (add_item_to_database(query->info)) {
        request->is_success = true;
        snprintf(request->result, BUFSIZ, "The staff [%llu] is added.", query->info->staff_id);
    }
    else {
        request->is_success = false;
        snprintf(request->result, BUFSIZ, "Failed to add the staff [%llu].", query->info->staff_id);
    }
}

/**
 * @brief           删除员工
 * @param query     查询信息
 * @param request   原始请求
 */
static void del_employee(query_info_t *query, user_request_t *request) {
    if (query->is_opt_all) {
        delete_database();
        create_database();
        request->is_success = true;
        snprintf(request->result, BUFSIZ, "All staffs are removed.");
    }
    else {
        if (remove_item_from_database(query->info->staff_id)) {
            request->is_success = true;
            snprintf(request->result, BUFSIZ, "The staff [%llu] is removed.", query->info->staff_id);
        }
        else {
            request->is_success = false;
            snprintf(request->result, BUFSIZ, "Failed to remove the staff [%llu].", query->info->staff_id);
        }
    }
}

/**
 * @brief           修改员工信息
 * @param query     查询信息
 * @param request   原始请求
 */
static void mod_employee(query_info_t *query, user_request_t *request) {
    if (modify_item_from_database(query->info)) {
        request->is_success = true;
        snprintf(request->result, BUFSIZ, "Info of the staff [%llu] is modified.", query->info->staff_id);
    }
    else {
        request->is_success = false;
        snprintf(request->result, BUFSIZ, "Failed to modify info of the staff [%llu].", query->info->staff_id);
    }
}

/**
 * @brief           获取员工信息
 * @param query     查询信息
 * @param request   原始请求
 */
static void get_employee(query_info_t *query, user_request_t *request) {
    if (query->is_opt_all || query->info->staff_id == 0) {
        uint64_t count = 0;
        staff_info_t **staff_infos = get_by_info_from_database(query->info, &count);
        if (count == 0) {
            snprintf(request->result, BUFSIZ, "No items are found.");
        }
        else {
            print_staffs_info(staff_infos, count, query->sort_type, request->result);
        }
    }
    else {
        staff_info_t *staff_info = get_by_id_from_database(query->info->staff_id);
        if (staff_info == NULL) {
            snprintf(request->result, BUFSIZ, "Staff with id [%llu] is not found.", query->info->staff_id);
        }
        else {
            print_a_staff_info(staff_info, request->result, BUFSIZ);
        }
    }
    request->is_success = true;
}

/**
 * @brief 初始化所有指令信息
 */
void init_all_cmd_info(void) {
    bzero(g_cmd_infos, sizeof(g_cmd_infos));
    g_cmd_infos[CMD_ADD].name = "ADD";
    g_cmd_infos[CMD_ADD].func = add_employee;
    g_cmd_infos[CMD_ADD].param = INPUT_ID | INPUT_INFO;
    g_cmd_infos[CMD_ADD].usage = "Use 'ADD' cmd to add a staff to the database.\n"
        "\te.g. [ADD id:10086 name:Zhangsan date:2022-05-11 dept:ZTA pos:engineer]\n";

    g_cmd_infos[CMD_DEL].name = "DEL";
    g_cmd_infos[CMD_DEL].func = del_employee;
    g_cmd_infos[CMD_DEL].param = INPUT_GLOBAL | INPUT_INFO;
    g_cmd_infos[CMD_DEL].usage = "Use 'DEL' cmd to remove a/all staff from the database.\n"
        "\te.g. [DEL id:10086] to remove a staff, or [DEL *] to clear the database.\n";

    g_cmd_infos[CMD_MOD].name = "MOD";
    g_cmd_infos[CMD_MOD].func = mod_employee;
    g_cmd_infos[CMD_MOD].param = INPUT_ID | INPUT_INFO;
    g_cmd_infos[CMD_MOD].usage = "Use 'MOD' cmd to modify a staff's info.\n"
        "\te.g. [MOD id:10086 dept:CWPP name:Lisi]\n";

    g_cmd_infos[CMD_GET].name = "GET";
    g_cmd_infos[CMD_GET].func = get_employee;
    g_cmd_infos[CMD_GET].param = INPUT_GLOBAL | INPUT_INFO;
    g_cmd_infos[CMD_GET].usage = "Use 'GET' cmd to obtain a/all staff's info.\n"
        "\te.g. [GET id:10086] to obtain a staff's info, or [GET name:Lisi dept:ZTA] to obtain one or more staff's info, "
        "or [GET *] to print all staff's info.\n"
        "\tIf you want output being sorted, use '--sort:id/date', e.g. [GET --sort:id *] to sort output by staff id.\n";

    g_cmd_infos[CMD_LOG].name = "LOG";
    g_cmd_infos[CMD_LOG].param = INPUT_LOG;
    g_cmd_infos[CMD_LOG].usage = "Use 'LOG' cmd [local user only] to set log level.\n"
        "\te.g. [LOG debug] to set log level to debug. Log level include [debug, info, error, fault, off].\n";
    g_cmd_infos[CMD_HELP].name = "HELP";
    g_cmd_infos[CMD_EXIT].name = "EXIT";
}

/**
 * @brief           执行输入指令
 * @param query     查询信息
 * @param request   原始请求
 */
void execute_input_command(query_info_t *query, user_request_t *request) {
    if (query == NULL || request == NULL) {
        return;
    }

    command_info_t *cmd_info = NULL;
    size_t rest = 0;
    switch (query->command) {
        case CMD_ADD:
        case CMD_DEL:
        case CMD_MOD:
        case CMD_GET:
            cmd_info = &g_cmd_infos[query->command];
            break;

        case CMD_LOG:
              snprintf(request->result, BUFSIZ, "LOG level is setted.");
              request->is_success = true;
              return;
        case CMD_HELP:
            for (user_command_t i = CMD_NUL + 1; i < CMD_MAX; ++i) {
                if (g_cmd_infos[i].usage == NULL) {
                    continue;
                }
                rest = BUFSIZ - strlen(request->result);
                rest = rest > strlen(g_cmd_infos[i].usage) ? rest : strlen(g_cmd_infos[i].usage);
                strncat(request->result, g_cmd_infos[i].usage, rest);
            }
            rest = BUFSIZ - strlen(request->result);
            strncat(request->result, "The above commands are not case sensitive.\n", rest);
            request->is_success = true;
            return;
        case CMD_EXIT:
            destroy_all_connection();
            delete_database();
            request->is_success = true;
            snprintf(request->result, BUFSIZ, "Process is over, now quit.");
            exit(0);
            return;
            
        default:
            return;
    }

    cmd_info->func(query, request);
}
