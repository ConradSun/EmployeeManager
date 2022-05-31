//
//  hash_table.c
//  EmployeeManager
//
//  Created by 孙康 on 2022/5/27.
//

#include "hash_table.h"

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
static inline void clear_value(StaffInfo *value) {
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
static inline void copy_value(StaffInfo *dst_value, const StaffInfo *src_value) {
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
 * @brief           创建哈希表
 * @param max_size  最大容量
 * @return          哈希表
 */
HashTable *create_hash_table(uint64_t max_size) {
    HashTable *hash_table = calloc(1, sizeof(HashTable));
    if (hash_table == NULL) {
        printf("Failed to calloc resources for creating hash table.\n");
        return NULL;
    }
    
    hash_table->max_size = max_size;
    hash_table->bucket_count = ((max_size / per_bucket + 1) >> 1) << 2;
    hash_table->buckets = calloc(1, sizeof(Bucket) * hash_table->bucket_count);
    if (hash_table->buckets == NULL) {
        printf("Failed to calloc resources for buckets.\n");
        free(hash_table);
        return NULL;
    }
    
    return hash_table;
}

/**
 * @brief               删除哈希表
 * @param hash_table    哈希表
 */
void delete_hash_table(HashTable **hash_table) {
    if (hash_table == NULL || *hash_table == NULL) {
        printf("Try to delete empty hash table.\n");
        return;
    }
    
    HashTable *table = *hash_table;
    for (uint64_t i = 0; i < table->bucket_count; ++i) {
        Bucket *bucket = &table->buckets[i];
        Node *current_node = bucket->head;
        Node *next_node = bucket->head;
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
HashTable *enlarge_hash_table(HashTable *old_table) {
    if (old_table == NULL) {
        printf("Try to enlarge empty hash table.\n");
        return NULL;
    }
    
    HashTable *new_table = create_hash_table(old_table->max_size * enlarge_factor);
    if (new_table == NULL) {
        return NULL;
    }
    
    for (uint64_t i = 0; i < old_table->bucket_count; ++i) {
        Bucket *bucket = &old_table->buckets[i];
        Node *current_node = bucket->head;
        Node *next_node = bucket->head;
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
static bool find_item_from_table(HashTable *hash_table, uint64_t key, Node **current, Node **last) {
    if (hash_table == NULL || key == 0) {
        printf("Failed to find item for invalid param.\n");
        return false;
    }
    
    Bucket *bucket = &hash_table->buckets[hash_code(key, hash_table->bucket_count)];
    Node *temp_node = bucket->head;
    Node *last_node = bucket->head;
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
bool add_item_to_table(HashTable **hash_table, uint64_t key, StaffInfo *value, bool is_copy) {
    if (hash_table == NULL || *hash_table == NULL || key == 0 || value == NULL) {
        printf("Failed to find item for invalid param.\n");
        return false;
    }
    
    HashTable *table = *hash_table;
    if (find_item_from_table(table, key, NULL, NULL)) {
        printf("Failed to add the item for already added.\n");
        return false;
    }
    
    if (table->count == table->max_size) {
        table = enlarge_hash_table(table);
        if (table == NULL) {
            return false;
        }
        *hash_table = table;
    }
    
    Node *new_node = calloc(1, sizeof(Node));
    if (new_node == NULL) {
        printf("Failed to calloc resources for new node.\n");
        return false;
    }
    new_node->key = key;
    if (is_copy) {
        new_node->value = calloc(1, sizeof(StaffInfo));
        copy_value(new_node->value, value);
    }
    else {
        new_node->value = value;
    }
    
    Bucket *bucket = &table->buckets[hash_code(key, table->bucket_count)];
    if (bucket->head == NULL) {
        bucket->head = new_node;
    }
    else {
        Node *tail_node = bucket->head;
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
bool remove_item_from_table(HashTable *hash_table, uint64_t key) {
    Node *current = NULL;
    Node *last = NULL;
    if (!find_item_from_table(hash_table, key, &current, &last)) {
        printf("Failed to remove item for not here.\n");
        return false;
    }
    
    if (last == current) {
        Bucket *bucket = &hash_table->buckets[hash_code(key, hash_table->bucket_count)];
        bucket->head = current->next;
    }
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
bool modify_item_from_table(HashTable *hash_table, uint64_t key, StaffInfo *value) {
    Node *node = NULL;
    if (!find_item_from_table(hash_table, key, &node, NULL)) {
        printf("Failed to modify item for not here.\n");
        return false;
    }
    copy_value(node->value, value);
    return true;
}

/**
 * @brief               从哈希表获取指定项
 * @param hash_table    哈希表
 * @param key           待获取项键
 * @return              所查询项值
 */
StaffInfo *get_item_from_table(HashTable *hash_table, uint64_t key) {
    Node *node = NULL;
    if (!find_item_from_table(hash_table, key, &node, NULL)) {
        printf("Failed to get item for not here.\n");
        return NULL;
    }
    
    return node->value;
}

void print_staff_info(StaffInfo *value) {
    printf("name: %s, date: %04d-%02d-%02d, department: %s, position: %s\n", value->name, value->date.year, value->date.month, value->date.day, value->department, value->position);
}

void print_all_of_table(HashTable *hash_table) {
    for (uint64_t i = 0; i < hash_table->bucket_count; ++i) {
        Bucket *bucket = &hash_table->buckets[i];
        Node *current_node = bucket->head;
        while (current_node != NULL) {
            print_staff_info(current_node->value);
            current_node = current_node->next;
        }
    }
}
