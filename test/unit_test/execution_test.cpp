//
//  execution_test.c
//  EmployeeManager
//
//  Created by 孙康 on 2022/6/25.
//

#ifdef __cplusplus
extern "C" {
#endif

#include "command_execution.h"
#include "database_manager.h"

#ifdef __cplusplus
};
#endif

#include <gtest/gtest.h>

class CommandExecTest : public testing::Test {
    virtual void SetUp() override {
        staff_info_t info = {
            .staff_id = 10086,
            .name = (char *)"Lisi",
            .department = (char *)"CWPP",
            .position = NULL
        };
        struct tm tm_time = {0};
        strptime((char *)"2022-06-25 09:00:00", "%Y-%m-%d %H:%M:%S", &tm_time);
        info.date = mktime(&tm_time);
        create_database();
        add_item_to_database(&info);
        info.staff_id = 10087;
        info.name = (char *)"WangWu";
        strptime((char *)"2022-06-24 09:00:00", "%Y-%m-%d %H:%M:%S", &tm_time);
        info.date = mktime(&tm_time);
        add_item_to_database(&info);
    }
    virtual void TearDown() override {
        delete_database();
    }
};

TEST_F(CommandExecTest, Add) {
    query_info_t query = {
        .command = CMD_ADD,
        .info = {
            .staff_id = 10088,
            .name = (char *)"ZhangSan",
        },
    };
    user_request_t request;

    bzero(&request, sizeof(user_request_t));
    execute_input_command(&query, &request);
    EXPECT_EQ(strcmp(request.result, "The staff [10088] is added."), 0);
    
    bzero(&request, sizeof(user_request_t));
    request.result[0] = 'q';
    request.result[1] = '\0';
    execute_input_command(NULL, &request);
    EXPECT_EQ(strcmp(request.result, "q"), 0);
}

TEST_F(CommandExecTest, Del) {
    query_info_t query = {
        .command = CMD_DEL,
        .info = {
            .staff_id = 10086,
        },
    };
    user_request_t request;

    bzero(&request, sizeof(user_request_t));
    execute_input_command(&query, &request);
    EXPECT_EQ(strcmp(request.result, "The staff [10086] is removed."), 0);

    bzero(&request, sizeof(user_request_t));
    query.info.staff_id = 10089;
    execute_input_command(&query, &request);
    EXPECT_FALSE(request.is_success);

    bzero(&request, sizeof(user_request_t));
    query.is_opt_all = true;
    execute_input_command(&query, &request);
    EXPECT_EQ(strcmp(request.result, "All staffs are removed."), 0);
}

TEST_F(CommandExecTest, Mod) {
    query_info_t query = {
        .command = CMD_MOD,
        .info = {
            .staff_id = 10086,
            .name = (char *)"ZhangSan",
        },
    };
    user_request_t request;

    bzero(&request, sizeof(user_request_t));
    execute_input_command(&query, &request);
    EXPECT_EQ(strcmp(request.result, "Info of the staff [10086] is modified."), 0);

    bzero(&request, sizeof(user_request_t));
    query.info.staff_id = 10089;
    execute_input_command(&query, &request);
    EXPECT_FALSE(request.is_success);
}

TEST_F(CommandExecTest, Get) {
    query_info_t query = {
        .command = CMD_GET,
        .info = {
            .staff_id = 10086,
        },
    };
    user_request_t request;
    struct tm tm_time = {0};

    bzero(&request, sizeof(user_request_t));
    execute_input_command(&query, &request);
    EXPECT_EQ(strcmp(request.result, "staff id: 10086, name: Lisi, date: 2022-06-25 09:00:00, department: CWPP, position: (null).\n"), 0);

    bzero(&request, sizeof(user_request_t));
    query.info.staff_id = 10089;
    execute_input_command(&query, &request);
    EXPECT_EQ(strcmp(request.result, "Staff with id [10089] is not found."), 0);

    bzero(&request, sizeof(user_request_t));
    query.info.staff_id = 0;
    strptime((char *)"2022-06-25 09:00:00", "%Y-%m-%d %H:%M:%S", &tm_time);
    query.info.date = mktime(&tm_time);
    execute_input_command(&query, &request);
    EXPECT_EQ(strcmp(request.result, "staff id: 10086, name: Lisi, date: 2022-06-25 09:00:00, department: CWPP, position: (null).\n"), 0);
    query.info.date = 0;

    bzero(&request, sizeof(user_request_t));
    query.is_opt_all = true;
    query.sort_type = SORT_ID;
    execute_input_command(&query, &request);
    EXPECT_EQ(strcmp(request.result, "staff id: 10086, name: Lisi, date: 2022-06-25 09:00:00, department: CWPP, position: (null).\nstaff id: 10087, name: WangWu, date: 2022-06-24 09:00:00, department: CWPP, position: (null).\n"), 0);

    bzero(&request, sizeof(user_request_t));
    query.info.staff_id = 0;
    query.info.department = (char *)"CWPP";
    query.sort_type = SORT_DATE;
    execute_input_command(&query, &request);
    EXPECT_EQ(strcmp(request.result, "staff id: 10087, name: WangWu, date: 2022-06-24 09:00:00, department: CWPP, position: (null).\nstaff id: 10086, name: Lisi, date: 2022-06-25 09:00:00, department: CWPP, position: (null).\n"), 0);
}

TEST_F(CommandExecTest, Log) {
    query_info_t query = {
        .command = CMD_LOG,
    };
    user_request_t request;

    bzero(&request, sizeof(user_request_t));
    execute_input_command(&query, &request);
    EXPECT_TRUE(request.is_success);
}

TEST_F(CommandExecTest, Help) {
    query_info_t query = {
        .command = CMD_HELP,
    };
    user_request_t request;

    bzero(&request, sizeof(user_request_t));
    execute_input_command(&query, &request);
    EXPECT_TRUE(request.is_success);
}

TEST_F(CommandExecTest, Exit) {
    query_info_t query = {
        .command = CMD_EXIT,
    };
    user_request_t request;

    bzero(&request, sizeof(user_request_t));
    execute_input_command(&query, &request);
    EXPECT_TRUE(request.is_success);
}
