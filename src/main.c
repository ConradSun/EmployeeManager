//
//  main.c
//  EmployeeManager
//
//  Created by 孙康 on 2022/5/27.
//

#include "command_execution.h"
#include "database_manager.h"
#include "manager_server.h"

int main(int argc, const char * argv[]) {
    create_database();
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
