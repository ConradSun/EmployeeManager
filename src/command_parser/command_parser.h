//
//  command_parser.h
//  EmployeeManager
//
//  Created by 孙康 on 2022/5/30.
//

#ifndef command_parser_h
#define command_parser_h

#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include "command_execution.h"

void process_input_messgae(user_request_t *user_request);

#endif /* command_parser_h */
