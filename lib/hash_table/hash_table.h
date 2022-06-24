//
//  hash_table.h
//  EmployeeManager
//
//  Created by 孙康 on 2022/5/27.
//

#ifndef hash_table_h
#define hash_table_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define FREE(ptr)   if (ptr != NULL) {free(ptr); ptr = NULL;}

typedef struct hash_table hash_table_t;     // 哈希表
typedef void(*clear_value_callback)(void *value);                           // 值清理回调
typedef void(*copy_value_callback)(void *dst, const void *src);             // 值拷贝回调
typedef bool(*is_value_equal_callback)(const void *src, const void *dst);   // 值匹配回调

hash_table_t *create_hash_table(uint64_t max_size, uint64_t value_size, clear_value_callback clear_func, copy_value_callback copy_func, is_value_equal_callback match_func);
void delete_hash_table(hash_table_t **hash_table);
hash_table_t *enlarge_hash_table(hash_table_t *old_table);

bool add_item_to_table(hash_table_t **hash_table, uint64_t key, void *value, bool is_copy);
bool remove_item_from_table(hash_table_t *hash_table, uint64_t key);
bool modify_item_from_table(hash_table_t *hash_table, uint64_t key, void *value);
void *get_item_by_key(hash_table_t *hash_table, uint64_t key);
void **get_items_by_value(hash_table_t *hash_table, void *value, uint64_t *count);
uint64_t get_count_from_table(hash_table_t *hash_table);

#endif /* hash_table_h */
