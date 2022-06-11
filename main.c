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
    staff_info_t common_value = {
        .name = "sunkang",
        .date = {
            .year = 2022,
            .month = 6,
            .day = 10
        },
        .department = "ZTA",
        .position = "developer"
    };
    
    hash_table_t *hash_table = create_hash_table(1024);
    for (uint64_t key = 1; key < 100001; key++) {
        add_item_to_table(&hash_table, key, &common_value, true);
    }
    printf("count: %llu\n", hash_table->count);
    
    get_item_from_table(hash_table, 1022);
    common_value.date.day = 30;
    modify_item_from_table(hash_table, 1022, &common_value);
    get_item_from_table(hash_table, 1022);
    
    for (uint64_t key = 1; key < 50001; key++) {
        remove_item_from_table(hash_table, key);
    }
    printf("delete 50000, count: %llu\n", hash_table->count);
    
    delete_hash_table(&hash_table);
}

int main(int argc, const char * argv[]) {
    bool is_msg_in = false;
    while (true) {
        is_msg_in = get_input_message();
        if (is_msg_in) {
            parse_input_messgae();
        }
    }
    
    return 0;
}
