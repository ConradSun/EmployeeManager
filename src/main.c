//
//  main.c
//  EmployeeManager
//
//  Created by 孙康 on 2022/5/27.
//

#include "command_execution.h"
#include "manager_server.h"
#include "hash_table.h"
#include <unistd.h>

int main(int argc, const char * argv[]) {
    init_all_cmd_info();
    if (!init_socket_server()) {
        return -1;
    }

    while (true) {
        process_connect_request();
        process_all_queries();
    }
    
    return 0;
}
