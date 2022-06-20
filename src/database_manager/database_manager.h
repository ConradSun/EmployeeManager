//
//  database_manager.h
//  EmployeeManager
//
//  Created by 孙康 on 2022/06/10.
//

#ifndef database_manager_h
#define database_manager_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "common.h"

bool create_database(void);
void delete_database(void);
bool add_item_to_database(staff_info_t *info);
bool remove_item_from_database(uint64_t staff_id);
bool modify_item_from_database(staff_info_t *info);
staff_info_t *get_by_id_from_database(uint64_t staff_id);
staff_info_t **get_by_info_from_database(staff_info_t *info, uint64_t *count);

#endif /* database_manager_h */