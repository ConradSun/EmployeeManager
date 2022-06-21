#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "log.h"

static uint16_t server_port = 16166;    // 服务端绑定端口
static fd_set client_set = {0};         // 服务端描述符集
static int server_fd = -1;              // 服务端文件描述符
static struct sockaddr_in server_addr;  // 服务端地址
static char *server_ip = "127.0.0.1";   // 服务端ip
log_level_t g_log_level = LOG_INFO;     // 当前日志等级[默认INFO级别]

/**
 * @brief   创建服务端套接字
 * @return  false表示失败，否则成功
 */
static bool create_server_socket(void) {
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
static bool establish_connection(void) {
    int result = connect(server_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in));
    if (result != 0) {
        LOG_C(LOG_ERROR, "Error occured in connecting.")
        return false;
    }
    LOG_C(LOG_DEBUG, "Connect with server successfully.")
    return true;
}

/**
 * @brief   等待用户输入/远程消息
 * @return  false表示无输入，否则有消息
 */
static bool is_message_available() {
    FD_ZERO(&client_set);
    FD_SET(STDIN_FILENO, &client_set);
    FD_SET(server_fd, &client_set);
    
    int result = select(server_fd + 1, &client_set, NULL, NULL, NULL);
    if (result < 0) {
        LOG_C(LOG_ERROR, "Error occured in selecting client fd.")
        return false;
    }
    if (result == 0) {
        LOG_C(LOG_ERROR, "Selecting client fd timeout.")
        return false;
    }

    return true;
}

/**
 * @brief   处理本地输入
 * @return  false表示无输入，否则有消息
 */
static void process_input_message(char *input_msg, size_t size) {
    if (!FD_ISSET(STDIN_FILENO, &client_set)) {
        return;
    }

    bzero(input_msg, size);
    if (fgets(input_msg, size, stdin) == NULL) {
        LOG_O("Read EOF, now qiut.")
        exit(0);
    }
    send(server_fd, input_msg, size, 0);
}

/**
 * @brief               接收远程信息
 * @param output_msg    接收缓存
 * @param size          缓存大小
 */
static void receive_message(char *output_msg, size_t size) {
    if (!FD_ISSET(server_fd, &client_set)) {
        return;
    }

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

    msg_size = msg_size > size ? size: msg_size;
    LOG_O("%s", output_msg)
}

int main(int argc, const char *argv[]) {
    char input_msg[BUFSIZ];
    char output_msg[BUFSIZ];

    if (argc >=2 && argv[1] != NULL) {
        server_ip = (char *)argv[1];
    }
    if (!create_server_socket()) {
        return -1;
    }
    if (!establish_connection()) {
        return -1;
    }
    
    while (true) {
        if (is_message_available()) {
            process_input_message(input_msg, BUFSIZ);
            receive_message(output_msg, BUFSIZ);
        }
    }
    
    return 0;
}