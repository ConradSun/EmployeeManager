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
#include <strings.h>

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
    char *name;
    short_date_t date;
    char *department;
    char *position;
} staff_info_t;

/**
 * @brief 哈希表链表结点
 */
typedef struct entry_node {
    uint64_t key;           // 员工工号，查询关键字
    staff_info_t *value;    // 员工信息
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
typedef struct {
    uint64_t count;
    uint64_t max_size;
    uint64_t bucket_count;
    hash_bucket_t *buckets;
} hash_table_t;

hash_table_t *create_hash_table(uint64_t max_size);
void delete_hash_table(hash_table_t **hash_table);
hash_table_t *enlarge_hash_table(hash_table_t *old_table);

bool add_item_to_table(hash_table_t **hash_table, uint64_t key, staff_info_t *value, bool is_copy);
bool remove_item_from_table(hash_table_t *hash_table, uint64_t key);
bool modify_item_from_table(hash_table_t *hash_table, uint64_t key, staff_info_t *value);
void get_item_from_table(hash_table_t *hash_table, uint64_t key);
void get_items_by_info(hash_table_t *hash_table, staff_info_t *value);

void print_all_of_table(hash_table_t *hash_table);

#endif /* hash_table_h */
