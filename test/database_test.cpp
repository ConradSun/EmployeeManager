//
//  database_test.c
//  EmployeeManager
//
//  Created by 孙康 on 2022/6/24.
//

#ifdef __cplusplus
extern "C" {
#endif

#include "hash_table.h"
#include "common.h"

extern void clear_value(void *value);
extern void copy_value(void *dst, const void *src);
extern bool is_value_equal(const void *src, const void *dst);

#ifdef __cplusplus
};
#endif

#include <gtest/gtest.h>

class HashTableTest: public testing::Test {
};

static table_init_config_t s_init_config = {
    .max_size = 4,
    .value_size = sizeof(staff_info_t),
    .clear_func = clear_value,
    .copy_func = copy_value,
    .match_func = is_value_equal
};

TEST_F(HashTableTest, Create) {
    hash_table_t *hash_table = NULL;
    table_init_config_t config = s_init_config;

    // 非法参数
    hash_table = create_hash_table(NULL);
    EXPECT_TRUE(hash_table == NULL);

    config.max_size = 0;
    hash_table = create_hash_table(&config);
    EXPECT_TRUE(hash_table == NULL);
    config.max_size = 4;

    config.value_size = 0;
    hash_table = create_hash_table(&config);
    EXPECT_TRUE(hash_table == NULL);
    config.value_size = 4;

    config.clear_func = NULL;
    hash_table = create_hash_table(&config);
    EXPECT_TRUE(hash_table == NULL);
    config.clear_func = clear_value;

    // 正常创建
    hash_table = create_hash_table(&config);
    EXPECT_FALSE(hash_table == NULL);
    delete_hash_table(&hash_table);
}

TEST_F(HashTableTest, Delete) {
    hash_table_t *hash_table = NULL;

    // 非法参数
    delete_hash_table(NULL);
    delete_hash_table(&hash_table);

    // 正常流程
    hash_table = create_hash_table(&s_init_config);
    EXPECT_FALSE(hash_table == NULL);
    delete_hash_table(&hash_table);
    EXPECT_TRUE(hash_table == NULL);
}

TEST_F(HashTableTest, AddItem) {
    hash_table_t *hash_table = NULL;
    uint64_t key = 1;
    staff_info_t info = {0};

    hash_table = create_hash_table(&s_init_config);
    ASSERT_FALSE(hash_table == NULL);

    // 非法参数
    EXPECT_FALSE(add_item_to_table(&hash_table, 0, &info, true));
    EXPECT_FALSE(add_item_to_table(&hash_table, 1, NULL, true));

    // 不允许插入相同主键的信息
    EXPECT_TRUE(add_item_to_table(&hash_table, key, &info, true));
    EXPECT_FALSE(add_item_to_table(&hash_table, key, &info, true));

    EXPECT_TRUE(add_item_to_table(&hash_table, ++key, &info, true));
    EXPECT_TRUE(add_item_to_table(&hash_table, ++key, &info, true));

    EXPECT_EQ(get_count_from_table(hash_table), key);
    delete_hash_table(&hash_table);
}

TEST_F(HashTableTest, RemoveItem) {
    hash_table_t *hash_table = NULL;
    uint64_t max_size = s_init_config.max_size;
    staff_info_t info = {0};

    hash_table = create_hash_table(&s_init_config);
    ASSERT_FALSE(hash_table == NULL);

    for (int i = 1; i <= max_size * 4; i++) {
        add_item_to_table(&hash_table, i, &info, true);
    }
    EXPECT_EQ(get_count_from_table(hash_table), max_size * 4);
    for (int i = 1; i <= max_size * 2; i++) {
        remove_item_from_table(hash_table, i);
    }
    EXPECT_EQ(get_count_from_table(hash_table), max_size * 2);

    // 非法参数
    EXPECT_FALSE(remove_item_from_table(hash_table, 0));
    // 主键不存在
    EXPECT_FALSE(remove_item_from_table(hash_table, 10086));
    delete_hash_table(&hash_table);
}

TEST_F(HashTableTest, ModifyItem) {
    hash_table_t *hash_table = NULL;
    uint64_t key = 10086;
    staff_info_t *check_info = NULL;
    staff_info_t info = {
        .staff_id = 10086,
        .name = (char *)"ZhangSan",
        .date = {
            .year = 2022,
            .month = 6,
            .day = 24
        },
        .department = (char *)"ZTA",
        .position = (char *)""
    };

    hash_table = create_hash_table(&s_init_config);
    add_item_to_table(&hash_table, key, &info, true);
    info.name = (char *)"Lisi";
    EXPECT_TRUE(modify_item_from_table(hash_table, key, &info));
    check_info = (staff_info_t *)get_item_by_key(hash_table, key);
    EXPECT_EQ(strcmp(check_info->name, "Lisi"), 0);

    info.date.day = 0;
    EXPECT_TRUE(modify_item_from_table(hash_table, key, &info));
    check_info = (staff_info_t *)get_item_by_key(hash_table, key);
    EXPECT_EQ(info.date.day, 0);

    // 非法参数
    EXPECT_FALSE(modify_item_from_table(hash_table, key, NULL));
    // 主键查找失败
    EXPECT_FALSE(modify_item_from_table(hash_table, key+1, &info));
    delete_hash_table(&hash_table);
}

TEST_F(HashTableTest, GetItem) {
    hash_table_t *hash_table = NULL;
    uint64_t max_size = s_init_config.max_size;
    staff_info_t info = {0};
    staff_info_t *check_info = NULL;
    staff_info_t **check_infos = NULL;
    uint64_t count = 0;

    hash_table = create_hash_table(&s_init_config);
    ASSERT_FALSE(hash_table == NULL);
    for (int i = 1; i <= max_size * 4; i++) {
        info.date.year = i;
        add_item_to_table(&hash_table, i, &info, true);
    }
    for (int i = 1; i <= max_size * 4; i++) {
        check_info = (staff_info_t *)get_item_by_key(hash_table, i);
        EXPECT_FALSE(check_info == NULL);
        EXPECT_EQ(check_info->date.year, i);
    }
    check_infos = (staff_info_t **)get_items_by_value(hash_table, NULL, &count);
    EXPECT_FALSE(check_infos == NULL);
    EXPECT_EQ(count, max_size * 4);
    FREE(check_infos)

    EXPECT_TRUE(get_items_by_value(hash_table, NULL, NULL) == NULL);
    // 非法参数
    EXPECT_TRUE(get_item_by_key(hash_table, 0) == NULL);
    // 主键不在表内
    EXPECT_TRUE(get_item_by_key(hash_table, 10086) == NULL);
    delete_hash_table(&hash_table);
}