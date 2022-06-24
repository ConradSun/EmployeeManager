//
//  manager_server.h
//  EmployeeManager
//
//  Created by 孙康 on 2022/06/16.
//

#include "manager_server.h"
#include "command_parser.h"
#include "log.h"
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

static const int backlog = 5;               // 并发处理连接请求数量
static const uint8_t max_clients = 8;       // 最大连接客户端数量
static uint16_t server_port = 16166;        // 服务端绑定端口
static fd_set server_set = {0};             // 服务端描述符集
static int server_fd = -1;                  // 服务端文件描述符
static struct sockaddr_in server_addr;      // 服务端地址
static int clients_fd[max_clients] = {0};   // 客户端文件描述符

/**
 * @brief   创建服务端套接字
 * @return  false表示失败，否则成功
 */
static bool create_server_socket(void) {
    int option = 1;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_family = AF_INET;
    server_addr.sin_len = sizeof(struct sockaddr_in);
    server_addr.sin_port = htons(server_port);
    bzero(server_addr.sin_zero, sizeof(server_addr.sin_zero));

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        LOG_C(LOG_ERROR, "Error occured in creating server socket.")
        return false;
    }
    // 设置端口复用[SO_REUSEADDR为1]
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    LOG_C(LOG_DEBUG, "Create server socket successfully.")
    return true;
}

/**
 * @brief   绑定端口开始监听
 * @return  false表示失败，否则成功
 */
static bool wait_for_connect(void) {
    int result = -1;
    result = bind(server_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in));
    if (result == -1) {
        LOG_C(LOG_ERROR, "Error occured in binding server socket.")
        return false;
    }

    result = listen(server_fd, backlog);
    if (result == -1) {
        LOG_C(LOG_ERROR, "Error occured in listening port.")
        return false;
    }
    return true;
}

/**
 * @brief           等待用户输入/远程消息
 * @param max_fd    管理fd数量
 * @return          false表示无输入，否则有消息
 */
static bool is_message_available(int *max_fd) {
    FD_ZERO(&server_set);
    FD_SET(STDIN_FILENO, &server_set);
    *max_fd = *max_fd < STDIN_FILENO ? STDIN_FILENO : *max_fd;
    FD_SET(server_fd, &server_set);
    *max_fd = *max_fd < server_fd ? server_fd : *max_fd;

    for (uint8_t i = 0; i < max_clients; ++i) {
        if (clients_fd[i] != 0) {
            FD_SET(clients_fd[i], &server_set);
            *max_fd = *max_fd < clients_fd[i] ? clients_fd[i] : *max_fd;
        }
    }
    
    int result = select(*max_fd + 1, &server_set, NULL, NULL, NULL);
    if (result < 0) {
        LOG_C(LOG_ERROR, "Error occured in selecting server fd.")
        return false;
    }
    if (result == 0) {
        LOG_C(LOG_DEBUG, "Selecting server fd timeout.")
        return false;
    }
    LOG_C(LOG_DEBUG, "Message is available now.")

    return true;
}

/**
 * @brief           发送查询结果
 * @param client_fd 客户端fd
 * @param result    查询结果
 */
static void send_query_result(uint8_t client_fd, char *result) {
    if (!FD_ISSET(client_fd, &server_set) || result == NULL) {
        return;
    }
    send(client_fd, result, strlen(result), 0);
}

/**
 * @brief           处理用户请求
 * @param input_msg 用户输入
 * @param input_fd  用户fd
 */
static void process_user_query(char *input_msg, uint8_t input_fd) {
    if (input_msg == NULL) {
        return;
    }

    user_request_t user_request = {0};
    user_request.input_fd = input_fd;
    strlcpy(user_request.request, input_msg, BUFSIZ);
    process_input_messgae(&user_request);
    if (input_fd == STDIN_FILENO) {
        LOG_O("%s", user_request.result)
    }
    else {
        send_query_result(input_fd, user_request.result);
    }
}

/**
 * @brief   处理连接请求
 * @return  false表示失败，否则成功
 */
static bool process_connect_request(void) {
    if (!FD_ISSET(server_fd, &server_set)) {
        return true;
    }

    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);
    int temp_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
    if (temp_fd == -1) {
        LOG_C(LOG_DEBUG, "Error occured in accept connection.")
        return false;
    }
    
    int index = -1;
    for (uint8_t i = 0; i < max_clients; i++) {
        if (clients_fd[i] == 0) {
            index = i;
            clients_fd[i] = temp_fd;
            break;
        }
    }
    if (index >= 0) {
        LOG_C(LOG_INFO, "NO[%d] client[%s:%d] connect successfully.\n", index, 
        inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    }
    else {
        LOG_C(LOG_INFO, "Error occured in accepting new clinet[%s:%d] for no more space.\n", 
        inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    }

    return true;
}

/**
 * @brief 获取并处理本地输入请求
 */
static void process_local_query(void) {
    char input_msg[BUFSIZ] = {'\0'};

    if (FD_ISSET(STDIN_FILENO, &server_set)) {
        if (fgets(input_msg, BUFSIZ, stdin) == NULL) {
            LOG_O("Ignore EOF.")
            return;
        }
        process_user_query(input_msg, STDIN_FILENO);
    }
}

/**
 * @brief 获取并处理远程输入请求
 */
static void process_remote_query(void) {
    char input_msg[BUFSIZ] = {'\0'};

    for (uint8_t i = 0; i < max_clients; i++) {
        if (clients_fd[i] == 0 || !FD_ISSET(clients_fd[i], &server_set)) {
            continue;
        }

        int msg_size = recv(clients_fd[i], input_msg, BUFSIZ, 0);
        if (msg_size < 0) {
            LOG_C(LOG_DEBUG, "Error occured in recviving message.")
            continue;
        }
        if (msg_size == 0) {
            LOG_C(LOG_INFO, "NO[%d] client is exited.", i)
            clients_fd[i] = 0;
            continue;
        }
        if (strlen(input_msg) <= 1) {
            continue;
        }

        msg_size = msg_size > BUFSIZ ? BUFSIZ: msg_size;
        LOG_C(LOG_INFO, "%s", input_msg)
        process_user_query(input_msg, clients_fd[i]);
        bzero(input_msg, BUFSIZ);
    }
}

/**
 * @brief   初始化服务端
 * @return  false表示失败，否则成功
 */
bool init_socket_server(void) {
    if (!create_server_socket()) {
        return false;
    }
    if (!wait_for_connect()) {
        return false;
    }
    LOG_C(LOG_INFO, "Init socket successfully, ready for connecting.")
    return true;
}

/**
 * @brief 处理所有查询请求
 */
void process_all_requests(void) {
    int max_fd = -1;
    if (!is_message_available(&max_fd)) {
        return;
    }

    process_connect_request();
    process_local_query();
    process_remote_query();
}

/**
 * @brief 断开所有连接
 */
void destroy_all_connection(void) {
    for (uint8_t i = 0; i < max_clients; ++i) {
        if (clients_fd[i] != 0) {
            close(clients_fd[i]);
        }
    }
    close(server_fd);
    server_fd = -1;
}
