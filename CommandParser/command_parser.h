//
//  command_parser.h
//  EmployeeManager
//
//  Created by 孙康 on 2022/5/30.
//

#ifndef command_parser_h
#define command_parser_h

#include "hash_table.h"
#include <unistd.h>

/**
 * @brief 有效指令
 */
typedef enum {
    NUL,
    
    ADD,
    DEL,
    MOD,
    GET,
    
    HELP,
    EXIT,
    MAX_CMD
} Command;

/**
 * @brief 员工信息类型
 */
typedef enum {
    NAME,
    DATE,
    DEPT,
    POS,
    MAX_TYPE
} InfoType;

extern HashTable *g_hash_table;

bool get_input_message(void);
void parse_input_messgae(void);

#endif /* command_parser_h */
