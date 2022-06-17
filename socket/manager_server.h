//
//  manager_server.h
//  EmployeeManager
//
//  Created by 孙康 on 2022/6/16.
//

#ifndef manager_server_h
#define manager_server_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

bool init_socket_server(void);
bool process_connect_request(void);
void process_all_queries(void);
void destroy_all_connection(void);

#endif /* manager_server_h */
