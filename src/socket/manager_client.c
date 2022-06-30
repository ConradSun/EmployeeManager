#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "log.h"

static struct sockaddr_in server_addr;  // 服务端地址
static char *server_ip = "127.0.0.1";   // 服务端ip
static uint16_t server_port = 16166;    // 服务端绑定端口
static int server_fd = -1;              // 服务端文件描述符

#ifndef UNIT_TEST
log_level_t g_log_level = LOG_INFO;     // 当前日志等级[默认INFO级别]
#endif

/**
 * @brief   创建服务端套接字
 * @return  false表示失败，否则成功
 */
STATIC bool create_peer_socket(void) {
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_family = AF_INET;
    server_addr.sin_len = sizeof(struct sockaddr_in);
    server_addr.sin_port = htons(server_port);
    bzero(server_addr.sin_zero, sizeof(server_addr.sin_zero));

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        LOG_C(LOG_ERROR, "Error occured in creating server socket.")
        return false;
    }
    LOG_C(LOG_DEBUG, "Create server socket successfully.")
    return true;
}

/**
 * @brief   连接到服务端
 * @return  false表示失败，否则成功
 */
STATIC bool establish_connection(void) {
    int result = connect(server_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in));
    if (result != 0) {
        LOG_C(LOG_ERROR, "Error occured in connecting.")
        return false;
    }
    LOG_C(LOG_DEBUG, "Connect with server successfully.")
    return true;
}

/**
 * @brief           处理本地输入
 * @param input_msg 输入缓存
 * @param size      缓存大小
 */
STATIC void process_input_message(char *input_msg, size_t size) {
    char *input = readline("client> ");
    if (input == NULL || strlen(input) == 0) {
        process_input_message(input_msg, size);
    }

    add_history(input);
    bzero(input_msg, size);
    snprintf(input_msg, size, "%s\n", input);
    free(input);
    send(server_fd, input_msg, BUFSIZ, 0);
}

/**
 * @brief               接收远程信息
 * @param output_msg    接收缓存
 * @param size          缓存大小
 */
STATIC void receive_message(char *output_msg, size_t size) {
    bzero(output_msg, size);
    int msg_size = recv(server_fd, output_msg, size, 0);

    if (msg_size < 0) {
        LOG_C(LOG_ERROR, "Error occured in recviving message.")
        return;
    }
    if (msg_size == 0) {
        LOG_O("Server is exited, now quit.")
        close(server_fd);
        exit(0);
    }
    LOG_O("%s", output_msg)
}

/**
 * @brief   初始化客户端
 * @return  false表示失败，否则为成功
 */
STATIC bool init_socket_client(void) {
    if (!create_peer_socket()) {
        return false;
    }
    if (!establish_connection()) {
        return false;
    }
    return true;
}

#ifndef UNIT_TEST
int main(int argc, const char *argv[]) {
    char input_msg[BUFSIZ];
    char output_msg[BUFSIZ];

    if (argc >=2 && argv[1] != NULL) {
        server_ip = (char *)argv[1];
    }
    if (!init_socket_client()) {
        return -1;
    }
    
    while (true) {
        process_input_message(input_msg, BUFSIZ);
        receive_message(output_msg, BUFSIZ);
    }
    
    return 0;
}
#endif
