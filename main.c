//
//  main.c
//  EmployeeManager
//
//  Created by 孙康 on 2022/5/27.
//

#include "hash_table.h"
#include "command_parser.h"
#include <unistd.h>

void test_hash_table(void) {
    StaffInfo common_value = {
        .name = "sunkang",
        .date = {
            .year = 2022,
            .month = 6,
            .day = 10
        },
        .department = "ZTA",
        .position = "developer"
    };
    
    g_hash_table = create_hash_table(1024);
    for (uint64_t key = 1; key < 100001; key++) {
        add_item_to_table(&g_hash_table, key, &common_value, true);
    }
    printf("count: %llu\n", g_hash_table->count);
    
    StaffInfo *value = get_item_from_table(g_hash_table, 1022);
    printf("name: %s, date: %d-%d-%d, department: %s, position: %s\n", value->name, value->date.year, value->date.month, value->date.day, value->department, value->position);
    
    common_value.date.day = 30;
    modify_item_from_table(g_hash_table, 1022, &common_value);
    value = get_item_from_table(g_hash_table, 1022);
    printf("name: %s, date: %d-%d-%d, department: %s, position: %s\n", value->name, value->date.year, value->date.month, value->date.day, value->department, value->position);
    
    for (uint64_t key = 1; key < 50001; key++) {
        remove_item_from_table(g_hash_table, key);
    }
    printf("delete 50000, count: %llu\n", g_hash_table->count);
    
    delete_hash_table(&g_hash_table);
}

int main(int argc, const char * argv[]) {
    g_hash_table = create_hash_table(1024);
    
    bool is_msg_in = false;
    while (true) {
        is_msg_in = get_input_message();
        if (is_msg_in) {
            parse_input_messgae();
        }
    }
    
    return 0;
}
