#include "database_manager.h"
#include "hash_table.h"
#include "log.h"
#include <string.h>

static const uint16_t default_table_size = 1024;    // 默认哈希表容量
static hash_table_t *s_hash_table = NULL;           // 哈希表

/**
 * @brief       清理存储值
 * @param value 待清理值
 */
STATIC void clear_value(void *value) {
    staff_info_t *info = (staff_info_t *)value;
    if (info != NULL) {
        FREE(info->name)
        FREE(info->position)
        FREE(info->department)
        FREE(info)
    }
}

/**
 * @brief       拷贝存储值
 * @param dst   拷贝至
 * @param src   拷贝于
 */
STATIC void copy_value(void *dst, const void *src) {
    staff_info_t *dst_value = (staff_info_t *)dst;
    staff_info_t *src_value = (staff_info_t *)src;

    if (dst_value != NULL && src_value != NULL) {
        dst_value->staff_id = src_value->staff_id;
        dst_value->date = src_value->date;

        if (src_value->name != NULL) {
            FREE(dst_value->name)
            dst_value->name = strndup(src_value->name, strlen(src_value->name));
        }
        if (src_value->position != NULL) {
            FREE(dst_value->position)
            dst_value->position = strndup(src_value->position, strlen(src_value->position));
        }
        if (src_value->department != NULL) {
            FREE(dst_value->department)
            dst_value->department = strndup(src_value->department, strlen(src_value->department));
        }
    }
}

/**
 * @brief           比较字符串是否相同
 * @param src_str   字符串1
 * @param dst_str   字符串2
 * @return          false表示不同，否则为相同
 */
STATIC bool is_string_equal(const char *src_str, const char *dst_str) {
    // src_str为空表示通配
    if (src_str != NULL) {
        if (dst_str != NULL) {
            if (strcmp(src_str, dst_str) != 0) {
                return false;
            }
        }
        else {
            return false;
        }
    }

    return true;
}

/**
 * @brief       比较员工信息是否相同
 * @param src   信息1
 * @param dst   信息2
 * @return      false表示不同，否则为相同
 */
STATIC bool is_value_equal(const void *src, const void *dst) {
    staff_info_t *dst_value = (staff_info_t *)dst;
    staff_info_t *src_value = (staff_info_t *)src;

    if (src_value == NULL || dst_value == NULL) {
        return false;
    }

    if (!is_string_equal(src_value->name, dst_value->name)) {
        return false;
    }
    if (!is_string_equal(src_value->department, dst_value->department)) {
        return false;
    }
    if (!is_string_equal(src_value->position, dst_value->position)) {
        return false;
    }
    if (src_value->date != 0 && src_value->date != dst_value->date) {
        return false;
    }
    
    return true;
}

/**
 * @brief   创建数据库
 * @return  false表示失败，否则为成功
 */
bool create_database(void) {
    table_init_config_t config = {
        .max_size = default_table_size,
        .value_size = sizeof(staff_info_t),
        .clear_func = clear_value,
        .copy_func = copy_value,
        .match_func = is_value_equal
    };
    s_hash_table = create_hash_table(&config);
    if (s_hash_table == NULL) {
        return false;
    }
    return true;
}

/**
 * @brief 删除数据库
 */
void delete_database(void) {
    delete_hash_table(&s_hash_table);
}

/**
 * @brief       添加员工
 * @param info  员工信息
 * @return      false表示失败，否则为成功
 */
bool add_item_to_database(staff_info_t *info) {
    return add_item_to_table(&s_hash_table, info->staff_id, info, true);
}

/**
 * @brief           删除员工
 * @param staff_id  工号
 * @return          false表示失败，否则为成功
 */
bool remove_item_from_database(uint64_t staff_id) {
    return remove_item_from_table(s_hash_table, staff_id);
}

/**
 * @brief       修改员工信息
 * @param info  员工信息
 * @return      false表示失败，否则为成功
 */
bool modify_item_from_database(staff_info_t *info) {
    return modify_item_from_table(s_hash_table, info->staff_id, info);
}

/**
 * @brief           获取指定工号员工信息
 * @param staff_id  员工工号
 * @return          NULL表示失败，否则为成功
 */
staff_info_t *get_by_id_from_database(uint64_t staff_id) {
    staff_info_t *item = (staff_info_t *)get_item_by_key(s_hash_table, staff_id);
    return item;
}

/**
 * @brief       获取信息匹配的所有员工信息
 * @param info  员工信息[NULL表示通配]
 * @param count 匹配的员工数量
 * @return      NULL表示失败，否则为成功[动态申请内存，需调用方释放]
 */
staff_info_t **get_by_info_from_database(staff_info_t *info, uint64_t *count) {
    staff_info_t **items = NULL;
    items = (staff_info_t **)get_items_by_value(s_hash_table, info, count);
    return items;
}
