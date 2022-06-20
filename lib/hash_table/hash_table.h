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
#include "common.h"

typedef struct hash_table hash_table_t;     // 哈希表

hash_table_t *create_hash_table(uint64_t max_size);
void delete_hash_table(hash_table_t **hash_table);
hash_table_t *enlarge_hash_table(hash_table_t *old_table);

bool add_item_to_table(hash_table_t **hash_table, staff_info_t *value, bool is_copy);
bool remove_item_from_table(hash_table_t *hash_table, uint64_t key);
bool modify_item_from_table(hash_table_t *hash_table, staff_info_t *value);
staff_info_t *get_item_by_key(hash_table_t *hash_table, uint64_t key);
staff_info_t **get_items_by_info(hash_table_t *hash_table, staff_info_t *value, uint64_t *count);
staff_info_t **get_all_items_from_table(hash_table_t *hash_table, uint64_t *count);

#endif /* hash_table_h */
