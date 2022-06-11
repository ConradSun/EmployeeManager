//
//  main.c
//  EmployeeManager
//
//  Created by 孙康 on 2022/5/27.
//

#include "command_execution.h"
#include "command_parser.h"
#include "hash_table.h"
#include <unistd.h>

int main(int argc, const char * argv[]) {
    bool is_msg_in = false;
    init_all_cmd_info();
    while (true) {
        is_msg_in = get_input_message();
        if (is_msg_in) {
            parse_input_messgae();
        }
    }
    
    return 0;
}
