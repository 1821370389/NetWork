#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <arpa/inet.h>


/* 状态码 */
enum STATUS_CODE
{
    SUCCESS = 0,        // 成功
    FAILURE = -1,       // 失败
    ERROR = -2,         // 错误
    EXIT = -3,          // 退出
    TIMEOUT = -4,       // 超时
    NOT_FOUND = -5,     // 未找到
    DUPLICATE = -6,     // 重复
};

#define SERVER_PORT 8080        // 服务器端口号
#define SERVER_IP "172.16.157.11" // 服务器IP地址
#define BUFFER_SIZE 1024        // 缓冲区大小

int main()
{
    /* 创建套接字 */
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket error");
        exit(1);
    }

    /* 绑定服务器地址 */
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    // int ret = inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);
    // if (ret <= 0)
    // {
    //     perror("inet_pton error");
    //     exit(1);
    // }
    /* 连接服务器 */
    int ret = connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (ret == -1)
    {
        perror("connect error");
        exit(1);
    }
    /* 发送数据 */
    printf("send data to server...\n");
    char buffer[BUFFER_SIZE] = {0};
    char recvbuffer[BUFFER_SIZE] = {0};


    if (strcmp(recvbuffer, "Good Night World") == 0)
    {
        strncpy(buffer, "Hello Server", BUFFER_SIZE);
    }
    else if (strcmp(recvbuffer,"Hello Client") == 0)
    {
        strncpy(buffer, "Good Night World", BUFFER_SIZE);
    }
    else
    {
        strncpy(buffer, "Hello World", BUFFER_SIZE);
    }
    
    memset(recvbuffer, 0, BUFFER_SIZE);
    printf("send data: %s\n", buffer);
    send(sockfd, buffer, sizeof(buffer), 0);
    
    int n = recv(sockfd, recvbuffer, BUFFER_SIZE, 0);
    if (n > 0)
    {

        printf("receive data from server: %s\n", recvbuffer);
    }

    close(sockfd);
    return 0;
}