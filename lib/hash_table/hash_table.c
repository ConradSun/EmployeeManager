//
//  hash_table.c
//  EmployeeManager
//
//  Created by 孙康 on 2022/5/27.
//

#include "hash_table.h"
#include "log.h"

/**
 * @brief 哈希表链表结点
 */
typedef struct entry_node {
    uint64_t key;           // 主键
    void *value;            // 映射信息
    struct entry_node *next;
} entry_node_t;

/**
 * @brief 哈希桶
 */
typedef struct {
    entry_node_t *head;
} hash_bucket_t;

/**
 * @brief 哈希表
 */
struct hash_table {
    uint64_t count;         // 当前数量
    uint64_t value_size;    // 存储信息大小
    uint64_t max_size;      // 最大容量
    uint64_t bucket_count;  // 桶数量
    hash_bucket_t *buckets; // 桶数组

    clear_value_callback clear_func;    // 值清理接口
    copy_value_callback copy_func;      // 值拷贝接口
    is_value_equal_callback match_func; // 值匹配接口
};

static const uint8_t per_bucket = 4;        // 哈希桶容量
static const float enlarge_factor = 1.5;    // 扩容倍数
log_level_t g_log_level = LOG_INFO;         // 当前日志等级[默认INFO级别]

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
 * @brief               创建哈希表
 * @param max_size      最大容量
 * @param value_size    存储信息大小
 * @param clear_func    值清理函数
 * @param copy_func     值拷贝函数
 * @param match_func    值匹配函数
 * @return              哈希表
 */
hash_table_t *create_hash_table(
    uint64_t max_size, 
    uint64_t value_size, 
    clear_value_callback clear_func, 
    copy_value_callback copy_func, 
    is_value_equal_callback match_func) {
    if (max_size == 0 || value_size == 0 || clear_func == NULL || copy_func == NULL || match_func == NULL) {
        LOG_C(LOG_ERROR, "Failed to create hash table for invalid param.")
        return NULL;
    }

    hash_table_t *hash_table = calloc(1, sizeof(hash_table_t));
    if (hash_table == NULL) {
        LOG_C(LOG_ERROR, "Failed to calloc resources for creating hash table.")
        return NULL;
    }
    
    hash_table->max_size = max_size;
    hash_table->value_size = value_size;
    hash_table->clear_func = clear_func;
    hash_table->copy_func = copy_func;
    hash_table->match_func = match_func;

    // 保证桶数量为偶数个
    hash_table->bucket_count = (((max_size + per_bucket) / per_bucket) >> 1) << 1;
    hash_table->buckets = calloc(1, sizeof(hash_bucket_t) * hash_table->bucket_count);
    if (hash_table->buckets == NULL) {
        LOG_C(LOG_ERROR, "Failed to calloc resources for buckets.")
        FREE(hash_table)
        return NULL;
    }
    LOG_C(LOG_DEBUG, "Create hash table successfully.")
    
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
            table->clear_func(current_node->value);
            FREE(current_node)
            current_node = next_node;
        }
    }
    FREE(table->buckets)
    FREE(table)
    *hash_table = NULL;
    LOG_C(LOG_DEBUG, "Delete hash table successfully.")
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
    
    hash_table_t *new_table = create_hash_table(old_table->max_size*enlarge_factor, old_table->value_size, 
        old_table->clear_func, old_table->copy_func, old_table->match_func);
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
    LOG_C(LOG_DEBUG, "Enlarge hash table successfully.")
    
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
 * @param key           主键
 * @param value         待添加项值
 * @param is_copy       是否深拷贝值
 * @return              false表示失败，否则为成功
 */
bool add_item_to_table(hash_table_t **hash_table, uint64_t key, void *value, bool is_copy) {
    if (hash_table == NULL || *hash_table == NULL || key == 0 || value == NULL) {
        LOG_C(LOG_ERROR, "Failed to add item for invalid param.");
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
        new_node->value = calloc(1, table->value_size);
        table->copy_func(new_node->value, value);
    }
    else {
        new_node->value = value;
    }
    
    hash_bucket_t *bucket = &table->buckets[hash_code(new_node->key, table->bucket_count)];
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
    LOG_C(LOG_DEBUG, "After adding, number of items in hash table is [%llu].", table->count)
    
    return true;
}

/**
 * @brief               从哈希表删除项
 * @param hash_table    哈希表
 * @param key           待删除项键
 * @return              false表示失败，否则为成功
 */
bool remove_item_from_table(hash_table_t *hash_table, uint64_t key) {
    if (hash_table == NULL || key == 0) {
        LOG_C(LOG_ERROR, "Failed to remove item for invalid param.");
        return false;
    }

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
    hash_table->clear_func(current->value);
    FREE(current)
    hash_table->count--;
    LOG_C(LOG_DEBUG, "After removing, number of items in hash table is [%llu].", hash_table->count)
    
    return true;
}

/**
 * @brief               从哈希表更新指定项
 * @param hash_table    哈希表
 * @param key           主键
 * @param value         更新后值
 * @return              false表示失败，否则为成功
 */
bool modify_item_from_table(hash_table_t *hash_table, uint64_t key, void *value) {
    if (hash_table == NULL || key == 0 || value == NULL) {
        LOG_C(LOG_ERROR, "Failed to modify item for invalid param.");
        return false;
    }

    entry_node_t *node = NULL;
    if (!find_item_from_table(hash_table, key, &node, NULL)) {
        LOG_C(LOG_ERROR, "Failed to modify item for not here.");
        return false;
    }
    hash_table->copy_func(node->value, value);
    return true;
}

/**
 * @brief               从哈希表获取指定项[关键字工号]
 * @param hash_table    哈希表
 * @param key           待获取项键
 * @return              指定项信息
 */
void *get_item_by_key(hash_table_t *hash_table, uint64_t key) {
    if (hash_table == NULL || key == 0) {
        LOG_C(LOG_ERROR, "Failed to get item for invalid param.");
        return NULL;
    }

    entry_node_t *node = NULL;
    if (!find_item_from_table(hash_table, key, &node, NULL)) {
        LOG_C(LOG_ERROR, "Failed to get item for not here.");
        return NULL;
    }
    return node->value;
}

/**
 * @brief               从哈希表获取指定项[匹配指定信息]
 * @param hash_table    哈希表
 * @param value         待匹配项
 * @param count         匹配成功项个数地址
 * @return              匹配成功项信息
 */
void **get_items_by_info(hash_table_t *hash_table, void *value, uint64_t *count) {
    if (hash_table == NULL || value == NULL || count == NULL) {
        return NULL;
    }
    
    *count = 0;
    void **info = calloc(1, sizeof(void *)*hash_table->count);
    if (info == NULL) {
        return NULL;
    }

    // 遍历输出所有匹配项，无序输出
    for (uint64_t i = 0; i < hash_table->bucket_count; ++i) {
        hash_bucket_t *bucket = &hash_table->buckets[i];
        entry_node_t *current_node = bucket->head;
        while (current_node != NULL) {
            if (hash_table->match_func(value, current_node->value)) {
                info[*count] = current_node->value;
                (*count)++;
            }
            current_node = current_node->next;
        }
    }

    if (*count == 0) {
        LOG_C(LOG_ERROR, "No matching items found.");
    }
    return info;
}

/**
 * @brief 遍历输出所有项信息
 * @param count 总个数地址
 * @return      所有项信息
 */
void **get_all_items_from_table(hash_table_t *hash_table, uint64_t *count) {
    if (hash_table == NULL || count == NULL) {
        return NULL;
    }

    *count = 0;
    void **info = calloc(1, sizeof(void *)*hash_table->count);
    if (info == NULL) {
        return NULL;
    }

    for (uint64_t i = 0; i < hash_table->bucket_count; ++i) {
        hash_bucket_t *bucket = &hash_table->buckets[i];
        entry_node_t *current_node = bucket->head;
        while (current_node != NULL) {
            info[*count] = current_node->value;
            (*count)++;
            current_node = current_node->next;
        }
    }

    return info;
}
