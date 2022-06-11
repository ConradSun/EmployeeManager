//
//  em_log.h
//  EmployeeManager
//
//  Created by 孙康 on 2022/06/10.
//

#ifndef em_log_h
#define em_log_h

/**
* @brief 日志打印等级
*/
typedef enum {
    LOG_OFF,        // 关闭日志
    LOG_FAULT,      // 严重错误
    LOG_ERROR,      // 错误信息
    LOG_INFO,       // 详细信息
    LOG_DEBUG,      // 调试信息
} log_level_t;

/**
* @brief 当前日志等级
*/
#define LOG_LEVEL   LOG_INFO

/**
* @brief C接口日志
*/
#define LOG_C(level, format, ...) \
    if (LOG_LEVEL >= level) { \
        printf("%s %s:%d [-] " format "\n", \
        #level, __func__, __LINE__, ##__VA_ARGS__); \
    } \

#endif /* em_log_h */
