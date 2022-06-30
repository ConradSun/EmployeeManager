//
//  socket_test.c
//  EmployeeManager
//
//  Created by 孙康 on 2022/6/25.
//

#ifdef __cplusplus
extern "C" {
#endif

#include "manager_server.h"
#include "database_manager.h"
#include "command_execution.h"

extern bool init_socket_client(void);
extern void receive_message(char *output_msg, size_t size);
extern void process_input_message(char *input_msg, size_t size);

#ifdef __cplusplus
};
#endif

#include <gtest/gtest.h>
#include <sys/wait.h>

class SocketTest: public testing::Test {
    virtual void SetUp() override {
        create_database();
    }
    virtual void TearDown() override {
        delete_database();
    }
};

TEST_F(SocketTest, Communicate) {
    pid_t pid = fork();
    int state = -1;
    ASSERT_GE(pid, 0);

    //  客户端
    if (pid == 0) {
        char input[BUFSIZ] = "ADD id:10088 name:Lisi date:2022-05-19 dept:CWPP pos:engineer\n";
        char output[BUFSIZ] = "\0";
        int pipe_fds[2] = {0};

        // 标准输入重定向
        ASSERT_EQ(pipe(pipe_fds), 0);
        dup2(pipe_fds[0], STDIN_FILENO);
        
        // 服务端不存在
        EXPECT_FALSE(init_socket_client());
        usleep(1000);
        // 服务端存在   1
        EXPECT_TRUE(init_socket_client());
        // 处理输入 2
        write(pipe_fds[1], input, BUFSIZ);
        process_input_message(input, BUFSIZ);
        // 接收输入处理结果
        receive_message(output, BUFSIZ);
        EXPECT_EQ(strcmp(output, "The staff [10088] is added."), 0);

        exit(0);
    }
    // 服务端
    else {
        int pipe_fds[2] = {0};

        // 标准输入重定向
        ASSERT_EQ(pipe(pipe_fds), 0);
        dup2(pipe_fds[0], STDIN_FILENO);

        usleep(500);
        EXPECT_TRUE(init_socket_server());
        write(pipe_fds[1], "ADD id:invalid\n", BUFSIZ);
        usleep(500);
        for (int i = 0; i < 2; i++) {
            process_all_requests();
        }  
        
        wait(&state);
        uninit_socket_server();
    }
}
