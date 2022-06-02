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
} Date;

/**
 * @brief 员工信息
 */
typedef struct {
    char *name;
    Date date;
    char *department;
    char *position;
} StaffInfo;

/**
 * @brief 哈希表链表结点
 */
typedef struct _Node {
    uint64_t key;       // 员工工号，查询关键字
    StaffInfo *value;   // 员工信息
    struct _Node *next;
} Node;

/**
 * @brief 哈希桶
 */
typedef struct {
    Node *head;
} Bucket;

/**
 * @brief 哈希表
 */
typedef struct {
    uint64_t count;
    uint64_t max_size;
    uint64_t bucket_count;
    Bucket *buckets;
} HashTable;

HashTable *create_hash_table(uint64_t max_size);
void delete_hash_table(HashTable **hash_table);
HashTable *enlarge_hash_table(HashTable *old_table);

bool add_item_to_table(HashTable **hash_table, uint64_t key, StaffInfo *value, bool is_copy);
bool remove_item_from_table(HashTable *hash_table, uint64_t key);
bool modify_item_from_table(HashTable *hash_table, uint64_t key, StaffInfo *value);
void get_item_from_table(HashTable *hash_table, uint64_t key);
void get_items_by_info(HashTable *hash_table, StaffInfo *value);

void print_all_of_table(HashTable *hash_table);

#endif /* hash_table_h */
