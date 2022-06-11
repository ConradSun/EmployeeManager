//
//  hash_table.c
//  EmployeeManager
//
//  Created by 孙康 on 2022/5/27.
//

#include "hash_table.h"
#include "em_log.h"

static const uint8_t per_bucket = 4;      // 哈希桶容量
static const float enlarge_factor = 1.5;  // 扩容倍数

/**
 * @brief               生成哈希值
 * @param key           哈希键
 * @param bucket_count  桶数量
 * @return              哈希值
 */
static inline uint64_t hash_code(uint64_t key, uint64_t bucket_count) {
    uint64_t hash = key * 11400714819323198549UL;
    return hash % bucket_count;
}

/**
 * @brief       清理存储值
 * @param value 待清理值
 */
static inline void clear_value(staff_info_t *value) {
    if (value != NULL) {
        FREE(value->name)
        FREE(value->position)
        FREE(value->department)
        FREE(value)
    }
}

/**
 * @brief           拷贝存储值
 * @param dst_value 拷贝至
 * @param src_value 拷贝于
 */
static inline void copy_value(staff_info_t *dst_value, const staff_info_t *src_value) {
    if (dst_value != NULL && src_value != NULL) {
        if (src_value->date.year != 0) {
            dst_value->date = src_value->date;
        }
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
 * @brief       比较日期是否相同
 * @param date1 日期1
 * @param date2 日期2
 * @return      false表示不同，否则为相同
 */
static inline bool is_date_equal(short_date_t date1, short_date_t date2) {
    // date1全0表示通配
    if (date1.year == 0 && date1.month == 0 && date1.day == 0) {
        return true;
    }

    if (date1.year == date2.year && date1.month == date2.month && date1.day == date2.day) {
        return true;
    }
    return false;
}

/**
 * @brief           比较字符串是否相同
 * @param src_str   字符串1
 * @param dst_str   字符串2
 * @return          false表示不同，否则为相同
 */
static bool is_string_equal(const char *src_str, const char *dst_str) {
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
 * @brief           比较员工信息是否相同
 * @param src_value 信息1
 * @param dst_value 信息2
 * @return          false表示不同，否则为相同
 */
static bool is_value_equal(const staff_info_t *src_value, const staff_info_t *dst_value) {
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
    if (!is_date_equal(src_value->date, dst_value->date)) {
        return false;
    }
    
    return true;
}

/**
 * @brief               打印指定员工信息
 * @param job_number    工号
 * @param value         员工信息
 */
static void print_staff_info(uint64_t job_number, staff_info_t *value) {
    LOG_C(LOG_INFO, "job_number: %llu, name: %s, date: %04d-%02d-%02d, department: %s, position: %s.", job_number, value->name, value->date.year, value->date.month, value->date.day, value->department, value->position)
}

/**
 * @brief           创建哈希表
 * @param max_size  最大容量
 * @return          哈希表
 */
hash_table_t *create_hash_table(uint64_t max_size) {
    hash_table_t *hash_table = calloc(1, sizeof(hash_table_t));
    if (hash_table == NULL) {
        LOG_C(LOG_ERROR, "Failed to calloc resources for creating hash table.")
        return NULL;
    }
    
    hash_table->max_size = max_size;
    // 保证桶数量为偶数个
    hash_table->bucket_count = (((max_size + per_bucket) / per_bucket) >> 1) << 1;
    hash_table->buckets = calloc(1, sizeof(hash_bucket_t) * hash_table->bucket_count);
    if (hash_table->buckets == NULL) {
        LOG_C(LOG_ERROR, "Failed to calloc resources for buckets.")
        free(hash_table);
        return NULL;
    }
    
    return hash_table;
}

/**
 * @brief               删除哈希表
 * @param hash_table    哈希表
 */
void delete_hash_table(hash_table_t **hash_table) {
    if (hash_table == NULL || *hash_table == NULL) {
        LOG_C(LOG_ERROR, "Try to delete empty hash table.")
        return;
    }
    
    hash_table_t *table = *hash_table;
    for (uint64_t i = 0; i < table->bucket_count; ++i) {
        hash_bucket_t *bucket = &table->buckets[i];
        entry_node_t *current_node = bucket->head;
        entry_node_t *next_node = bucket->head;
        while (current_node != NULL) {
            next_node = current_node->next;
            clear_value(current_node->value);
            FREE(current_node)
            current_node = next_node;
        }
    }
    FREE(table->buckets)
    FREE(table)
    *hash_table = NULL;
}

/**
 * @brief           扩容哈希表
 * @param old_table 旧哈希表
 * @return          扩容后哈希表
 */
hash_table_t *enlarge_hash_table(hash_table_t *old_table) {
    if (old_table == NULL) {
        LOG_C(LOG_ERROR, "Try to enlarge empty hash table.")
        return NULL;
    }
    
    hash_table_t *new_table = create_hash_table(old_table->max_size * enlarge_factor);
    if (new_table == NULL) {
        return NULL;
    }
    
    // 转移旧表值至新表，浅拷贝，仅复制指针
    for (uint64_t i = 0; i < old_table->bucket_count; ++i) {
        hash_bucket_t *bucket = &old_table->buckets[i];
        entry_node_t *current_node = bucket->head;
        entry_node_t *next_node = bucket->head;
        while (current_node != NULL) {
            next_node = current_node->next;
            add_item_to_table(&new_table, current_node->key, current_node->value, false);
            FREE(current_node)
            current_node = next_node;
        }
    }
    FREE(old_table->buckets)
    FREE(old_table)
    
    return new_table;
}

/**
 * @brief               查找指定项
 * @param hash_table    哈希表
 * @param key           指定项键
 * @param current       存储指定项当前结点
 * @param last          存储指定项上一结点
 * @return              false表示失败，否则为成功
 */
static bool find_item_from_table(hash_table_t *hash_table, uint64_t key, entry_node_t **current, entry_node_t **last) {
    if (hash_table == NULL || key == 0) {
        LOG_C(LOG_ERROR, "Failed to find item for invalid param.");
        return false;
    }
    
    hash_bucket_t *bucket = &hash_table->buckets[hash_code(key, hash_table->bucket_count)];
    entry_node_t *temp_node = bucket->head;
    entry_node_t *last_node = bucket->head;
    while (temp_node != NULL) {
        if (temp_node->key == key) {
            break;
        }
        last_node = temp_node;
        temp_node = temp_node->next;
    }
    
    if (temp_node == NULL) {
        return false;
    }
    if (current != NULL) {
        *current = temp_node;
    }
    if (last != NULL) {
        *last = last_node;
    }
    return true;
}

/**
 * @brief               从哈希表添加项
 * @param hash_table    哈希表
 * @param key           待添加项键
 * @param value         待添加项值
 * @param is_copy       是否深拷贝值
 * @return              false表示失败，否则为成功
 */
bool add_item_to_table(hash_table_t **hash_table, uint64_t key, staff_info_t *value, bool is_copy) {
    if (hash_table == NULL || *hash_table == NULL || key == 0 || value == NULL) {
        LOG_C(LOG_ERROR, "Failed to find item for invalid param.");
        return false;
    }
    
    hash_table_t *table = *hash_table;
    if (find_item_from_table(table, key, NULL, NULL)) {
        LOG_C(LOG_ERROR, "Failed to add the item for already added.");
        return false;
    }
    
    if (table->count > table->max_size) {
        table = enlarge_hash_table(table);
        if (table == NULL) {
            return false;
        }
        *hash_table = table;
    }
    
    entry_node_t *new_node = calloc(1, sizeof(entry_node_t));
    if (new_node == NULL) {
        LOG_C(LOG_ERROR, "Failed to calloc resources for new node.");
        return false;
    }
    new_node->key = key;
    if (is_copy) {
        new_node->value = calloc(1, sizeof(staff_info_t));
        copy_value(new_node->value, value);
    }
    else {
        new_node->value = value;
    }
    
    hash_bucket_t *bucket = &table->buckets[hash_code(key, table->bucket_count)];
    // 头结点无值存储至头
    if (bucket->head == NULL) {
        bucket->head = new_node;
    }
    // 头结点有值存储至尾
    else {
        entry_node_t *tail_node = bucket->head;
        while (tail_node->next != NULL) {
            tail_node = tail_node->next;
        }
        tail_node->next = new_node;
    }
    table->count++;
    
    return true;
}

/**
 * @brief               从哈希表删除项
 * @param hash_table    哈希表
 * @param key           待删除项键
 * @return              false表示失败，否则为成功
 */
bool remove_item_from_table(hash_table_t *hash_table, uint64_t key) {
    entry_node_t *current = NULL;
    entry_node_t *last = NULL;
    if (!find_item_from_table(hash_table, key, &current, &last)) {
        LOG_C(LOG_ERROR, "Failed to remove item for not here.");
        return false;
    }
    
    // 待删除结点为头结点
    if (last == current) {
        hash_bucket_t *bucket = &hash_table->buckets[hash_code(key, hash_table->bucket_count)];
        bucket->head = current->next;
    }
    // 待删除结点为中间结点
    else {
        last->next = current->next;
    }
    clear_value(current->value);
    FREE(current)
    hash_table->count--;
    
    return true;
}

/**
 * @brief               从哈希表更新指定项
 * @param hash_table    哈希表
 * @param key           待更新项键
 * @param value         更新后值
 * @return              false表示失败，否则为成功
 */
bool modify_item_from_table(hash_table_t *hash_table, uint64_t key, staff_info_t *value) {
    entry_node_t *node = NULL;
    if (!find_item_from_table(hash_table, key, &node, NULL)) {
        LOG_C(LOG_ERROR, "Failed to modify item for not here.");
        return false;
    }
    copy_value(node->value, value);
    return true;
}

/**
 * @brief               从哈希表获取指定项[关键字工号]
 * @param hash_table    哈希表
 * @param key           待获取项键
 */
void get_item_from_table(hash_table_t *hash_table, uint64_t key) {
    entry_node_t *node = NULL;
    if (!find_item_from_table(hash_table, key, &node, NULL)) {
        LOG_C(LOG_ERROR, "Failed to get item for not here.");
        return;
    }
    print_staff_info(key, node->value);
}

/**
 * @brief               从哈希表获取指定项[匹配指定信息]
 * @param hash_table    哈希表
 * @param value         待匹配项
 */
void get_items_by_info(hash_table_t *hash_table, staff_info_t *value) {
    if (hash_table == NULL || value == NULL) {
        return;
    }
    
    uint64_t count = 0;
    // 遍历输出所有匹配项，无序输出
    for (uint64_t i = 0; i < hash_table->bucket_count; ++i) {
        hash_bucket_t *bucket = &hash_table->buckets[i];
        entry_node_t *current_node = bucket->head;
        while (current_node != NULL) {
            if (is_value_equal(value, current_node->value)) {
                print_staff_info(current_node->key, current_node->value);
                count++;
            }
            current_node = current_node->next;
        }
    }

    if (count == 0) {
        LOG_C(LOG_ERROR, "No matching items found.");
    }
}

/**
 * @brief 遍历输出所有项信息
 */
void print_all_of_table(hash_table_t *hash_table) {
    if (hash_table == NULL) {
        return;
    }

    for (uint64_t i = 0; i < hash_table->bucket_count; ++i) {
        hash_bucket_t *bucket = &hash_table->buckets[i];
        entry_node_t *current_node = bucket->head;
        while (current_node != NULL) {
            print_staff_info(current_node->key, current_node->value);
            current_node = current_node->next;
        }
    }
}
