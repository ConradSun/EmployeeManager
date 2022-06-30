//
//  parser_test.c
//  EmployeeManager
//
//  Created by 孙康 on 2022/6/25.
//

#ifdef __cplusplus
extern "C" {
#endif

#include "command_execution.h"
#include "command_parser.h"
#include "common.h"

extern uint8_t get_split_params(const char *string, char params[][BUFSIZ]);
extern bool is_date_valid(const char *string);
extern bool is_name_valid(const char *string);
extern sort_type_t parse_sort_type(const char *string);
extern bool parse_log_level(const char *string);
extern info_type_t parse_info_type(const char *string, size_t *end);
extern bool parse_staff_info(const char *string, staff_info_t *info);
extern user_command_t parse_input_command(const char *string);

#ifdef __cplusplus
};
#endif

#include <gtest/gtest.h>

class CommandParserTest : public testing::Test {
};

TEST_F(CommandParserTest, SplitInput) {
    int count = 0;
    int max = 32;
    char params[max][BUFSIZ];
    char string[BUFSIZ] = {'\0'};
    int total = 100;
    
    for (int i = 0; i < total; ++i) {
        strcat(string, "aaa    ");
    }
    count = (int)get_split_params(string, params);
    ASSERT_EQ(count, max);

    for (int i = 0; i < max; ++i) {
        EXPECT_EQ(strcmp(params[i], "aaa"), 0);
    }
}

TEST_F(CommandParserTest, CheckInfoValid) {
    EXPECT_TRUE(is_date_valid("2022-08-04"));
    EXPECT_FALSE(is_date_valid("2022-8-04"));
    EXPECT_FALSE(is_date_valid("2022-08-4"));
    EXPECT_FALSE(is_date_valid("2022-008-04"));

    EXPECT_TRUE(is_name_valid("lisi"));
    EXPECT_FALSE(is_date_valid("lisi5"));
    EXPECT_FALSE(is_date_valid("5lisi"));
    EXPECT_FALSE(is_date_valid("li_si"));
}

TEST_F(CommandParserTest, ParseSortType) {
    sort_type_t type = SORT_NONE;

    type = parse_sort_type("--sort:id");
    EXPECT_EQ(type, SORT_ID);

    type = parse_sort_type("--sort:date");
    EXPECT_EQ(type, SORT_DATE);

    type = parse_sort_type("--sort:name");
    EXPECT_EQ(type, SORT_NONE);

    type = parse_sort_type("invalid");
    EXPECT_EQ(type, SORT_NONE);
}

TEST_F(CommandParserTest, ParseLogLevel) {
    EXPECT_TRUE(parse_log_level("debug"));
    EXPECT_TRUE(parse_log_level("info"));
    EXPECT_TRUE(parse_log_level("error"));

    EXPECT_FALSE(parse_log_level("invalid"));
}

TEST_F(CommandParserTest, ParseStaffInfo) {
    staff_info_t info;

    bzero(&info, sizeof(staff_info_t));
    parse_staff_info("id:1111", &info);
    EXPECT_EQ(info.staff_id, 1111);
    parse_staff_info("name:lisi", &info);
    EXPECT_EQ(strcmp(info.name, "lisi"), 0);
    parse_staff_info("date:2022-06-25", &info);
    EXPECT_EQ(info.date.year, 2022);
    parse_staff_info("dept:CWPP", &info);
    EXPECT_EQ(strcmp(info.department, "CWPP"), 0);
    parse_staff_info("pos:Programmer", &info);
    EXPECT_EQ(strcmp(info.position, "Programmer"), 0);

    EXPECT_FALSE(parse_staff_info("id:invalid", NULL));
    EXPECT_FALSE(parse_staff_info("id:invalid", &info));
    EXPECT_FALSE(parse_staff_info("id1:1111", &info));
    EXPECT_FALSE(parse_staff_info("name:lisi3", &info));
    EXPECT_FALSE(parse_staff_info("date:2202--", &info));
}

TEST_F(CommandParserTest, ParseCommand) {
    user_command_t command = CMD_NUL;

    command = parse_input_command("ADD");
    EXPECT_EQ(command, CMD_ADD);
    command = parse_input_command("del");
    EXPECT_EQ(command, CMD_DEL);
    command = parse_input_command("Mod");
    EXPECT_EQ(command, CMD_MOD);
    command = parse_input_command("gEt");
    EXPECT_EQ(command, CMD_GET);

    command = parse_input_command((char *)"ddd");
    EXPECT_EQ(command, CMD_NUL);
}

TEST_F(CommandParserTest, ParseInput) {
    query_info_t query_info;

    parse_user_input("ADD id:10086 name:Zhangsan\n", &query_info);
    EXPECT_EQ(query_info.command, CMD_ADD);
    EXPECT_EQ(query_info.info.staff_id, 10086);
    EXPECT_EQ(strcmp(query_info.info.name, "Zhangsan"), 0);
    FREE(query_info.info.name)

    parse_user_input("DEL id:10086\n", &query_info);
    EXPECT_EQ(query_info.command, CMD_DEL);
    EXPECT_EQ(query_info.info.staff_id, 10086);

    parse_user_input("MOD id:10086 name:Lisi\n", &query_info);
    EXPECT_EQ(query_info.command, CMD_MOD);
    EXPECT_EQ(query_info.info.staff_id, 10086);
    EXPECT_EQ(strcmp(query_info.info.name, "Lisi"), 0);
    FREE(query_info.info.name)

    parse_user_input("GET --sort:id *\n", &query_info);
    EXPECT_EQ(query_info.command, CMD_GET);
    EXPECT_EQ(query_info.sort_type, SORT_ID);
    EXPECT_TRUE(query_info.is_opt_all);

    parse_user_input("LOG debug\n", &query_info);
    EXPECT_EQ(query_info.command, CMD_LOG);
    parse_user_input("LOG info\n", &query_info);

    EXPECT_FALSE(parse_user_input("NUL\n", &query_info));
    EXPECT_FALSE(parse_user_input("LOG idddd\n", &query_info));
}
