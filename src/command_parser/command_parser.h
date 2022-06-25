//
//  command_parser.h
//  EmployeeManager
//
//  Created by 孙康 on 2022/5/30.
//

#ifndef command_parser_h
#define command_parser_h

#include "command_execution.h"
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

bool parse_user_input(const char *input_request, query_info_t *query_info);

#endif /* command_parser_h */
