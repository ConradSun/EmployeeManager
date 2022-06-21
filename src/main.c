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
    init_all_cmd_info();
    if (!create_database()) {
        return -1;
    }
    if (!init_socket_server()) {
        return -1;
    }

    while (true) {
        process_all_requests();
    }
    
    return 0;
}
